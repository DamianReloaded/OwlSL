/*
OwlSL (Owl Script Language)

Copyright (c) 2013-2014 Damian Reloaded <>

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#include "vm.h"
#include "types/types.h"
using namespace owlsl;

/* Hack to avoid compiler bug in win32 scalar deleting destructor'() */
#ifdef WIN32
	void ForceVectors()
	{
		unsigned *pClass = new unsigned[2];
	}
	static void (*pFunc)() = ForceVectors;
#endif
/* Hack to avoid compiler bug in win32 scalar deleting destructor'() */

vm::vm()
{
	rtx = -1;
	heap.vm = this;
	op.push_back(&vm::pushctx);	// 0
  	op.push_back(&vm::popctx);	// 1
	op.push_back(&vm::push);	// 2
	op.push_back(&vm::pushs);	// 3
	op.push_back(&vm::pop);		// 4
	op.push_back(&vm::palloc);	// 5
	op.push_back(&vm::pdalloc);	// 6
	op.push_back(&vm::pcall);	// 7
	op.push_back(&vm::salloc);	// 8
	op.push_back(&vm::sdalloc);	// 9
	op.push_back(&vm::scall);	// 10
	op.push_back(&vm::sreturn0);// 11
	op.push_back(&vm::sreturn1);// 12
	op.push_back(&vm::jump);	// 13
	op.push_back(&vm::end);		// 14
	op.push_back(&vm::pushc);	// 15
	op.push_back(&vm::ialloc);	// 16
	op.push_back(&vm::jumpt);	// 17
	op.push_back(&vm::jumpf);	// 18
}

vm::~vm()
{
	heap.terminate();
}

int vm::new_type(const owlsl::text& name, const bool& scripted)
{
	if (table.contains(name))
	{
		return table.id(name);
	}
	return table.add(name, vtable(scripted));
}

void vm::pushctx()
{
	push_context();
}

void vm::push_context()
{
	context.push_back(stack.size()-1);
}

void vm::popctx()
{
	pop_context();
}

void vm::pop_context()
{
	// pop all references in the last stack context
	pop((stack.size()-1)-context.back());
	context.pop_back();
}

void vm::push()
{
	int pos = code[++cursor];
	push(pos);
}

int vm::push(const int& pos)
{
	heap.mem[pos].refcount++;
	stack.push_back(pos);
	return stack.size()-1;
}

void vm::pushs()
{
	int pos = code[++cursor];
	int spos = func_stack.back()+pos;
	spos = spos<0?0:spos; // hack cause I don't currently feel like going down this rabbit hole... ^_^	
	push(stack[spos]);
}

void vm::pushc()
{
	int pos = code[++cursor];
	unsigned levels = code[++cursor];
	int stack_pos = (pos>-1) ? func_stack.back()+pos : stack.size()+pos;
	stype* t = heap.cast<stype>(stack[stack_pos]);
	pos = t->prop[code[++cursor]];
	for (unsigned i=1; i<levels; i++)
	{
		stype* t = heap.cast<stype>(pos);
		pos = t->prop[code[++cursor]];
	}
	push(pos);
}

void vm::pop()
{
	pop(1);
}

int vm::pop(const unsigned& amount)
{
	int pos = 0;
	unsigned size = stack.size();
	unsigned newsize = size-amount;
	for (unsigned i = newsize; i<size; i++)
	{
		int pos = stack[i];
		var& v = heap.mem[pos];
		if (v.refcount>0) 
		{
			v.refcount--;
			if (v.refcount<1)
			{
				if (v.owned)
				{
					if (table[v.type].scripted)
					{
						sdestruct(pos);
					}
					else
					{
						pdestruct(pos);
					}
				}
			}
		}
	}
	stack.resize(stack.size()-amount);
	return pos;
}

// +1 type, +2 constructor
void vm::palloc()
{
	int type = code[++cursor];
	int constructor = code[++cursor];
	int pos = pconstruct(type, constructor);
	push(pos);
}

int vm::pconstruct(const int& type, const int& constructor) // Variable type, constructor id
{
	return table[type].pconstruct[constructor](this, type);
}

void vm::pdalloc()
{
	int pos = code[++cursor];
	pdestruct(pos);
}

void vm::pdestruct(const int& pos)
{
	if (table[heap.mem[pos].type].pdestruct != NULL)
	{
		table[heap.mem[pos].type].pdestruct(this, pos);
	}
	else
	{
		heap.unused.push_back(pos);
	}
}

void vm::pcall()
{
	int type = code[++cursor];
	int method = code[++cursor];
	pcallmethod(type, method);
}

void vm::pcallmethod(const int& type, const int& method)
{
	table[type].pmethod[method](this);
	//pop(table[type].pparameters[method]+1); // pop parameters + this
	// push return value if any
	/*if (rtx>-1)
	{
		heap.mem[rtx].refcount--;
		push(rtx);
		rtx = -1;
	}*/
}

void vm::preturn (const int& pos)
{
	rtx = pos;
}

int vm::heap_pos (const int& relative_pos)
{
	return stack[(stack.size()-1)+relative_pos];
}

void vm::salloc()
{
	int type = code[++cursor];
	int constructor = code[++cursor];
	salloc(type, constructor);
}

int vm::salloc (const int& type, const int& constructor)
{
	int pos = sconstruct(type, constructor);
	push(pos);
	return pos;
}

int	vm::sconstruct (const int& type, const int& constructor)
{
	int pos = heap.palloc<stype>(type, new stype());
	stype* t = heap.cast<stype>(pos);

	unsigned num_props = table[type].sproperty.size();
	t->prop.resize(num_props);
	for (unsigned i=0; i<num_props; i++)
	{
		unsigned ptype = table[type].sproperty[i];
		if (table[ptype].scripted)
		{
			t->prop[i] = sconstruct(ptype, 0); // construct using default constructor
		}
		else
		{
			t->prop[i] = pconstruct(ptype, 0); // construct using default constructor
		}
		heap.mem[t->prop[i]].refcount++; // increment reference because the parent points to it
	}
	return pos;
}

void vm::sdalloc()
{
	int pos = code[++cursor];
	sdestruct(pos);
}

void vm::sdestruct(const int& pos)
{
	unsigned type = heap.mem[pos].type;
	stype* t = heap.cast<stype>(pos);
	unsigned num_props = t->prop.size();
	for (unsigned i=0; i<num_props; i++)
	{
		unsigned ptype = table[type].sproperty[i];
		if (table[ptype].scripted)
		{
			sdestruct(t->prop[i]);
		}
		else
		{
			pdestruct(t->prop[i]);
		}
	}
	t->prop.clear();
	heap.pdalloc<stype>(pos);
}

void vm::scall()
{
	int type = code[++cursor];
	int method = code[++cursor];
	scallmethod(type, method);
}

void vm::scallmethod (const int& type, const int& method, const bool& static_call)
{
	callstack.push_back(cursor); // save current cursor location so the run loop will increment it in the next step
	
	func_stack.push_back(stack.size()-1);
	
	cursor = table[type].smethod[method]; // set the cursor to the start of the method's code
	run(cursor);
	pop(table[type].sparameters[method]+ ( (static_call)?0:1 ) ); // pop parameters + this
	// push return value if any
	if (rtx>-1)
	{
		heap.mem[rtx].refcount--;
		push(rtx);
		rtx = -1;
	}
	cursor = callstack.back();
	callstack.pop_back();
	func_stack.pop_back();
}

void vm::end ()
{
	// Jump to the end of the program
	cursor = code.size();
}

void vm::sreturn0()
{

}

void vm::sreturn1()
{
	// get return value (if any)
	rtx = stack.back();
	heap.mem[rtx].refcount++;
	pop();

}

void vm::ialloc()
{
	int type = code[++cursor];
	int method = code[++cursor];
	pcallmethod(type, method);
}

void vm::jump ()
{
    ++cursor;
	cursor = code[cursor];
}

void vm::jumpt ()
{
	owlsl::type::boolean* b = heap.cast<owlsl::type::boolean>(stack.back());
	++cursor; // always increment to pass over the parameter
	if (b->get()) cursor = code[cursor]; // Check if true
	pop(); // pop the evaluated value
}

void vm::jumpf ()
{
	owlsl::type::boolean* b = heap.cast<owlsl::type::boolean>(stack.back());
	++cursor; // always increment to pass over the parameter
	if (!b->get()) cursor = code[cursor]; // Check if false
	pop(); // pop the evaluated value
}

int vm::run(unsigned cur)
{
	if (cur==0) cur=cursor;
	unsigned size = code.size();
	for(cursor = cur;cursor<size; ++cursor)
	{
		(this->*op[code[cursor]])();
 	}

	return 0;
}

int	vm::run_main ()
{
	callstack.push_back(code.size());
	return run(entry_point);
}

void vm::ins(const int& val)
{
	code.push_back(val);
}

void vm::ins (const int& val, const owlsl::text& dbg_text)
{
	ins(val);
	code_dbg.push_back(dbg_text);
}

void vm::dec_ref_count (const int& pos)
{
	var& v = heap.mem[pos];
	if (v.refcount>0) v.refcount--;
	if (v.refcount<1)
	{
		if (v.owned)
		{
			if (table[v.type].scripted)
			{
				sdestruct(pos);
			}
			else
			{
				pdestruct(pos);
			}
		}
	}
}
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

#include "reference.h"
#include "base.h"
#include "../sco.h"
#include <iostream>
using namespace owlsl::type;

int reference::m_type = -1;

reference::reference()
{
	
}

reference::~reference()
{
	
}

void reference::inscribe (owlsl::sco* sco)
{
	m_type = sco->vm.new_type("ref");
	sco->vm.table[m_type].pspecialize = &specialize; // Set specialization function pointer
}

void reference::specialize (owlsl::sco* sco, const owlsl::text& new_type, std::deque<owlsl::text>& ttypes)
{
	if (sco->vm.table.contains(new_type)) return; // Already defined

	// Set the specialized methods
	owlsl::text method_set = "void =(0)";
	owlsl::text method_set_ref = "void =(0)";
	owlsl::text method_get = "0 ref()";

	method_set.replace("0", new_type);
	for (int32_t i=0; i<(int32_t)ttypes.size(); i++)
	{
		method_set_ref.replace(owlsl::text(i), ttypes[i]);
		method_get.replace(owlsl::text(i), ttypes[i]);
	}

	int type_id = sco->vm.new_type(new_type);
	sco->constructor(type_id, new_type+"()", &base::alloc<reference>);
	sco->destructor(type_id, &reference::script_dalloc);
	sco->method(type_id, method_set, &reference::script_set);
	sco->method(type_id, method_set_ref, &reference::script_set_ref);
	sco->method(type_id, method_get, &reference::script_ref);
	sco->method(type_id, "void clear()",&reference::script_clear);
}

///////////////////////////////////////////////////////////////
// Script Interface Methods
///////////////////////////////////////////////////////////////

int reference::script_dalloc(owlsl::vm* vm, const int& pos)
{
	reference* ref_this = vm->heap.cast<reference>(pos); // this
	if (ref_this->m_ref>-1) vm->dec_ref_count(ref_this->m_ref); 
	return vm->heap.pdalloc<reference>(pos);
}

void reference::script_set(owlsl::vm* vm)
{
	reference* ref_this = vm->stack_cast<reference>(-1); // this
	reference* ref_param = vm->stack_cast<reference>(0);  // parameter
	
	// increment reference count to prevent variable going out of scope
	vm->heap.mem[ref_param->m_ref].refcount++; 
	// Copy the lists
	ref_this->m_ref = ref_param->m_ref;
	// pop parameter, pop this
	vm->pop(2); 
}

void reference::script_set_ref(owlsl::vm* vm)
{
	reference*	ref_this = vm->stack_cast<reference>(-1); // this
	int			param_pos = vm->stack.back();  // parameter
	
	// increment reference count to prevent variable going out of scope
	vm->heap.mem[param_pos].refcount++; 
	// Copy the lists
	ref_this->m_ref = param_pos;
	// pop parameter, pop this
	vm->pop(2); 
}

void reference::script_ref(owlsl::vm* vm)
{
	reference* ref_this = vm->stack_cast<reference>(0); // this
	vm->pop(1); // pop this

	vm->push(ref_this->m_ref);
}

void reference::script_clear(owlsl::vm* vm)
{
	reference* ref_this = vm->stack_cast<reference>(0); // this
	vm->dec_ref_count(ref_this->m_ref); 
	ref_this->m_ref = -1;
	vm->pop(1); // pop this
}
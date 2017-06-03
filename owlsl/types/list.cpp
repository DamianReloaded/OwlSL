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

#include "list.h"
#include "base.h"
#include "number.h"
#include "../sco.h"
#include <iostream>
using namespace owlsl::type;

int list::m_type = 0;

list::list()
{

}

list::~list()
{

}

void list::inscribe (owlsl::sco* sco)
{
	m_type = sco->vm.new_type("list");
	sco->vm.table[m_type].pspecialize = &specialize; // Set specialization function pointer
}

void list::specialize (owlsl::sco* sco, const owlsl::text& new_type, std::deque<owlsl::text>& ttypes)
{
	if (sco->vm.table.contains(new_type)) return; // Already defined

	// Set the specialized methods
	owlsl::text method_set = "void =(0)";
	owlsl::text method_add = "void add(0)";
	owlsl::text method_at  = "0 at(number)";

	method_set.replace("0", new_type);
	for (int32_t i=0; i<(int32_t)ttypes.size(); i++)
	{
		method_add.replace(owlsl::text(i), ttypes[i]);
		method_at.replace(owlsl::text(i), ttypes[i]);
	}

	int type_id = sco->vm.new_type(new_type);
	sco->constructor(type_id, new_type+"()", &base::alloc<list>);
	sco->destructor(type_id, &list::script_dalloc);
	sco->method(type_id, method_set, &list::script_set);
	sco->method(type_id, method_add, &list::script_add);
	sco->method(type_id, method_at, &list::script_at);
	sco->method(type_id, "number count()", &list::script_count);
	sco->method(type_id, "void clear()", &list::script_clear);
	sco->method(type_id, "void print(text)", &list::script_borrame_print);
}

void list::script_borrame_print(owlsl::vm* vm)
{
	owlsl::text t = *vm->stack_cast<owlsl::text>(0);  // parameter
	vm->pop(2); // pop parameter, pop this
	std::wcout << t.wstring() <<std::endl;
}

///////////////////////////////////////////////////////////////
// Script Interface Methods
///////////////////////////////////////////////////////////////

int list::script_dalloc(owlsl::vm* vm, const int& pos)
{
	list* lst_this = vm->heap.cast<list>(pos); // this
	for (size_t i=0; i<lst_this->items.size(); i++) vm->dec_ref_count(lst_this->items[i]); 
	return vm->heap.pdalloc<list>(pos);
}

void list::script_set(owlsl::vm* vm)
{
	list* lst_this = vm->stack_cast<list>(-1); // this
	list* lst_param = vm->stack_cast<list>(0);  // parameter
	
	// increment reference count to prevent variable going out of scope
	for (size_t i=0; i<lst_param->items.size(); i++) vm->heap.mem[lst_param->items[i]].refcount++; 
	// Copy the lists
	lst_this->items = lst_param->items;
	// pop parameter, pop this
	vm->pop(2); 
}

void list::script_add(owlsl::vm* vm)
{
	list* lst = vm->stack_cast<list>(-1); // this
	int param_pos = vm->stack.back();  // parameter
	vm->heap.mem[param_pos].refcount++; // increment reference count to prevent variable going out of scope
	vm->pop(2); // pop parameter, pop this
	lst->items.push_back(param_pos);
}

void list::script_at(owlsl::vm* vm)
{
	list* lst = vm->stack_cast<list>(-1); // this
	size_t pos = (size_t)vm->stack_cast<number>(0)->value;  // parameter
	vm->pop(2); // pop parameter, pop this

	vm->push(lst->items[pos]);
}

void list::script_count(owlsl::vm* vm)
{
	list* lst = vm->stack_cast<list>(0); // this
	vm->pop(1); // pop this

	int type_name = vm->table.id("number");
	int constructor = vm->table[type_name].pconstruct.id("number()");

	int pos = vm->pconstruct(type_name, constructor);
	number* data = vm->heap.cast<number>(pos);
	data->set(lst->items.size());
	vm->push(pos);
}

void list::script_clear(owlsl::vm* vm)
{
	list* lst_this = vm->stack_cast<list>(0); // this
	for (size_t i=0; i<lst_this->items.size(); i++) vm->dec_ref_count(lst_this->items[i]); 
	std::deque<int>().swap(lst_this->items); //clear
	vm->pop(1); // pop this
}
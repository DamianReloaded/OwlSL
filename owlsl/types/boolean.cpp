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

#include "boolean.h"
#include "base.h"
#include "../sco.h"
#include <iostream>
using namespace owlsl::type;

int boolean::m_type = 0;

boolean::boolean() 
{

}

boolean::boolean(boolean& val) : value(val.value)
{

}


boolean::~boolean()
{

}

void boolean::inscribe (owlsl::sco* sco)
{
	m_type = sco->vm.new_type("bool");
	sco->constructor(m_type, "bool()", &base::alloc<boolean>);
	sco->constructor(m_type, "bool(bool)", &base::alloc_set<boolean>);
	sco->destructor(m_type, &base::dalloc<boolean>);
	sco->method(m_type, "void =(bool)", &base::set<boolean>);
	sco->method(m_type, "bool ==(bool)", &base::equal<boolean, double>);
	sco->method(m_type, "bool !=(bool)", &base::not_equal<boolean, double>);
	sco->method(m_type, "bool &&(bool)", &boolean::script_land);
	sco->method(m_type, "bool ||(bool)", &boolean::script_lor);
	sco->method(m_type, "text text()", &boolean::script_to_string);
	sco->method(m_type, "bool !()", &boolean::script_lnot); // Unary not
}

///////////////////////////////////////////////////////////////
// Script Interface Methods
///////////////////////////////////////////////////////////////

void boolean::script_to_string(owlsl::vm* vm)
{
	bool val1 = vm->stack_cast<boolean>(0)->get(); // this
	vm->pop(1); // pop this

	int type_text = vm->table.id("text");
	int constructor = vm->table[type_text].pconstruct.id("text()");

	int pos = vm->pconstruct(type_text, constructor);
	owlsl::text* data = vm->heap.cast<owlsl::text>(pos);
	data->from_bool(val1);
	vm->push(pos);
}

void boolean::script_lnot(owlsl::vm* vm)
{
	bool val1 = vm->stack_cast<boolean>(0)->get(); // this
	vm->pop(1); // pop this

	int type_boolean = vm->table.id("bool");
	int constructor = vm->table[type_boolean].pconstruct.id("bool()");

	int pos = vm->pconstruct(type_boolean, constructor);
	boolean* data = vm->heap.cast<boolean>(pos);
	data->set(!val1);
	vm->push(pos);
}

void boolean::script_land(owlsl::vm* vm)
{
	bool val1 = vm->stack_cast<boolean>(-1)->get(); // this
	bool val2 = vm->stack_cast<boolean>(0)->get();  // parameter
	vm->pop(2); // pop parameter, pop this

	int type_boolean = vm->table.id("bool");
	int constructor = vm->table[type_boolean].pconstruct.id("bool()");

	int pos = vm->pconstruct(type_boolean, constructor);
	boolean* data = vm->heap.cast<boolean>(pos);
	data->set(val1&&val2);
	vm->push(pos);
}

void boolean::script_lor(owlsl::vm* vm)
{
	bool val1 = vm->stack_cast<boolean>(-1)->get(); // this
	bool val2 = vm->stack_cast<boolean>(0)->get();  // parameter
	vm->pop(2); // pop parameter, pop this

	int type_boolean = vm->table.id("bool");
	int constructor = vm->table[type_boolean].pconstruct.id("bool()");

	int pos = vm->pconstruct(type_boolean, constructor);
	boolean* data = vm->heap.cast<boolean>(pos);
	data->set(val1||val2);
	vm->push(pos);
}
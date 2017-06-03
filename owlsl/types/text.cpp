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

#include "text.h"
#include "base.h"
#include "number.h"
#include "boolean.h"
#include "../sco.h"
#include <iostream>
using namespace owlsl::type;

text::text()
{
	//std::wcout << "text constructor" <<std::endl; 
}

text::~text()
{
	//std::wcout << "text destructor" <<std::endl; 
}

void text::inscribe (owlsl::sco* sco)
{
	sco->vm.new_type	("text");
	sco->constructor	("text", "text()", &base::alloc<owlsl::text>);
	sco->constructor	("text", "text(text)", &base::alloc_set<owlsl::text>);
	sco->destructor		("text", &base::dalloc<owlsl::text>);
	sco->method			("text", "void =(text)", &text::script_set);
	sco->method			("text", "text ==(text)", &text::script_equal);
	sco->method			("text", "text +(text)", &text::script_add);
	sco->method			("text", "text +(number)", &text::script_add_number);
	sco->method			("text", "text +=(text)", &text::script_add_set);
}

void text::script_set(owlsl::vm* vm)
{
	owlsl::text& param = *vm->stack_cast<owlsl::text>(0);
	owlsl::text& obj = *vm->stack_cast<owlsl::text>(-1);
	obj.set(param);
	vm->pop(2); // pop parameter, pop this
}

void text::script_equal(owlsl::vm* vm)
{
	owlsl::text& val1 = *vm->stack_cast<owlsl::text>(-1); // this
	owlsl::text& val2 = *vm->stack_cast<owlsl::text>(0);  // parameter

	int type_boolean = vm->table.id("bool");
	int constructor = vm->table[type_boolean].pconstruct.id("bool()");

	int pos = vm->pconstruct(type_boolean, constructor);
	boolean* data = vm->heap.cast<boolean>(pos);
	data->set(val1==val2);

	vm->pop(2); // pop parameter, pop this

	vm->push(pos);
}

// add
void text::script_add(owlsl::vm* vm)
{
	owlsl::text& val1 = *vm->stack_cast<owlsl::text>(-1); // this
	owlsl::text& val2 = *vm->stack_cast<owlsl::text>(0);  // parameter

	int type_name = vm->table.id("text");
	int constructor = vm->table[type_name].pconstruct.id("text()");

	int pos = vm->pconstruct(type_name, constructor);
	owlsl::text* data = vm->heap.cast<owlsl::text>(pos);
	data->set(val1+val2);

	vm->pop(2); // pop parameter, pop this
	vm->push(pos);
}

// add set
void text::script_add_set(owlsl::vm* vm)
{
	owlsl::text* val1 = vm->stack_cast<owlsl::text>(-1); // this
	owlsl::text* val2 = vm->stack_cast<owlsl::text>(0);  // parameter
	val1->set(*val1+*val2);
	vm->pop(1); // pop parameter, leave this in the top of the stack
}

// add number
void text::script_add_number(owlsl::vm* vm)
{
	owlsl::text& val1 = *vm->stack_cast<owlsl::text>(-1); // this
	double num = vm->stack_cast<number>(0)->value;  // parameter

	int type_name = vm->table.id("text");
	int constructor = vm->table[type_name].pconstruct.id("text()");

	int pos = vm->pconstruct(type_name, constructor);
	owlsl::text* data = vm->heap.cast<owlsl::text>(pos);
	data->set(val1+owlsl::text((float)num));

	vm->pop(2); // pop parameter, pop this
	vm->push(pos);
}
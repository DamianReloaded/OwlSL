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

#include "number.h"
#include "base.h"
#include "../sco.h"

#include <iostream>
using namespace owlsl::type;

int number::m_type = 0;

number::number() : value(0)
{

}

number::number(number &num) : value (num.value)
{

}

number::~number()
{

}

void number::inscribe (owlsl::sco* sco)
{
	m_type = sco->vm.new_type("number");
	sco->constructor(m_type, "number()", &base::alloc<number>);
	sco->constructor(m_type, "number(number)", &base::alloc_set<number>);
	sco->destructor(m_type, &base::dalloc<number>);
	sco->method(m_type, "void =(number)", &base::set<number>);
	sco->method(m_type, "number +(number)", &number::script_add);
	sco->method(m_type, "number +=(number)", &number::script_add_set);
	sco->method(m_type, "number -(number)", &number::script_sub);
	sco->method(m_type, "number -=(number)", &number::script_sub_set);
	sco->method(m_type, "number /(number)", &number::script_div);
	sco->method(m_type, "number /=(number)", &number::script_div_set);
	sco->method(m_type, "number *(number)", &number::script_mult);
	sco->method(m_type, "number *=(number)", &number::script_mult_set);
	sco->method(m_type, "bool ==(number)", &base::equal<number, double>);
	sco->method(m_type, "bool !=(number)", &base::not_equal<number, double>);
	sco->method(m_type, "bool <(number)", &base::less<number, double>);
	sco->method(m_type, "bool >(number)", &base::greater<number, double>);
	sco->method(m_type, "bool <=(number)", &base::less_equal<number, double>);
	sco->method(m_type, "bool >=(number)", &base::greater_equal<number, double>);
	sco->method(m_type, "text text()", &number::script_to_string);
	sco->method(m_type, "number -()", &number::script_negate); // unary negation
	sco->method(m_type, "number ++()", &number::script_increment_set); // unary increment
	sco->method(m_type, "number --()", &number::script_decrement_set); // unary decrement
	sco->method(m_type, "number inc_literal()", &number::script_increment_literal); // unary increment
	sco->method(m_type, "number dec_literal()", &number::script_decrement_literal); // unary decrement
}

///////////////////////////////////////////////////////////////
// Script Interface Methods
///////////////////////////////////////////////////////////////

void number::script_to_string(owlsl::vm* vm)
{
	double num1 = vm->stack_cast<number>(0)->value; // this
	vm->pop(1); // pop this

	int type_text = vm->table.id("text");
	int constructor = vm->table[type_text].pconstruct.id("text()");

	int pos = vm->pconstruct(type_text, constructor);
	owlsl::text* data = vm->heap.cast<owlsl::text>(pos);
	data->set((float)num1);
	vm->push(pos);
}

// add
void number::script_add(owlsl::vm* vm)
{
	double num1 = vm->stack_cast<number>(-1)->value; // this
	double num2 = vm->stack_cast<number>(0)->value;  // parameter
	vm->pop(2); // pop parameter, pop this

	int type_name = vm->table.id("number");
	int constructor = vm->table[type_name].pconstruct.id("number()");

	int pos = vm->pconstruct(type_name, constructor);
	number* data = vm->heap.cast<number>(pos);
	data->set(num1+num2);
	vm->push(pos);
}

// add set
void number::script_add_set(owlsl::vm* vm)
{
	number* num1 = vm->stack_cast<number>(-1); // this
	double num2 = vm->stack_cast<number>(0)->value;  // parameter
	num1->set(num1->value+num2);
	vm->pop(1); // pop parameter, leave this in the top of the stack
}

void number::script_sub(owlsl::vm* vm)
{
	double num1 = vm->stack_cast<number>(-1)->value; // this
	double num2 = vm->stack_cast<number>(0)->value;  // parameter
	vm->pop(2); // pop parameter, pop this

	int type_name = vm->table.id("number");
	int constructor = vm->table[type_name].pconstruct.id("number()");

	int pos = vm->pconstruct(type_name, constructor);
	number* data = vm->heap.cast<number>(pos);
	data->set(num1-num2);
	vm->push(pos);
}


void number::script_sub_set(owlsl::vm* vm)
{
	number* num1 = vm->stack_cast<number>(-1); // this
	double num2 = vm->stack_cast<number>(0)->value;  // parameter
	num1->set(num1->value-num2);
	vm->pop(1); // pop parameter, leave this in the top of the stack
}

void number::script_mult(owlsl::vm* vm)
{
	double num1 = vm->stack_cast<number>(-1)->value; // this
	double num2 = vm->stack_cast<number>(0)->value;  // parameter
	vm->pop(2); // pop parameter, pop this

	int type_name = vm->table.id("number");
	int constructor = vm->table[type_name].pconstruct.id("number()");

	int pos = vm->pconstruct(type_name, constructor);
	number* data = vm->heap.cast<number>(pos);
	data->set((double)num1*(double)num2);
	vm->push(pos);
}

void number::script_mult_set(owlsl::vm* vm)
{
	number* num1 = vm->stack_cast<number>(-1); // this
	double num2 = vm->stack_cast<number>(0)->value;  // parameter
	num1->set((double)num1->value*(double)num2);
	vm->pop(1); // pop parameter, leave this in the top of the stack
}

void number::script_div(owlsl::vm* vm)
{
	double num1 = vm->stack_cast<number>(-1)->value; // this
	double num2 = vm->stack_cast<number>(0)->value;  // parameter
	vm->pop(2); // pop parameter, pop this

	int type_name = vm->table.id("number");
	int constructor = vm->table[type_name].pconstruct.id("number()");

	int pos = vm->pconstruct(type_name, constructor);
	number* data = vm->heap.cast<number>(pos);
	data->set((double)num1/(double)num2);
	vm->push(pos);
}


void number::script_div_set(owlsl::vm* vm)
{
	number* num1 = vm->stack_cast<number>(-1); // this
	double num2 = vm->stack_cast<number>(0)->value;  // parameter
	num1->set((double)num1->value/(double)num2);
	vm->pop(1); // pop parameter, leave this in the top of the stack
}

void number::script_negate(owlsl::vm* vm)
{
	double val1 = vm->stack_cast<number>(0)->get(); // this
	vm->pop(1); // pop this

	int type_name = vm->table.id("number");
	int constructor = vm->table[type_name].pconstruct.id("number()");

	int pos = vm->pconstruct(type_name, constructor);
	number* data = vm->heap.cast<number>(pos);
	data->set(-val1);
	vm->push(pos);
}

void number::script_increment_set(owlsl::vm* vm)
{
	number* data = vm->stack_cast<number>(0); // this
	data->set(++data->get());
}
void number::script_decrement_set(owlsl::vm* vm)
{
	number* data = vm->stack_cast<number>(0); // this
	data->set(--data->get());
}

void number::script_increment_literal(owlsl::vm* vm)
{
	number* val1 = vm->stack_cast<number>(0); // this
	vm->pop(1); // pop this

	int type_name = vm->table.id("number");
	int constructor = vm->table[type_name].pconstruct.id("number()");

	int pos = vm->pconstruct(type_name, constructor);
	number* data = vm->heap.cast<number>(pos);
	data->set(val1->get()+1);
	vm->push(pos);
}
void number::script_decrement_literal(owlsl::vm* vm)
{
	number* val1 = vm->stack_cast<number>(0); // this
	vm->pop(1); // pop this

	int type_name = vm->table.id("number");
	int constructor = vm->table[type_name].pconstruct.id("number()");

	int pos = vm->pconstruct(type_name, constructor);
	number* data = vm->heap.cast<number>(pos);
	data->set(val1->get()-1);
	vm->push(pos);
}
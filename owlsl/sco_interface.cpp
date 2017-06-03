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

#include "sco.h"
#include "vm.h"
#include "types/types.h"

using namespace owlsl;

bool sco::init_interface()
{
	// Add internal types in order of dependency
	vm.new_type("object"); // default base type
	owlsl::type::text::inscribe(this);
	owlsl::type::boolean::inscribe(this);
	owlsl::type::number::inscribe(this);
	owlsl::type::list::inscribe(this);
	owlsl::type::reference::inscribe(this);
	
	return true;
}

int sco::constructor (const owlsl::text& type_name, const owlsl::text& method_name, owlsl::vm::func_int_int fp)
{
	return constructor (vm.table.id(type_name), method_name, fp);
}

int sco::destructor (const owlsl::text& type_name, owlsl::vm::func_int_int fp)
{
	return destructor(vm.table.id(type_name), fp);
}

int sco::method (const owlsl::text& type_name, const owlsl::text& method_name, owlsl::vm::func_void_void fp)
{
	return method (vm.table.id(type_name), method_name, fp);
}

int sco::constructor (const unsigned& type, const owlsl::text& method_name, owlsl::vm::func_int_int fp)
{
	parse_parameters(type, method_name, true); // constructors don't use this pointer
	return vm.table[type].constructor(method_name, fp);
}

int sco::destructor (const unsigned& type, owlsl::vm::func_int_int fp)
{
	return vm.table[type].destructor(owlsl::text("~") + vm.table.name(type), fp);
}

int	sco::method	(const unsigned& type, const owlsl::text& method_name, owlsl::vm::func_void_void fp)
{
	owlsl::text name = method_name;
	
	// determine return type
	size_t pos = name.find(" ");
	owlsl::text ret_type = name.subtext(0, pos).trim();
	name = name.subtext(pos+1, name.length()-(pos+1)).trim();
	
	if (ret_type != owlsl::text("void"))
	{
		vm.table[type].pretval.add(name, vm.table.id(ret_type));
	}

	parse_parameters(type, name);
	return vm.table[type].method(name, fp);
}

void sco::parse_parameters (const unsigned& type, const owlsl::text& method_name, const bool& is_static)
{
	unsigned count = method_name.count(",");
	if (0 == count) // if there are no commas, check there is just one parameter
	{
		int pos = method_name.find("(");
		pos = method_name.find(")") - pos;
		if (pos>1) count++; // there is 1 parameter
	}
	else
	{
		count++; // number of paramters is number of commas + 1
	}
	if (!is_static) count++; // this pointer counts as parameter
	vm.table[type].pparameters.add(method_name, count);
}
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

#ifndef owl_script_base_H
#define owl_script_base_H

#include "../vm.h"

namespace owlsl {
namespace type {

	class sco;

	class OWL_SCRIPT base : public owlsl::dllnew
	{
		public:

			// alloc
			template <class T>
			static int alloc(owlsl::vm* vm, const int& type)
			{
				return vm->heap.palloc<T>(type, new T());
			}
			// alloc_set
			template <class T>
			static int alloc_set(owlsl::vm* vm, const int& type)
			{
				T& param = *vm->stack_cast<T>(0);
				int pos = vm->heap.palloc<T>(type, new T(param));
				vm->pop(); // pop parameter
				return pos;
			}
			// dalloc
			template <class T>
			static int dalloc(owlsl::vm* vm, const int& pos)
			{
				return vm->heap.pdalloc<T>(pos);
			}
			// set
			template <class T>
			static void set(owlsl::vm* vm)
			{
				T& param = *vm->stack_cast<T>(0);
				T& obj = *vm->stack_cast<T>(-1);
				obj.set(param.value);
				vm->pop(2); // pop parameter, pop this
			}
			// equal
			template <class T, class TCast>
			static void equal(owlsl::vm* vm)
			{
				TCast val1 = vm->stack_cast<T>(-1)->value; // this
				TCast val2 = vm->stack_cast<T>(0)->value;  // parameter
				vm->pop(2); // pop parameter, pop this

				static int type_boolean = vm->table.id("bool");
				static int constructor = vm->table[type_boolean].pconstruct.id("bool()");

				int pos = vm->pconstruct(type_boolean, constructor);
				T* data = vm->heap.cast<T>(pos);
				data->set(val1==val2);
				vm->push(pos);
			}
			// not_equal
			template <class T, class TCast>
			static void not_equal(owlsl::vm* vm)
			{
				TCast val1 = vm->stack_cast<T>(-1)->value; // this
				TCast val2 = vm->stack_cast<T>(0)->value;  // parameter
				vm->pop(2); // pop parameter, pop this

				static int type_boolean = vm->table.id("bool");
				static int constructor = vm->table[type_boolean].pconstruct.id("bool()");

				int pos = vm->pconstruct(type_boolean, constructor);
				T* data = vm->heap.cast<T>(pos);
				data->set(val1!=val2);
				vm->push(pos);
			}
			// less
			template <class T, class TCast>
			static void less(owlsl::vm* vm)
			{
				TCast val1 = vm->stack_cast<T>(-1)->value; // this
				TCast val2 = vm->stack_cast<T>(0)->value;  // parameter
				vm->pop(2); // pop parameter, pop this

				static int type_boolean = vm->table.id("bool");
				static int constructor = vm->table[type_boolean].pconstruct.id("bool()");

				int pos = vm->pconstruct(type_boolean, constructor);
				T* data = vm->heap.cast<T>(pos);
				data->set(val1<val2);
				vm->push(pos);
			}
			// greater
			template <class T, class TCast>
			static void greater(owlsl::vm* vm)
			{
				TCast val1 = vm->stack_cast<T>(-1)->value; // this
				TCast val2 = vm->stack_cast<T>(0)->value;  // parameter
				vm->pop(2); // pop parameter, pop this

				static int type_boolean = vm->table.id("bool");
				static int constructor = vm->table[type_boolean].pconstruct.id("bool()");

				int pos = vm->pconstruct(type_boolean, constructor);
				T* data = vm->heap.cast<T>(pos);
				data->set(val1>val2);
				vm->push(pos);
			}
			// less equal
			template <class T, class TCast>
			static void less_equal(owlsl::vm* vm)
			{
				TCast val1 = vm->stack_cast<T>(-1)->value; // this
				TCast val2 = vm->stack_cast<T>(0)->value;  // parameter
				vm->pop(2); // pop parameter, pop this

				static int type_boolean = vm->table.id("bool");
				static int constructor = vm->table[type_boolean].pconstruct.id("bool()");

				int pos = vm->pconstruct(type_boolean, constructor);
				T* data = vm->heap.cast<T>(pos);
				data->set(val1<=val2);
				vm->push(pos);
			}
			// greater equal
			template <class T, class TCast>
			static void greater_equal(owlsl::vm* vm)
			{
				TCast val1 = vm->stack_cast<T>(-1)->value; // this
				TCast val2 = vm->stack_cast<T>(0)->value;  // parameter
				vm->pop(2); // pop parameter, pop this

				static int type_boolean = vm->table.id("bool");
				static int constructor = vm->table[type_boolean].pconstruct.id("bool()");

				int pos = vm->pconstruct(type_boolean, constructor);
				T* data = vm->heap.cast<T>(pos);
				data->set(val1>=val2);
				vm->push(pos);
			}
	};

}}

#endif // owl_script_base_H
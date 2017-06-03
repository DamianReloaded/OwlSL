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

#ifndef owl_script_number_H
#define owl_script_number_H

#include "object.h"

namespace owlsl {
namespace type {

	class OWL_SCRIPT number : public object
	{
		public:
						number		();
						number		(number &num);
			virtual		~number		();

			static void	inscribe	(owlsl::sco* sco);

			virtual int	type		() {return m_type;}

			void		set			(const double& val) { value = val; }
			double&		get			()					{ return value; }

			double		value;

			static int m_type;
			///////////////////////////////////////////////////////////////
			// Script Interface Methods
			///////////////////////////////////////////////////////////////
			static void script_to_string(owlsl::vm* vm);
			static void script_add(owlsl::vm* vm);
			static void script_add_set(owlsl::vm* vm);
			static void script_sub(owlsl::vm* vm);
			static void script_sub_set(owlsl::vm* vm);
			static void script_mult(owlsl::vm* vm);
			static void script_mult_set(owlsl::vm* vm);
			static void script_div(owlsl::vm* vm);
			static void script_div_set(owlsl::vm* vm);
			static void script_negate(owlsl::vm* vm);
			static void script_increment_set(owlsl::vm* vm);
			static void script_decrement_set(owlsl::vm* vm);
			static void script_increment_literal(owlsl::vm* vm);
			static void script_decrement_literal(owlsl::vm* vm);
	};

}}

#endif // owl_script_number_H
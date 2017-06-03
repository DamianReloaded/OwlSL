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

#ifndef owl_script_reference_H
#define owl_script_reference_H

#include "object.h"
#include <owlsl/utils/text.h>
#include <deque>

namespace owlsl {
	class vm;
	class sco;
namespace type {

	class OWL_SCRIPT reference : public object
	{
		public:
							reference		();
			virtual			~reference		();

			static void		inscribe		(owlsl::sco* sco);
			static void		specialize		(owlsl::sco* sco, const owlsl::text& new_type, std::deque<owlsl::text>& ttypes);
			virtual int		type			() {return m_type;}

			static int		m_type;
			int				m_ref;

			///////////////////////////////////////////////////////////////
			// Script Interface Methods
			///////////////////////////////////////////////////////////////
			static int  script_dalloc	(owlsl::vm* vm, const int& pos);
			static void script_set		(owlsl::vm* vm);
			static void script_set_ref	(owlsl::vm* vm);
			static void script_ref		(owlsl::vm* vm);
			static void script_clear	(owlsl::vm* vm);
	};

}}

#endif // owl_script_reference_H
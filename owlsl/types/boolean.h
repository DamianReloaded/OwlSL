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

#ifndef owl_script_boolean_H
#define owl_script_boolean_H

#include "object.h"

namespace owlsl {
namespace type {

	class OWL_SCRIPT boolean : public owlsl::dllnew
	{
		public:
						boolean		();
						boolean		(boolean& val);
			virtual		~boolean	();

			virtual int	type		() {return m_type;}

			void		set			(const double& val) { value = val; }
			bool		get			()					{ return value!=0.0; }

			double		value; // represent it as double because of scalar destructor bug
			static int	m_type;

			static void	inscribe	(owlsl::sco* sco);

			///////////////////////////////////////////////////////////////
			// Script Interface Methods
			///////////////////////////////////////////////////////////////
			static void script_to_string(owlsl::vm* vm);
			static void script_lnot(owlsl::vm* vm);
			static void script_land(owlsl::vm* vm);
			static void script_lor(owlsl::vm* vm);
	};

}}

#endif // owl_script_boolean_H
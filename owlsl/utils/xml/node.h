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
#ifndef owl_xml_node_H
#define owl_xml_node_H

#include "../map.h"
#include "../text.h"

namespace owlsl {
namespace xml {

	class node
	{
		friend class document;

		public:
								node				();
			virtual				~node				();

			owlsl::text&			name				();
			owlsl::text&			inner_text			();

			void				name				(const owlsl::text& val);
			void				inner_text			(const owlsl::text& val);

			void				add_property		(const owlsl::text& name, const owlsl::text& value); 
			void				add_node			(node* _node);

			bool				find_property		(const owlsl::text& name);
			bool				next_property		();
			owlsl::text			property_name		();
			owlsl::text&			property_value		();

			bool				find_node			(const owlsl::text& name);
			bool				next_sibling_node	();
			node*				current_node		();
			bool				found				();

			void				first_node			();
			bool				next_node			();

			void				first_property		();
			bool				found_property		();

		protected:
			owlsl::text										m_name;
			owlsl::text										m_inner_text;

            typedef std::multimap<std::wstring, node*>		map_t;
            typedef map_t::iterator							iterator_t;
			typedef std::pair<iterator_t,iterator_t>		range_t;

			owlsl::map<std::wstring, owlsl::text>				m_properties;
			map_t											m_child_nodes;
			iterator_t										m_current_node;
			range_t											m_results;
	};
}}

#endif // owl_xml_node_H
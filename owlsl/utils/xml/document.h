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
#ifndef owl_xml_document_H
#define owl_xml_document_H

#include "../wfile.h"
#include "node.h"

namespace owlsl {
namespace xml {

	class document
	{
		public:
								document			();
			virtual				~document			();

			bool				parse				(owlsl::mfile& f);
			bool				write				(owlsl::mfile& f);

			node*				root				();
			owlsl::text&			error_description	();
			owlsl::text			xml					();

		protected:
			void				parse_node			(owlsl::wfile&	m_file, size_t& line, size_t& col, std::wstring& current_char, node* parent);
			void				set_error			(owlsl::wfile& m_file, owlsl::text text, const size_t& line, const size_t& col);

			bool				write_nodes			(node* parent, owlsl::text& text);
			void				write_node			(node* node, owlsl::text& text);

			node				m_root;
			bool				m_error;
			owlsl::text			m_error_description;
	};

}}

#endif // owl_xml_document_H
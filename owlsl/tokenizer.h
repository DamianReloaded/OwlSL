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

#ifndef owl_script_tokenizer_H
#define owl_script_tokenizer_H

#include "_export.h"
#include <set>
#include <vector>
#include <owlsl/utils/map.h>
#include <owlsl/utils/wfile.h>
#include "error.h"

namespace owlsl {

	class OWL_SCRIPT tokenizer : public owlsl::dllnew
	{
		public:
			struct OWL_SCRIPT token_type : public owlsl::dllnew //token type
			{
				owlsl::text	name;
				owlsl::text	text;
				unsigned	id;
			};

			struct OWL_SCRIPT token : public owlsl::dllnew
			{
				enum id {end_of_file=5000, end_of_statement, end_of_line, symbol, string_literal, number_literal, boolean_literal, comment, block_comment, operation, user_defined};

				owlsl::text			text;
				token_type*			type;
				unsigned			column;
				unsigned			line;

				bool operator == (const owlsl::text& val)		{ return text == val; }
				bool operator == (const id& val)			{ return type->id == val; }
			};
			typedef std::vector<token> tokens;

			// To hold token types
			struct OWL_SCRIPT _types : public owlsl::dllnew
			{
				_types() {}

				void add(const owlsl::text& text, const owlsl::text& name, const unsigned& id)
				{
					token_type tkn;
					tkn.name = name;
					tkn.text = text;
					tkn.id	  = id;
					items_by_name.insert(text.wstring(), tkn);
					items_by_name.find(text.wstring());
					items_by_id.insert(id, &items_by_name.value());
				}

				token_type* get(const owlsl::text& str)
				{
					if (items_by_name.find(str.wstring()))
					{
						return &items_by_name.value();
					}
					return get(token::symbol);
				}

				token_type* get(const unsigned& id)
				{
					if (items_by_id.find(id))
					{
						return items_by_id.value();
					}
					return get(token::symbol);
				}

				private:
					owlsl::map<std::wstring, token_type>	items_by_name;
					owlsl::map<unsigned, token_type*>		items_by_id;
			} type;

			// To hold the separator characters
			struct OWL_SCRIPT _separator : public owlsl::dllnew
			{
				void add(const owlsl::text& item, const bool& space=false)
				{
					items.insert(item.wstring());
					if (space) spaces.insert(item.wstring());
				}
				
				bool exists (const owlsl::text& chr)
				{
					return items.find(chr.wstring()) != items.end();
				}

				bool is_space(const owlsl::text& chr)
				{
					return spaces.find(chr.wstring()) != spaces.end();
				}

				private:
					std::set<std::wstring>	items;
					std::set<std::wstring>	spaces;
			} separator;

			// To hold string and block comment keywords
			struct OWL_SCRIPT _blocks : public owlsl::dllnew
			{
				struct _block : public owlsl::dllnew
				{
					void set(const owlsl::text& start, const owlsl::text& end)
					{
						m_start = start;
						m_end   = end;
					}

					owlsl::text start() { return m_start; }
					owlsl::text end()   { return m_end; }

					private:
						owlsl::text m_start;
						owlsl::text m_end;
				} string, comment;
			} blocks;

			// To hold line comment keyword
			struct OWL_SCRIPT _comment : public owlsl::dllnew
			{
				friend class tokenizer;
				void set(const owlsl::text& str) {m_comment = str;}

				private:
					owlsl::text m_comment;
			} comment;

			const error& last_error() { return m_last_error; }

		public:
						tokenizer	();
			virtual		~tokenizer	();
			bool		parse		(owlsl::wfile& wf, tokens& tkns);
			bool		is_comment	(const owlsl::text& str) { return str == comment.m_comment; }

			void		append(const unsigned& id, tokens& tkns, const unsigned& column, const unsigned& line);

		private:
			bool		is_block(const owlsl::text& str, unsigned& column, owlsl::wfile::wchars& wc);
			bool		parse_string_literal(token& tk, owlsl::wfile& wf, unsigned& column, unsigned& line);
			bool		parse_line_comment(token& tk, owlsl::wfile& wf, unsigned& column, unsigned& line);
			bool		parse_block_comment(token& tk, owlsl::wfile& wf, unsigned& column, unsigned& line);
			bool		parse_separator(token& tk, owlsl::wfile& wf, unsigned& column, unsigned& line);
			
			token_type*	determine_type(const owlsl::text& str);

			void set_last_error(const owlsl::text& description, const unsigned& column, const unsigned& line)
			{
				m_last_error.column = column+1;
				m_last_error.line = line+1;
				m_last_error.description = description;
			}
			error		m_last_error;
	};

}

#endif // owl_script_tokenizer_H
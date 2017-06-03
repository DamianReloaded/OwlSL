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

#ifndef owl_script_sco_H
#define owl_script_sco_H

#include <deque>
#include "_export.h"
#include "tokenizer.h"
#include "vm.h"

namespace owlsl {

	#ifdef _DEBUG
		#define add_code(code,text) vm.ins(code,text)
	#else
		#define add_code(code,text) vm << code
	#endif

	struct OWL_SCRIPT scosyntax
	{
		enum id {root, classtoken, classname, classbody, functoken, funcname, funcparams, paramtype, paramname, paramcomma, funcbody, proptoken, propname, propcomma, host};
	};

	class OWL_SCRIPT sco : public owlsl::dllnew
	{
		public:
			struct OWL_SCRIPT _symbol : public owlsl::dllnew
			{
				_symbol() : parameter(false) {}
				bool		parameter;
				owlsl::text	name;
				int			pos;
				unsigned	type;
			};
			std::vector<_symbol> current_param;

			struct OWL_SCRIPT _context : public owlsl::dllnew
			{
				unsigned pos;
				owlsl::map<std::wstring, _symbol> symbol;
			};
			unsigned				stack_pos;
			std::deque<_context>	context;

								sco();
			virtual				~sco();

			bool				open				(const owlsl::path& filepath);
			bool				append				(const owlsl::text& code);
			bool				compile				(const int& _pass=0);

			bool				init_tokens			();
			bool				init_interface		();

			bool				parse				();
			bool				ignore				(const unsigned& id);
			bool				parse_function		(unsigned& t, tokenizer::token* token);
			void				parse_end_params	();
			bool				parse_method_name	(unsigned& t, owlsl::text& method_name);
			void				parse_pushctx		();
			void				parse_popctx		();
			bool				parse_push			(unsigned& t);
			bool				parse_push_string	(unsigned& t);
			bool				parse_push_number	(unsigned& t);
			bool				parse_push_boolean	(unsigned& t);
			bool				parse_push_symbol	(unsigned& t);
			bool				parse_palloc		(unsigned& t);
			bool				parse_pcall			(unsigned& t);
			bool				parse_salloc		(unsigned& t);
			bool				parse_scall			(unsigned& t);
			bool				parse_return0		(unsigned& t);
			bool				parse_return1		(unsigned& t);
			bool				parse_ialloc		(unsigned& t);
			bool				parse_jump			(unsigned& t);
			bool				parse_jumpt			(unsigned& t);
			bool				parse_jumpf			(unsigned& t);
			bool				parse_pop			(unsigned& t);

			int					constructor			(const owlsl::text& type_name, const owlsl::text& method_name, owlsl::vm::func_int_int fp);
			int					destructor			(const owlsl::text& type_name, owlsl::vm::func_int_int fp);
			int					method				(const owlsl::text& type_name, const owlsl::text& method_name, owlsl::vm::func_void_void fp);
			int					constructor			(const unsigned& type, const owlsl::text& method_name, owlsl::vm::func_int_int fp);
			int					destructor			(const unsigned& type, owlsl::vm::func_int_int fp);
			int					method				(const unsigned& type, const owlsl::text& method_name, owlsl::vm::func_void_void fp);
			void				parse_parameters	(const unsigned& type, const owlsl::text& method_name, const bool& is_static = false);

			void				push_context		();
			void				pop_context			();
			bool				find_symbol			(const std::wstring& name, _symbol*& sym);
			int					symbol_type			(const std::wstring& name);

			const				error& last_error	() { return m_last_error; }
			void				set_last_error		(const owlsl::text& description, const unsigned& column, const unsigned& line);

			tokenizer			tok;
			tokenizer::tokens	tokens;
			scosyntax::id		syntax_context;
			owlsl::vm		vm;
			unsigned			current_type;
			owlsl::text			current_method_type_str;
			owlsl::text			current_method_str;
			owlsl::text			current_property_str;
			unsigned			num_params;
			unsigned			pass; // current parse pass
			unsigned			code_size;
			error				m_last_error;

			struct OWL_SCRIPT _table : public owlsl::dllnew
			{
				owlsl::map<std::wstring, unsigned> string;
				owlsl::map<std::wstring, unsigned> number;
				owlsl::map<std::wstring, unsigned> boolean;
				owlsl::map<std::wstring, unsigned> host;
				owlsl::map<std::wstring, unsigned> label;
			} table;
	};

}

#endif // owl_script_sco_H
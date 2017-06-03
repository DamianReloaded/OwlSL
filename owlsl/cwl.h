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

#ifndef owl_script_cwl_H
#define owl_script_cwl_H

#include <deque>
#include "_export.h"
#include "sco.h"
#include "tokenizer.h"
#include "types/types.h"

namespace owlsl {

	struct OWL_SCRIPT cwlsyntax
	{
		enum id {root, host, classtoken, classname, classbody, functoken, funcname, funcparams, paramtype, paramname, paramcomma, funcbody, proptoken, propname, statement};
	};

	class OWL_SCRIPT callback
	{
		public:
			callback() : vm(NULL) {}

			void set(owlsl::vm* vm)
			{
				// Get the name of the method (text)
				owlsl::text& mtd_name = *vm->stack_cast<owlsl::text>(0); // top of the stack
				
				// Get script defined object instance
				obj_pos = *(vm->stack.end()-2); // top of the stack - 1
				obj_type = vm->heap.mem[obj_pos].type;
				// get the id of the method to call
				mtd_pos = vm->table[obj_type].smethod.id(mtd_name);
				// save the pointer to the virtual machine
				this->vm = vm;

				// pop parameter, pop this
				vm->pop(3);
			}

			void call()
			{
				if (vm) 
				{
					vm->push(obj_pos);
					vm->scallmethod(obj_type, mtd_pos);
				}
			}

			void call_begin()
			{
				if (vm) vm->push(obj_pos);
			}

			template <class T>
			T* new_parameter(const owlsl::text& type_name, const T& value)
			{
				return new_parameter<T>(vm->table.id(type_name), value);
			}

			template <class T>
			T* new_parameter(const int& type_id, const T& value)
			{
				heap_obj<T> v;
				v.pos = vm->pconstruct(type_id, 0);
				v.instance = vm->heap.cast<T>(v.pos);
				v.instance->set(value);
				vm->push(v.pos);
				return v.instance;
			}

			void add_parameter(const int& pos)
			{
				vm->push(pos);
			}
		
			void call_end()
			{
				if (vm) vm->scallmethod(obj_type, mtd_pos);
			}

			owlsl::vm* vm;
			int				 obj_pos;
			int				 obj_type;
			int				 mtd_pos;
	};

	class OWL_SCRIPT cwl : public owlsl::dllnew
	{
		public:
			struct OWL_SCRIPT _symbol : public owlsl::dllnew
			{
				owlsl::text name;
				owlsl::text type;
			};
			typedef owlsl::map<std::wstring, _symbol> symbol_map;

			struct OWL_SCRIPT _function : public owlsl::dllnew
			{
				owlsl::text	name;
				owlsl::text	type;
				symbol_map	params;
			};
			typedef owlsl::map<std::wstring, _function> func_map;

			struct OWL_SCRIPT _type : public owlsl::dllnew
			{
				owlsl::text	name;
				symbol_map	props;
				func_map	funcs;
			};
			typedef owlsl::map<std::wstring, _type> type_map;

			struct OWL_SCRIPT _context;
			typedef std::deque<_context*> context_stack;
			struct OWL_SCRIPT _context : public owlsl::dllnew
			{
						_context() : context_iterator(0) {}
				virtual ~_context()
				{
					for(unsigned i=0; i<contexts.size(); i++)
					{
						delete contexts[i];
					}
				}

				symbol_map	var;

				context_stack contexts;
				unsigned	  context_iterator;
				_context*	  parent;
			};

		public:
								cwl();
			virtual				~cwl();

			bool				open				(const owlsl::path& filepath);
			bool				append				(const owlsl::text& code);
			bool				compile				();
			bool				compile_sco			();
			bool				parse				();
			void				terminate			();

			struct OWL_SCRIPT _script
			{
				owlsl::sco*	sco;
				void				static_call(const owlsl::text& type, const owlsl::text& method);
				void				call (const owlsl::text& type, const owlsl::text& method, const bool& static_call=false);
				void				call (const int& type, const int& method, const bool& static_call=false);

				int					alloc (const owlsl::text& type);
				void				dalloc (const int& pos);
			} script;

			struct OWL_SCRIPT _host
			{
				owlsl::sco*	sco;
				
				int					type				(const owlsl::text& name);
				int					constructor			(const owlsl::text& type_name, const owlsl::text& method_name, owlsl::vm::func_int_int fp);
				int					constructor			(const unsigned&  type_id  , const owlsl::text& method_name, owlsl::vm::func_int_int fp);
				int					destructor			(const owlsl::text& type_name, owlsl::vm::func_int_int fp);
				int					destructor			(const unsigned&  type_id  , owlsl::vm::func_int_int fp);
				int					method				(const owlsl::text& type_name, const owlsl::text& method_name, owlsl::vm::func_void_void fp);
				int					method				(const unsigned&  type_id  , const owlsl::text& method_name, owlsl::vm::func_void_void fp);
				
				template <class T>
				heap_obj<T>			alloc				(const owlsl::text& type_name, const int& constructor = 0)
				{
					return sco->vm.alloc<T>(type_name, constructor);
				}

			} host;

			bool				ignore				(const unsigned& id);
			bool				parse_statement		(unsigned& t, const bool& must_end_statement=true);
			bool				parse_expression	(unsigned& t, const bool& is_func_param=false);
			bool				parse_terms			(unsigned& t);
			bool				parse_term			(unsigned& t);
			bool				parse_factor		(unsigned& t);
			bool				parse_declaration	(unsigned& t);
			bool				parse_templated_var	(unsigned& t, const bool& is_class_property=false);
			bool				parse_func_call		(unsigned& t, const owlsl::text& subcall_type="");
			bool				parse_func_params	(unsigned& t);
			owlsl::text			parse_obj_params	(const size_t& _perm, const std::vector<owlsl::text>& _params);
			bool				parse_assignment	(unsigned& t);
			bool				parse_return		(unsigned& t);
			bool				parse_if			(unsigned& t);
			bool				parse_for			(unsigned& t);
			bool				parse_switch		(unsigned& t);
			bool				parse_while			(unsigned& t);

			bool				at_end_of_file		(const unsigned& t);

			bool				init_tokens			();

			bool				is_type				(const tokenizer::token* token);
			bool				is_stype			(const tokenizer::token* token);
			bool				is_ptype			(const tokenizer::token* token);
			bool				is_boolean			(const tokenizer::token* token);
			bool				is_number			(const tokenizer::token* token);
			bool				is_string			(const tokenizer::token* token);
			bool				is_symbol			(const tokenizer::token* token);
			bool				is_assignment		(const tokenizer::token* token);
			bool				is_addition			(const tokenizer::token* token);
			bool				is_addition_set		(const tokenizer::token* token);
			bool				is_substraction		(const tokenizer::token* token);
			bool				is_substraction_set	(const tokenizer::token* token);
			bool				is_multiplication	(const tokenizer::token* token);
			bool				is_multiplication_set(const tokenizer::token* token);
			bool				is_division			(const tokenizer::token* token);
			bool				is_division_set		(const tokenizer::token* token);
			bool				is_open_parent		(const tokenizer::token* token);
			bool				is_close_parent		(const tokenizer::token* token);
			bool				is_end_of_statement	(const tokenizer::token* token);
			bool				is_open_curly		(const tokenizer::token* token);
			bool				is_close_curly		(const tokenizer::token* token);
			bool				is_comma			(const tokenizer::token* token);
			bool				is_colon			(const tokenizer::token* token);
			bool				is_return			(const tokenizer::token* token);
			bool				is_if				(const tokenizer::token* token);
			bool				is_else				(const tokenizer::token* token);
			bool				is_for				(const tokenizer::token* token);
			bool				is_switch			(const tokenizer::token* token);
			bool				is_case				(const tokenizer::token* token);
			bool				is_while			(const tokenizer::token* token);
			bool				is_default			(const tokenizer::token* token);
			bool				is_equal			(const tokenizer::token* token);
			bool				is_not				(const tokenizer::token* token);
			bool				is_not_equal		(const tokenizer::token* token);
			bool				is_less				(const tokenizer::token* token);
			bool				is_less_equal		(const tokenizer::token* token);
			bool				is_greater			(const tokenizer::token* token);
			bool				is_greater_equal	(const tokenizer::token* token);
			bool				is_boolean_operator	(const tokenizer::token* token);
			bool				is_logical_and		(const tokenizer::token* token);
			bool				is_logical_or		(const tokenizer::token* token);
			bool				is_increment_set	(const tokenizer::token* token);
			bool				is_decrement_set	(const tokenizer::token* token);
			bool				is_operator			(const tokenizer::token* token);
			bool				is_literal			(const tokenizer::token* token);
			bool				is_this				(const tokenizer::token* token);

			void				write_assembly		(const owlsl::text& text);
			bool				write_method_call	(const tokenizer::token* token, const owlsl::text& type_name, const owlsl::text& func_name, const owlsl::text& func_params);

			owlsl::text			indent				(const owlsl::text& text, const unsigned& left=0, const unsigned& right=0);

			const error&		last_error()		{ return m_last_error; }
			void				set_last_error		(const owlsl::text& description, const unsigned& column, const unsigned& line);

			void				add_type			(const owlsl::text& type_name);
			void				add_prop			(const owlsl::text& type_name, const owlsl::text& prop_name, const owlsl::text& prop_type);
			void				add_func			(const owlsl::text& type_name, const owlsl::text& func_name, const owlsl::text& func_type);
			void				add_func_param		(const owlsl::text& type_name, const owlsl::text& func_name, const owlsl::text& param_name, const owlsl::text& param_type);

			bool				is_type				(const owlsl::text& name);
			bool				is_stype			(const owlsl::text& name);
			bool				is_ptype			(const owlsl::text& name);

			void				push_context		();
			void				pop_context			();
			void				add_context_var		(const owlsl::text& var_name, const owlsl::text& var_type);
			owlsl::text			get_call_symbol		(const owlsl::text& func_call_text);
			owlsl::text			get_call_function	(const owlsl::text& func_call_text);
			bool				find_context_var	(const owlsl::text& var_name, _symbol*& sym, bool& use_this);
			bool				find_member_var		(const owlsl::text& type_name, const owlsl::text& var_name, _symbol*& sym);
			owlsl::text			get_func_type		(const owlsl::text& type_name, const owlsl::text& func_name, const owlsl::text& func_params);

			void				set_expression_type	  (const owlsl::text& type_name);
			void				clear_expression_type ();

			void				remove_uneeded_tokens ();

			void				push				  (const int& pos);
			void				pop					  (const int& val=1);

			void				heap_size			  (const unsigned& value);

			void				set_syntax_context	  (const cwlsyntax::id& id);

			bool				set_syntax_func_body  (unsigned& t);

			typedef	std::vector<owlsl::text> vector_text;

			owlsl::sco			sco;
			tokenizer					tok;
			tokenizer::tokens			tokens;
			cwlsyntax::id				syntax_context;
			cwlsyntax::id				syntax_context_prev;
			owlsl::text					assembly;
			unsigned					pass;
			type_map					types;
			_context					context;
			_context*					current_context;
			owlsl::text					current_class_name;
			owlsl::text					current_func_name;
			owlsl::text					current_param_type;
			owlsl::text					current_expression_type;
			std::vector<owlsl::text>		expression_depth;
			unsigned					label_count;
			std::vector<unsigned>		label_depth;
			unsigned					label_function_end;
			unsigned					temp_var_count;
			std::vector<int>			unnamed_contexts;
			std::vector<vector_text>	current_param_types;
			bool						factor_is_constant;
			bool						parsing_bool_condition;

			owlsl::path					m_current_file;
			error						m_last_error;
	};
}

#endif // owl_script_cwl_H
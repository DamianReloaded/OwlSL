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

#include "cwl.h"
#include <algorithm>

using namespace owlsl;

cwl::cwl()
{
	script.sco = &sco;
	host.sco   = &sco;

	init_tokens();

	m_current_file = "Unnamed";
	m_last_error.file = m_current_file;

	sco.vm.new_type("host"); // Add host type.

	parsing_bool_condition = false;
}

cwl::~cwl()
{

}

void cwl::set_last_error(const owlsl::text& description, const unsigned& column, const unsigned& line)
{
	m_last_error.column = column + 1;
	m_last_error.line = line + 1;
	m_last_error.description = description;
}

int cwl::_host::type(const owlsl::text& name)
{
	return sco->vm.new_type(name);
}

int cwl::_host::constructor	(const owlsl::text& type_name, const owlsl::text& method_name, owlsl::vm::func_int_int fp)
{
	return sco->constructor(type_name, method_name, fp);
}

int cwl::_host::destructor (const owlsl::text& type_name, owlsl::vm::func_int_int fp)
{
	return sco->destructor(type_name, fp);
}

int cwl::_host::method (const owlsl::text& type_name, const owlsl::text& method_name, owlsl::vm::func_void_void fp)
{
	return sco->method(type_name, method_name, fp);
}

int	cwl::_host::constructor	(const unsigned&  type_id  , const owlsl::text& method_name, owlsl::vm::func_int_int fp)
{
	return sco->constructor(type_id, method_name, fp);
}

int	cwl::_host::destructor (const unsigned&  type_id  , owlsl::vm::func_int_int fp)
{
	return sco->destructor(type_id, fp);
}

int	cwl::_host::method (const unsigned&  type_id  , const owlsl::text& method_name, owlsl::vm::func_void_void fp)
{
	return sco->method(type_id, method_name, fp);
}

void cwl::_script::call (const owlsl::text& type, const owlsl::text& method, const bool& static_call)
{
	sco->vm.scallmethod(sco->vm.table.id(type), sco->vm.table[type].smethod.id(method), static_call); //OJO con static!!
}

void cwl::_script::static_call(const owlsl::text& type, const owlsl::text& method)
{
	sco->vm.scallmethod(sco->vm.table.id(type), sco->vm.table[type].smethod.id(method), true);
}

void cwl::_script::call (const int& type, const int& method, const bool& static_call)
{
	sco->vm.scallmethod(type, method, static_call);
}

int cwl::_script::alloc (const owlsl::text& type)
{
	return sco->vm.salloc(sco->vm.table.id(type),0);
}

void cwl::_script::dalloc (const int& pos)
{
	sco->vm.sdestruct(pos);
}

bool cwl::open(const owlsl::path& filepath)
{
	m_current_file = filepath;
	m_last_error.file = m_current_file;

	owlsl::wfile f;
	if (!f.open(filepath))
	{
		m_last_error.description = owlsl::text("Couldn't open file: ") + filepath;
		m_last_error.file = m_current_file;
		return false;
	}

	if (!tok.parse(f, tokens))
	{
		m_last_error = tok.last_error();
		m_last_error.file = m_current_file;
		return false;
	}

	remove_uneeded_tokens ();

	return true;
}

void cwl::remove_uneeded_tokens ()
{
	tokens.erase(std::remove(tokens.begin(), tokens.end(), "\n"), tokens.end());
	tokens.erase(std::remove(tokens.begin(), tokens.end(), tokenizer::token::block_comment), tokens.end());
	tokens.erase(std::remove(tokens.begin(), tokens.end(), tokenizer::token::comment), tokens.end());
	tokens.erase(std::remove(tokens.begin(), tokens.end(), tokenizer::token::end_of_file), tokens.end());
}

bool cwl::append (const owlsl::text& code)
{
	owlsl::wfile f;

	std::vector<uint8_t> c;
	owlsl::text::get_mark(c, owlsl::encoding::utf16le);
	code.to_array(c, owlsl::encoding::utf16le);

	f.open(&c[0], c.size());
	if (!tok.parse(f, tokens))
	{
		m_last_error = tok.last_error();
		m_last_error.file = m_current_file;
		return false;
	}

	remove_uneeded_tokens ();

	return true;
}

bool cwl::compile ()
{
	if (!compile_sco()) return false;
	if (!sco.append(assembly) || !sco.compile())
	{
		m_last_error = sco.last_error();
		m_last_error.description = owlsl::text("SCO: ") + m_last_error.description;
		m_last_error.file = "Generated Assembly";
		return false;
	}

	return true;
}

bool cwl::compile_sco ()
{
	for (pass=0; pass<2; pass++)
	{
		if (!parse()) return false;
	}

	return true;
}

bool cwl::parse	()
{
	label_count = 0;
	temp_var_count = 0;
	syntax_context = cwlsyntax::root;
	syntax_context_prev = cwlsyntax::root;
	unnamed_contexts.clear();

	if (0 == pass)
	{
		context.parent = NULL;
		context.contexts.clear();
	}
	current_context = &context;

	tokenizer::token* token;
	for (unsigned t=0; t<tokens.size(); ++t)
	{
		token = &tokens[t];

		// Ignore comments, eof, bof and eos
		/*if ( ignore(token->type->id) )
		{
			continue;
		}*/

		switch (syntax_context)
		{
			case cwlsyntax::root:
			{
				// Host exposed instance declaration
				if (token->text == owlsl::text("host"))
				{
					// write interface directive
					write_assembly( owlsl::text(".host") + owlsl::endl );

					set_syntax_context(cwlsyntax::host);
					continue;
				}
				// class declaration
				else if (token->text == owlsl::text("class"))
				{
					write_assembly( owlsl::endl + owlsl::text("class ") );

					set_syntax_context(cwlsyntax::classtoken);
					continue;
				}
			}
			break;

			// class name declaration
			case cwlsyntax::classtoken:
			{
				if (is_symbol(token))
				{
					// Add class name
					write_assembly( token->text );
					add_type(token->text);
					sco.vm.new_type(token->text, true); // Add new types here, because of specialized templated methods

					set_syntax_context(cwlsyntax::classname);
				}
				else
				{
					set_last_error(owlsl::text("Unexpected: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
					return false;
				}
			}
			break;

			// class body begin
			case cwlsyntax::classname:
			{
				if (token->text == owlsl::text("{"))
				{
					write_assembly( owlsl::endl + "{" + owlsl::endl );
					set_syntax_context(cwlsyntax::classbody);
				}
				else
				{
					set_last_error(owlsl::text("Expected '{' got: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
					return false;
				}
			}
			break;

			// class body
			case cwlsyntax::classbody:
			{
				// specialize templated property definition
				if (is_symbol(token) &&is_less(&tokens[t+1]))
				{
					parse_templated_var (t, true);
				}

				// property definition
				if (  is_symbol(token) && is_symbol(&tokens[t+1]) && is_end_of_statement(&tokens[t+2])  )
				{
					write_assembly( "    property    " );

					owlsl::text type_name = token->text;

					owlsl::text symbol_name;
					// must be a symbol token
					token = &tokens[++t];
					if (is_symbol(token))
					{
						symbol_name = token->text;
					}
					else
					{
						set_last_error(owlsl::text("Unexpected: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
						return false;
					}

					write_assembly( symbol_name + ", " );
					write_assembly( type_name + owlsl::endl );

					add_prop(current_class_name, symbol_name, type_name);

					// expect end of statement
					token = &tokens[++t];
					if (!is_end_of_statement(token))
					{
						set_last_error(owlsl::text("Expected end of statement. Got: '") + token->type->name + owlsl::text(": ") + token->text + owlsl::text("'"), tokens[t].column, tokens[t].line);
						return false;
					}

					set_syntax_context(cwlsyntax::classbody);
				}
				// function definition
				else if ( is_symbol(token) && is_symbol(&tokens[t+1]) && is_open_parent(&tokens[t+2]) )
				{
					write_assembly( owlsl::endl + "    function " );

					owlsl::text function_type = token->text;

					token = &tokens[++t];
					owlsl::text function_name = token->text;

					write_assembly( function_type + " " );
					write_assembly( function_name + " " );

					add_func(current_class_name, function_name, function_type);

					label_function_end = 10000000 + (++label_count);

					set_syntax_context(cwlsyntax::funcname);
				}
				else if (is_close_curly(token))
				{
					write_assembly( owlsl::text("}") + owlsl::endl );
					set_syntax_context(cwlsyntax::root);
				}
				else
				{
					set_last_error(owlsl::text("Unexpected: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
					return false;
				}
			}
			break;

			case cwlsyntax::funcname:
			{
				if (is_open_parent(token))
				{
					write_assembly( "(" );
					set_syntax_context(cwlsyntax::funcparams);
				}
				else
				{
					set_last_error(owlsl::text("Unexpected: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
					return false;
				}
			}
			break;

			case cwlsyntax::paramcomma:
			case cwlsyntax::funcparams:
			{
				if (is_close_parent(token))
				{
					write_assembly( ")" );
					if (!set_syntax_func_body(t)) return false;

				}
				else if (is_symbol(token))
				{
					write_assembly( token->text + " " );
					current_param_type = token->text;
					set_syntax_context(cwlsyntax::paramtype);
				}
				else
				{
					set_last_error(owlsl::text("Unexpected: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
					return false;
				}
			}
			break;

			case cwlsyntax::paramtype:
			{
				if (is_symbol(token))
				{
					write_assembly( token->text );
					add_func_param(current_class_name, current_func_name, token->text, current_param_type);
					set_syntax_context(cwlsyntax::paramname);
				}
				else if (is_less(token))
				{
					if ( !parse_templated_var(--t, true) ) return false;
					--t;
					set_syntax_context(cwlsyntax::funcparams);
				}
				else
				{
					set_last_error(owlsl::text("Unexpected: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
					return false;
				}
			}
			break;

			case cwlsyntax::paramname:
			{
				if (is_close_parent(token))
				{
					write_assembly( ")" );
					if (!set_syntax_func_body(t)) return false;
				}
				else if (is_comma(token))
				{
					write_assembly( ", " );
					set_syntax_context(cwlsyntax::paramcomma);
				}
				else
				{
					set_last_error(owlsl::text("Unexpected: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
					return false;
				}
			}
			break;

			case cwlsyntax::funcbody:
			{
				if (is_open_curly(token))
				{
					if (syntax_context_prev == cwlsyntax::funcbody)
					{
						if (pass==1) write_assembly( indent("pushctx",8,4));
						unnamed_contexts.push_back(0);
					}
					write_assembly( owlsl::endl );
					push_context();
					if (!set_syntax_func_body(t)) return false;
				}
				else if (is_close_curly(token))
				{
					if (unnamed_contexts.size())
					{
						if (pass==1) write_assembly( indent("popctx",8,4) + owlsl::endl );
						unnamed_contexts.pop_back();
						pop_context();
						if (!set_syntax_func_body(t)) return false;
					}
					else
					{
						write_assembly( owlsl::text((uint32_t)label_function_end) + ":" + owlsl::endl );
						write_assembly( owlsl::text("    end") + owlsl::endl );
						pop_context();
						set_syntax_context(cwlsyntax::classbody);
					}
				}
				else if (is_end_of_statement(token))
				{
					if (!set_syntax_func_body(t)) return false;
				}
				else
				{
					--t;
					set_syntax_context(cwlsyntax::statement);

					//set_last_error(owlsl::text("Unexpected: '") + token->text + owlsl::text("'"), token->column, token->line);
					//return false;
				}
			}
			break;

			case cwlsyntax::statement:
			{
				if (!parse_statement(t)) return false;
				if (is_close_curly(&tokens[t]))
				{
					t--; // Go back so we increment t properly
				}
				else if (is_close_curly(&tokens[t-1]))
				{
					t--; // Go back so we increment t properly
				}
				if (!set_syntax_func_body(t)) return false;
			}
			break;

			case cwlsyntax::host:
			{
				if (!is_symbol(token))
				{
					set_last_error(owlsl::text("Expected symbol. Got: '") + token->type->name + owlsl::text(": ") + token->text + owlsl::text("'"), tokens[t].column, tokens[t].line);
					return false;
				}

				// write ialloc directive
				write_assembly( "ialloc " );

				owlsl::text type_name = token->text;
				owlsl::text constructor_name = token->text;

				// must be a symbol token
				token = &tokens[++t];
				if (!is_symbol(token))
				{
					set_last_error(owlsl::text("Expected symbol. Got: '") + token->type->name + owlsl::text(": ") + token->text + owlsl::text("'"), tokens[t].column, tokens[t].line);
					return false;
				}

				// get var name.
				owlsl::text var_name = token->text;

				// append parentheses if any (for now it doesn't allow construction parameters)
				token = &tokens[++t];
				if (!is_open_parent(token))
				{
					set_last_error(owlsl::text("Expected '('. Got: '") + token->type->name + owlsl::text(": ") + token->text + owlsl::text("'"), tokens[t].column, tokens[t].line);
					return false;
				}
				constructor_name += token->text;

				token = &tokens[++t];
				if (!is_close_parent(token))
				{
					set_last_error(owlsl::text("Expected ')'. Got: '") + token->type->name + owlsl::text(": ") + token->text + owlsl::text("'"), tokens[t].column, tokens[t].line);
					return false;
				}
				constructor_name += token->text;


				// expect end of statement
				token = &tokens[++t];
				if (!is_end_of_statement(token))
				{
					set_last_error(owlsl::text("Expected end of statement. Got: '") + token->type->name + owlsl::text(": ") + token->text + owlsl::text("'"), tokens[t].column, tokens[t].line);
					return false;
				}

				write_assembly( var_name );
				write_assembly( ", host, " );
				write_assembly( constructor_name + owlsl::endl );

				add_context_var(var_name, type_name);

				set_syntax_context(cwlsyntax::root);
				continue;
			}
			break;

			default: break;
		}
	}

	if (syntax_context != cwlsyntax::root)
	{
		set_last_error("Unexpected end of file. Missing '}'?", 0, tokens.size());
		return false;
	}


	pop_context();

	return true;
}

bool cwl::set_syntax_func_body (unsigned& t)
{
	if (1 == pass)
	{
		set_syntax_context(cwlsyntax::funcbody);
	}
	else
	{
		// Find end of function body
		size_t open_pars = 0;
		size_t close_pars = 0;
		while(!at_end_of_file(t))
		{
			if ( is_open_curly(&tokens[t]) ) open_pars++;
			if ( is_close_curly(&tokens[t]) )
			{
				close_pars++;
				if (open_pars == close_pars)
				{
					break;
				}
			}

			t++;
		}
		if (at_end_of_file(t))
		{
			set_last_error(owlsl::text("Expected '}'"), tokens[t].column, tokens[t].line);
			return false;
		}
		set_syntax_context(cwlsyntax::classbody);
	}

	return true;
}

void cwl::write_assembly (const owlsl::text& text)
{
	if (pass < 1) return;
	assembly += text;
}

bool cwl::write_method_call	(const tokenizer::token* token, const owlsl::text& type_name, const owlsl::text& func_name, const owlsl::text& func_params)
{
	if (pass < 1) return true;

	owlsl::text func_type = get_func_type(type_name, func_name, owlsl::text("(") + func_params + ")");
	if (func_type.length()==0)
	{
		set_last_error(owlsl::text("Method not found: '") + type_name + " " + func_name + "(" + func_params + ")" + owlsl::text("'"), token->column, token->line);
		return false;
	}

	write_assembly( indent("pcall",8,4) + type_name + ", " + func_name + "(" + func_params + ")" + owlsl::endl );
	return true;
}

bool cwl::is_literal (const tokenizer::token* token)
{
	return (is_number(token) || is_string(token) || is_boolean(token));
}

bool cwl::is_type (const tokenizer::token* token)
{
	return is_type(token->text);
}

bool cwl::is_stype (const tokenizer::token* token)
{
	return is_stype(token->text);
}

bool cwl::is_ptype (const tokenizer::token* token)
{
	return is_ptype(token->text);
}

bool cwl::is_type (const owlsl::text& name)
{
	return is_ptype(name) || is_stype(name) || (name == owlsl::text("void"));
}

bool cwl::is_stype (const owlsl::text& name)
{
	return types.find(name.wstring());
}

bool cwl::is_ptype (const owlsl::text& name)
{
	if (!sco.vm.table.contains(name)) return false;
	return !sco.vm.table[name].scripted;
}

bool cwl::is_number	(const tokenizer::token* token)
{
	return (token->type->id == tokenizer::token::number_literal);
}

bool cwl::is_boolean (const tokenizer::token* token)
{
	return (token->type->id == tokenizer::token::boolean_literal);
}

bool cwl::is_string	(const tokenizer::token* token)
{
	return (token->type->id == tokenizer::token::string_literal);
}

bool cwl::is_symbol	(const tokenizer::token* token)
{
	return (token->type->id == tokenizer::token::symbol);
}

bool cwl::is_assignment (const tokenizer::token* token)
{
	return (token->text.wstring() == L"=");
}

bool cwl::is_addition (const tokenizer::token* token)
{
	return (token->text.wstring() == L"+");
}

bool cwl::is_addition_set (const tokenizer::token* token)
{
	return (token->text.wstring() == L"+=");
}

bool cwl::is_substraction (const tokenizer::token* token)
{
	return (token->text.wstring() == L"-");
}

bool cwl::is_substraction_set(const tokenizer::token* token)
{
	return (token->text.wstring() == L"-=");
}

bool cwl::is_multiplication	(const tokenizer::token* token)
{
	return (token->text.wstring() == L"*");
}

bool cwl::is_multiplication_set (const tokenizer::token* token)
{
	return (token->text.wstring() == L"*=");
}

bool cwl::is_division (const tokenizer::token* token)
{
	return (token->text.wstring() == L"/");
}

bool cwl::is_division_set (const tokenizer::token* token)
{
	return (token->text.wstring() == L"/=");
}

bool cwl::is_open_parent (const tokenizer::token* token)
{
	return (token->text.wstring() == L"(");
}
bool cwl::is_close_parent (const tokenizer::token* token)
{
	return (token->text.wstring() == L")");
}
bool cwl::is_end_of_statement	(const tokenizer::token* token)
{
	return (token->text.wstring() == L";");
}

bool cwl::is_open_curly	(const tokenizer::token* token)
{
	return (token->text.wstring() == L"{");
}

bool cwl::is_close_curly (const tokenizer::token* token)
{
	return (token->text.wstring() == L"}");
}

bool cwl::is_comma (const tokenizer::token* token)
{
	return (token->text.wstring() == L",");
}

bool cwl::is_colon (const tokenizer::token* token)
{
	return (token->text.wstring() == L":");
}

bool cwl::is_return	(const tokenizer::token* token)
{
	return (token->text.wstring() == L"return");
}

bool cwl::is_if (const tokenizer::token* token)
{
	return (token->text.wstring() == L"if");
}

bool cwl::is_else (const tokenizer::token* token)
{
	return (token->text.wstring() == L"else");
}

bool cwl::is_for (const tokenizer::token* token)
{
	return (token->text.wstring() == L"for");
}

bool cwl::is_switch (const tokenizer::token* token)
{
	return (token->text.wstring() == L"switch");
}

bool cwl::is_case (const tokenizer::token* token)
{
	return (token->text.wstring() == L"case");
}

bool cwl::is_while (const tokenizer::token* token)
{
	return (token->text.wstring() == L"while");
}

bool cwl::is_default (const tokenizer::token* token)
{
	return (token->text.wstring() == L"default");
}

bool cwl::is_equal (const tokenizer::token* token)
{
	return (token->text.wstring() == L"==");
}

bool cwl::is_not (const tokenizer::token* token)
{
	return (token->text.wstring() == L"!");
}

bool cwl::is_not_equal (const tokenizer::token* token)
{
	return (token->text.wstring() == L"!=");
}

bool cwl::is_less (const tokenizer::token* token)
{
	return (token->text.wstring() == L"<");
}

bool cwl::is_less_equal (const tokenizer::token* token)
{
	return (token->text.wstring() == L"<=");
}

bool cwl::is_greater (const tokenizer::token* token)
{
	return (token->text.wstring() == L">");
}

bool cwl::is_greater_equal (const tokenizer::token* token)
{
	return (token->text.wstring() == L">=");
}

bool cwl::is_boolean_operator (const tokenizer::token* token)
{
	return (is_logical_and(token) || is_logical_or(token) || is_equal(token) || is_not_equal(token) || is_less(token) || is_less_equal(token) || is_greater(token) || is_greater_equal(token));
}

bool cwl::is_logical_and (const tokenizer::token* token)
{
	return (token->text.wstring() == L"&&");
}

bool cwl::is_logical_or (const tokenizer::token* token)
{
	return (token->text.wstring() == L"||");
}

bool cwl::is_increment_set (const tokenizer::token* token)
{
	return (token->text.wstring() == L"++");
}

bool cwl::is_decrement_set (const tokenizer::token* token)
{
	return (token->text.wstring() == L"--");
}

bool cwl::is_operator (const tokenizer::token* token)
{
	return (token->type->id == tokenizer::token::operation);
}

bool cwl::is_this (const tokenizer::token* token)
{
	return (token->text.wstring() == L"this");
}

owlsl::text cwl::indent (const owlsl::text& text, const unsigned& left, const unsigned& right)
{
	owlsl::text str(text);
	str.fill_left(str.length()+left, L" ");
	return str.fill_right(17+right, L" ");
}

void cwl::add_type (const owlsl::text& type_name)
{
	current_class_name = type_name;

	if (pass>0) return;

	_type type;
	type.name = type_name;
	types.insert(type_name.wstring(), type);
}

void cwl::add_prop (const owlsl::text& type_name, const owlsl::text& prop_name, const owlsl::text& prop_type)
{
	if (pass>0) return;

	_symbol prop;
	prop.name = prop_name;
	prop.type = prop_type;
	types.find(type_name.wstring());
	types.value().props.insert(prop_name.wstring(), prop);
}

void cwl::add_func (const owlsl::text& type_name, const owlsl::text& func_name, const owlsl::text& func_type)
{
	current_func_name = func_name;

	if (pass>0) return;

	_function func;
	func.name = func_name;
	func.type = func_type;
	types.find(type_name.wstring());
	types.value().funcs.insert(func_name.wstring(), func);

}

void cwl::add_func_param (const owlsl::text& type_name, const owlsl::text& func_name, const owlsl::text& param_name, const owlsl::text& param_type)
{
	if (pass>0) return;

	_symbol param;
	param.name = param_name;
	param.type = param_type;

	types.find(type_name.wstring());
	types.value().funcs.find(func_name.wstring());
	types.value().funcs.value().params.insert(param_name.wstring(), param);
}

void cwl::push_context ()
{
//	if (0 == pass)
	{
		current_context->contexts.push_back(new _context());
		current_context->contexts.back()->parent = current_context;
		current_context = current_context->contexts.back();
	}
/*	else
	{
		current_context = current_context->contexts[current_context->context_iterator];
		current_context->parent->context_iterator++;
	}*/
}

void cwl::pop_context ()
{
	current_context = current_context->parent;
}

void cwl::add_context_var (const owlsl::text& var_name, const owlsl::text& var_type)
{
	//if (pass>0) return;

	_symbol symbol;
	symbol.name = var_name;
	symbol.type = var_type;
	current_context->var.insert(var_name.wstring(), symbol);
}

bool cwl::find_context_var (const owlsl::text& var_name, _symbol*& sym, bool& use_this)
{
	// See if it is a composed member (var1.var2.etc)
	owlsl::text symbol_name = var_name;
	owlsl::text remaining;
	size_t pos = symbol_name.find(".");

	if (pos != owlsl::text::max_length())
	{
		remaining = symbol_name.subtext(pos+1, symbol_name.length()-(pos+1));
		symbol_name = symbol_name.subtext(0, pos);
	}

	// See if it is a member variable of the current class
	if (find_member_var(current_class_name, var_name, sym))
	{
		use_this = true;
		return true;
	}
	else
	{
		use_this = false;
	}

	// See if the variable is present within actual contexts
	_context* icontext = current_context;
	while(true)
	{
		if (icontext->var.find(symbol_name.wstring()))
		{
			sym = &icontext->var.value();
			if (remaining.length())
			{
				if (!find_member_var(sym->type, remaining, sym)) return false;
				pos = remaining.find(".");
			}

			return true;
		}

		if (icontext->parent == NULL) break;
		icontext = icontext->parent;
	}

	// See if it is a function parameter
	if (types.find(current_class_name.wstring()))
	{
		if (types.value().funcs.find(current_func_name.wstring()))
		{
			if (types.value().funcs.value().params.find(var_name.wstring()))
			{
				sym = &types.value().funcs.value().params.value();
				return true;
			}
		}
	}

	return false;
}

bool cwl::find_member_var(const owlsl::text& type_name, const owlsl::text& var_name, _symbol*& sym)
{
	owlsl::text class_name = type_name;
	if (!types.find(class_name.wstring())) return false;

	owlsl::text symbol_name = var_name;
	size_t pos = var_name.find(".");
	if (pos == owlsl::text::max_length())
	{
		if (!types.value().props.find(symbol_name.wstring()))
			return false;
		sym = &types.value().props.value();
		return true;
	}

	owlsl::text remaining = var_name;

	while(pos != owlsl::text::max_length())
	{
		symbol_name = remaining.subtext(0, pos);

		if (!types.value().props.find(symbol_name.wstring()))
			return false;
		class_name = types.value().props.value().type;
		if (!types.find(class_name.wstring()))
			return false;

		remaining = remaining.subtext(pos+1, symbol_name.length()-(pos+1));
		pos = remaining.find(".");
	}
	if (remaining.length()>0) symbol_name = remaining;

	if (!types.value().props.find(symbol_name.wstring()))
		return false;
	sym = &types.value().props.value();
	return true;
}

owlsl::text cwl::get_call_symbol(const owlsl::text& func_call_text)
{
	owlsl::text symbol_name;
	owlsl::text remaining = func_call_text;
	size_t pos = func_call_text.find(".");
	while(pos != owlsl::text::max_length())
	{
		if (symbol_name.length()>0)
		{
			symbol_name += owlsl::text(".") + remaining.subtext(0, pos);
		}
		else
		{
			symbol_name = remaining.subtext(0, pos);
		}

		remaining = remaining.subtext(pos+1, symbol_name.length()-(pos+1));
		pos = remaining.find(".");
	}
	return symbol_name;
}

owlsl::text cwl::get_call_function(const owlsl::text& func_call_text)
{
	owlsl::text func_name = func_call_text;
	size_t pos = func_name.find(".");
	if (pos != owlsl::text::max_length())
	{
		while(pos != owlsl::text::max_length())
		{
			func_name = func_name.subtext(pos+1, func_name.length()-(pos+1));
			pos = func_name.find(".");
		}
	}
	return func_name;
}

void cwl::set_expression_type (const owlsl::text& type_name)
{
	if (expression_depth.size()>0) expression_depth.back() = type_name;
	if (current_expression_type.length()>0) return;
	current_expression_type = type_name;
}

void cwl::clear_expression_type ()
{
	current_expression_type = "";
}

owlsl::text cwl::get_func_type (const owlsl::text& type_name, const owlsl::text& func_name, const owlsl::text& func_params)
{
	if (sco.vm.table.contains(type_name))
	{
		owlsl::text fname = func_name + func_params;
		if (sco.vm.table[type_name].pmethod.contains(fname))
		{
			if (sco.vm.table[type_name].pretval.contains(fname))
			{
				return sco.vm.table.name(sco.vm.table[type_name].pretval[fname]);
			}
			else
			{
				return owlsl::text("void");
			}
		}
	}

	if (types.find(type_name.wstring()))
	{
		if (types.value().funcs.find(func_name.wstring()))
		{
			bool found = false;
			owlsl::text found_type;

			for ( ; !types.value().funcs.eof() && !found ;  types.value().funcs.move_next() )
			{
				// Check parameters match
				if (types.value().funcs.value().params.size()>0)
				{
					owlsl::text registered_params = "(";
					symbol_map&	params = types.value().funcs.value().params;
					params.move_first();
					while ( !params.eof())
					{
						registered_params += types.value().funcs.value().params.value().type + ", ";
						params.move_next();
					}
					// remove trailing ', '
					registered_params = registered_params.subtext(0,registered_params.length()-2);
					registered_params += ")";

					// check expected parameters match registered ones
					if (found = ( registered_params == func_params ))
					{
						found_type = types.value().funcs.value().type;
					}
				}
				else
				{
					// no params registered but we expect to have some
					if (found = (func_params == owlsl::text("()")) )
					{
						found_type = types.value().funcs.value().type;
					}
				}
			}

			if (!found)
			{
				return owlsl::text(); // not found
			}

			return found_type;
		}
	}
	return owlsl::text();
}

void cwl::push(const int& pos)
{
	sco.vm.push(pos);
}

void cwl::pop(const int& val)
{
	sco.vm.pop(val);
}

void cwl::heap_size	(const unsigned& value)
{
	sco.vm.heap.size_factor(value);
}

void cwl::set_syntax_context (const cwlsyntax::id& id)
{
	syntax_context_prev = syntax_context;
	syntax_context = id;
}

bool cwl::at_end_of_file (const unsigned& t)
{
	return ( t==tokens.size() );
}

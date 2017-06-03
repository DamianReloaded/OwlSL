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

#include "sco.h"
#include "vm.h"
#include "types/types.h"

using namespace owlsl;

bool sco::init_tokens()
{
	// Tokenizer
	tok.type.add("pushctx"		,	"opcode"			,	opcode::pushctx);
	tok.type.add("popctx"		,	"opcode"			,	opcode::popctx);
	tok.type.add("push"			,	"opcode"			,	opcode::push);
	tok.type.add("pop"			,	"opcode"			,	opcode::pop);
	tok.type.add("palloc"		,	"opcode"			,	opcode::palloc);
	tok.type.add("pcall"		,	"opcode"			,	opcode::pcall);
	tok.type.add("salloc"		,	"opcode"			,	opcode::salloc);
	tok.type.add("scall"		,	"opcode"			,	opcode::scall);	
	tok.type.add("return0"		,	"opcode"			,	opcode::sreturn0);
	tok.type.add("return1"		,	"opcode"			,	opcode::sreturn1);
	tok.type.add("ialloc"		,	"opcode"			,	opcode::ialloc);
	tok.type.add("jump"			,	"opcode"			,	opcode::jump);
	tok.type.add("jumpt"		,	"opcode"			,	opcode::jumpt);
	tok.type.add("jumpf"		,	"opcode"			,	opcode::jumpf);
	tok.type.add("\""			,	"string_literal"	,	tokenizer::token::string_literal);
	tok.type.add("\n"			,	"end_of_statement"	,	tokenizer::token::end_of_statement);
	tok.type.add("/*"			,	"block_comment"		,	tokenizer::token::block_comment);
	tok.type.add("//"			,	"line_comment"		,	tokenizer::token::comment);
	tok.type.add(","			,	"comma"				,	tokenizer::token::user_defined+1);
	tok.type.add("class"		,	"class"				,	tokenizer::token::user_defined+2);
	tok.type.add("{"			,	"curly_open"		,	tokenizer::token::user_defined+3);
	tok.type.add("}"			,	"curly_close"		,	tokenizer::token::user_defined+4);
	tok.type.add("property"		,	"class property"	,	tokenizer::token::user_defined+5);
	tok.type.add("function"		,	"class method"		,	tokenizer::token::user_defined+6);
	tok.type.add("("			,	"parentheses_open"	,	tokenizer::token::user_defined+7);
	tok.type.add(")"			,	"parentheses_close"	,	tokenizer::token::user_defined+8);
	tok.type.add("end"			,	"end function"		,	tokenizer::token::user_defined+9);
	tok.type.add(".main"		,	"entry point"		,	tokenizer::token::user_defined+10);
	tok.type.add(".host"		,	"exposed instances"	,	tokenizer::token::user_defined+11);

	tok.separator.add(" ", true);
	tok.separator.add("\"", true);
	tok.separator.add("\t", true);
	tok.separator.add(",");
	tok.separator.add("\n");
	//tok.separator.add("/");
	tok.separator.add("{");
	tok.separator.add("}");
	tok.separator.add("(");
	tok.separator.add(")");

	tok.blocks.string.set("\"", "\"");
	tok.blocks.comment.set("/*", "*/");
	tok.comment.set("//");

	return true;
}

bool sco::ignore (const unsigned& id)
{
	return ( (id == tokenizer::token::comment) || (id == tokenizer::token::block_comment ) || (id ==tokenizer::token::end_of_statement) || (id ==tokenizer::token::end_of_file) );
}

bool sco::parse_push (unsigned& t)
{
	++t;
	switch (tokens[t].type->id)
	{
		// string literal
		case tokenizer::token::string_literal:
		{
			parse_push_string(t);
		}
		break;

		// number literal
		case tokenizer::token::number_literal:
		{
			parse_push_number(t);
		}
		break;

		// boolean literal
		case tokenizer::token::boolean_literal:
		{
			parse_push_boolean(t);
		}
		break;

		// Variable
		case tokenizer::token::symbol:
		{
			if (!parse_push_symbol(t)) return false;
		}
	}

	return true;
}

bool sco::parse_ialloc (unsigned& t)
{
	if (0==pass)
	{
		t += 5;
		owlsl::text method_name;
		if (!parse_method_name(t, method_name)) return false;
		return true;
	}

	owlsl::text symbol_name = tokens[++t].text;
	++t; // ignore comma
	owlsl::text type_name = tokens[++t].text;
	++t;++t; // ignore comma
	owlsl::text method_name;
	if (!parse_method_name(t, method_name)) return false;

	if ( !table.host.find(symbol_name.wstring()) )
	{
		if (!vm.table.contains(type_name))
		{
			set_last_error(owlsl::text("Undefined type: '") + type_name + owlsl::text("'"), tokens[t-2].column, tokens[t-2].line-1);
			return false;
		}
		int type = vm.table.id(type_name);
		if (!vm.table[type_name].pconstruct.contains(method_name))
		{
			set_last_error(owlsl::text("Undefined interface constructor: '") + method_name + owlsl::text("'"), tokens[t].column, tokens[t].line-1);
			return false;
		}
		int method = vm.table[type].pconstruct.id(method_name);
		int pos = vm.pconstruct(type, method);
		vm.heap.mem[pos].refcount++;
		table.host.insert(symbol_name.wstring(), pos);
	}
	else
	{
		set_last_error(owlsl::text("Interface: '") + symbol_name + owlsl::text("' already defined."), tokens[t-4].column, tokens[t-4].line-1);
		return false;
	}
	//stack_pos++; // increment stack pos

	return true;
}

bool sco::parse_push_string	(unsigned& t)
{
	if (0==pass)
	{
		code_size +=2;
		return true;
	}

	tokenizer::token* token = &tokens[t];
	add_code(opcode::push, "push");
	if ( !table.string.find(token->text.wstring()) )
	{
		static int type_text = vm.table.id("text");
		static int constructor = vm.table[type_text].pconstruct.id("text()");
		int pos = vm.pconstruct(type_text, constructor);
		owlsl::text* data = vm.heap.cast<owlsl::text>(pos);
		vm.heap.mem[pos].refcount++;
		data->set(token->text.wstring());
		table.string.insert(data->wstring(), pos);
		table.string.find(token->text.wstring());
	}
	add_code(table.string.value(), token->text);
	stack_pos++; // increment stack pos

	return true;
}

bool sco::parse_push_number	(unsigned& t)
{
	if (0==pass)
	{
		code_size +=2;
		return true;
	}

	tokenizer::token* token = &tokens[t];
	add_code(opcode::push, "push");
	if ( !table.number.find(token->text.wstring()) )
	{
		static int type_number = vm.table.id("number");
		static int constructor = vm.table[type_number].pconstruct.id("number()");
		int pos = vm.pconstruct(type_number, constructor);
		owlsl::type::number* data = vm.heap.cast<owlsl::type::number>(pos);
		vm.heap.mem[pos].refcount++;
		data->set(token->text.to_float());
		table.number.insert(token->text.wstring(), pos);
		table.number.find(token->text.wstring());
	}
	add_code(table.number.value(), token->text);
	stack_pos++; // increment stack pos

	return true;
}

bool sco::parse_push_boolean (unsigned& t)
{
	if (0==pass)
	{
		code_size +=2;
		return true;
	}

	tokenizer::token* token = &tokens[t];
	add_code(opcode::push, "push");
	if ( !table.boolean.find(token->text.wstring()) )
	{
		static int type_bool = vm.table.id("bool");
		static int constructor = vm.table[type_bool].pconstruct.id("bool()");
		int pos = vm.pconstruct(type_bool, constructor);
		owlsl::type::boolean* data = vm.heap.cast<owlsl::type::boolean>(pos);
		vm.heap.mem[pos].refcount++;
		data->set(token->text.to_bool());
		table.boolean.insert(token->text.wstring(), pos);
		table.boolean.find(token->text.wstring());
	}
	add_code(table.boolean.value(), token->text);
	stack_pos++; // increment stack pos

	return true;
}

bool sco::parse_push_symbol (unsigned& t)
{
	unsigned levels = tokens[t].text.count(".");
	if (0==pass)
	{
		code_size +=2;
		if (levels>0)
		{
			code_size += 1+levels; // (number of levels), (level1), (level2), etc.
		}
		return true;
	}

	// get the name of the root variable
	owlsl::text var_name = tokens[t].text;

	if (levels>0)
	{
		add_code(opcode::pushc, "pushc"); // push scripted properties
	}
	else
	{
		if (table.host.find(var_name.wstring()))
		{
			add_code(opcode::push, "push"); // push using absolute heap position
			add_code(table.host.value(), var_name);
			stack_pos++; // increment stack pos
			return true;
		}
		else
		{
			add_code(opcode::pushs, "pushs"); // push using position relative to the context

			if (var_name == owlsl::text("this"))
			{
				int val = stack_pos + vm.table[current_type].sparameters[current_method_str]+1;
				add_code( -val, "this");
				stack_pos++; // increment stack pos
				return true;
			}
		}
	}

	unsigned dot_pos = var_name.find(".");
	if (dot_pos!=owlsl::text::max_length())
	{
		var_name = var_name.subtext(0, dot_pos);
	}

	// push the position of the root variable
	_symbol* sym = 0;
	bool is_this = var_name == owlsl::text("this");
	if (!find_symbol(var_name.wstring(), sym) && !is_this)
	{
		set_last_error(owlsl::text("Undefined symbol: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
		return false;
	}
	if (is_this)
	{
		int val = (vm.table[current_type].sparameters[current_method_str]+stack_pos+1); //+1 this
		add_code( -val, "this");
	}
	else
	{
		if (sym->parameter)
		{
			int current_pos = sym->pos;
			add_code( current_pos, var_name);
		}
		else
		{
			int current_pos = sym->pos+1;
			add_code(current_pos, var_name);
		}
	}
	stack_pos++; // increment stack pos

	// Push the position of each property level
	if (levels>0)
	{
		int		   type;
		if (is_this)
		{
			type = current_type;
		}
		else
		{
			type = symbol_type(var_name.wstring());
		}
		var_name = tokens[t].text;

		// push the number of levels
		add_code(levels, (uint32_t)levels);

		// add each level
		dot_pos = var_name.find(".");

		// Substract the root variable
		var_name = var_name.subtext(dot_pos+1, var_name.length()-dot_pos);
		dot_pos = var_name.find(".");
		owlsl::text  remaining = var_name.subtext(dot_pos+1, var_name.length()-dot_pos);

		dot_pos = var_name.find(".");

		while (true)
		{
			if (dot_pos!=owlsl::text::max_length())
			{
				var_name = var_name.subtext(0, dot_pos);
				
				if (!vm.table[type].sproperty.contains(var_name))
				{
					set_last_error(owlsl::text("Unknown symbol: '") + var_name + owlsl::text("'"), tokens[t].column, tokens[t].line-1);
					return false;
				}
								
				add_code( vm.table[type].sproperty.id(var_name), var_name);

				type = vm.table[type].sproperty[var_name];
				var_name = remaining;
				dot_pos = var_name.find(".");
				if (dot_pos!=owlsl::text::max_length())
				{
					remaining = var_name.subtext(dot_pos+1, var_name.length()-dot_pos);
				}
				dot_pos = var_name.find(".");
			}
			else
			{
				if (!vm.table[type].sproperty.contains(var_name))
				{
					set_last_error(owlsl::text("Unknown symbol: '") + var_name + owlsl::text("'"), tokens[t].column, tokens[t].line-1);
					return false;
				}

				add_code(vm.table[type].sproperty.id(var_name), var_name); // add the position of the property within the class
				break;
			}
		}
	}

	return true;
}

bool sco::parse_palloc (unsigned& t)
{
	if (0==pass)
	{
		code_size +=3;
		t += 5;
		owlsl::text method_name;
		if (!parse_method_name(t, method_name)) return false;
		return true;
	}

	tokenizer::token* token = &tokens[t];
	add_code(token->type->id, token->text);											// opcode
	owlsl::text symbol_name = tokens[++t].text;						// get symbol name
	if (context.back().symbol.find(symbol_name.wstring()))
	{
		set_last_error(owlsl::text("Variable: '") + tokens[t].text + owlsl::text("' already defined."), tokens[t].column, tokens[t].line);
		return false;
	}
	t += 2;															// ignore comma
	token = &tokens[t];
	owlsl::text class_name = token->text;
	if (!vm.table.contains(class_name))
	{
		set_last_error(owlsl::text("Undefined type: '") + class_name + owlsl::text("'"), tokens[t].column, tokens[t].line);
		return false;
	}
	add_code(vm.table.id(class_name), class_name);									// class type
	t+=2;
	token = &tokens[t];
	owlsl::text method_name;
	if (!parse_method_name(t, method_name)) return false;
	if (!vm.table[class_name].pconstruct.contains(method_name))
	{
		set_last_error(owlsl::text("Undefined constructor: '") + method_name + owlsl::text("'"), tokens[t].column, tokens[t].line);
		return false;
	}
	add_code( vm.table[class_name].pconstruct.id(method_name), class_name + owlsl::text("::") + method_name);			// method
	stack_pos -= vm.table[class_name].pparameters[method_name];		// pop parameters 
	_symbol sym;
	sym.pos = stack_pos; //-context.back()->pos;
	sym.type = vm.table.id(class_name);
	context.back().symbol.insert(symbol_name.wstring(), sym); // save this symbol and it's position relative to the context
	stack_pos++;													// increment stack pos

	return true;
}

bool sco::parse_method_name	(unsigned& t, owlsl::text& method_name)
{
	tokenizer::token* token = &tokens[t];
	method_name = token->text;
	token = &tokens[++t];
	while (L")" != token->text.wstring())
	{
		if (token->text.wstring() != L"," && token->text.wstring() != L"(" && token->text.wstring() != L")" && token->type->id != tokenizer::token::symbol)
		{
			set_last_error(owlsl::text("Unexpected: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
			return false;
		}
		method_name += token->text;
		if (token->text == owlsl::text(","))
		{
			method_name += " "; 
		}
		token = &tokens[++t];
	}
	method_name += token->text;

	return true;
}

bool sco::parse_pcall (unsigned& t)
{
	if (0==pass)
	{
		code_size +=3;
		t += 3;
		owlsl::text method_name;
		if (!parse_method_name(t, method_name)) return false;
		return true;
	}

	owlsl::text class_name = tokens[++t].text;
	if (!vm.table.contains(class_name))
	{
		set_last_error(owlsl::text("Undefined type: '") + class_name + owlsl::text("'"), tokens[t].column, tokens[t].line);
		return false;
	}

	t+=2;														// ignore comma

	owlsl::text method_name;
	int to_pop = 0;

	if (!parse_method_name(t, method_name)) return false;

	tokenizer::token_type* call_type = NULL;
	
	if (vm.table[class_name].pmethod.contains(method_name))
	{
		call_type = tok.type.get(opcode::pcall);
		add_code(call_type->id, call_type->text);					// pcall
		add_code( vm.table.id(class_name), class_name);				// class name
		add_code( vm.table[class_name].pmethod.id(method_name), class_name + owlsl::text("::") + method_name);			// method

		to_pop = vm.table[class_name].pparameters[method_name];	// pop parameters
		if (vm.table[class_name].pretval.contains(method_name))		// if return value
		{
			to_pop--;												// push return value
		}
	}
	else
	{
		if (vm.table[class_name].smethod.contains(method_name))
		{
			call_type = tok.type.get(opcode::scall);
			add_code(call_type->id, call_type->text);					// pcall
			add_code( vm.table.id(class_name), class_name);				// class name
			add_code( vm.table[class_name].smethod.id(method_name), class_name + owlsl::text("::") + method_name);			// method

			to_pop = vm.table[class_name].sparameters[method_name];	// pop parameters
			if (vm.table[class_name].sretval.contains(method_name))		// if return value
			{
				to_pop--;												// push return value
			}
		}
		else
		{
			set_last_error(owlsl::text("Undefined method: '") + method_name + owlsl::text("'"), tokens[t].column, tokens[t].line);
			return false;
		}
	}

	stack_pos -=  to_pop;  // pop parameters 

	return true;
}

bool sco::parse_salloc (unsigned& t)
{
	if (0==pass)
	{
		code_size +=3;
		t += 5;
		owlsl::text method_name;
		if (!parse_method_name(t, method_name)) return false;
		return true;
	}

	tokenizer::token* token = &tokens[t];
	add_code( token->type->id, token->text);											// opcode
	owlsl::text symbol_name = tokens[++t].text;						// get symbol name
	if (context.back().symbol.find(symbol_name.wstring()))
	{
		set_last_error(owlsl::text("Variable: '") + tokens[t].text + owlsl::text("' already defined."), tokens[t].column, tokens[t].line);
		return false;
	}
	t += 2;															// ignore comma
	token = &tokens[t];
	owlsl::text class_name = token->text;
	if (!vm.table.contains(class_name))
	{
		set_last_error(owlsl::text("Undefined type: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
		return false;
	}
	add_code( vm.table.id(class_name), class_name);									// class name
	t+=2;
	token = &tokens[t];
	owlsl::text method_name;
	if (!parse_method_name(t, method_name)) return false;
	// Do not check constructor because they're not implemented yet. using default
	/*if (!vm.table[class_name].sconstruct.contains(method_name))
	{
		set_last_error(owlsl::text("Undefined constructor: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
		return false;
	}*/
	add_code( 0, class_name + owlsl::text("::") + method_name); //vm.table[class_name].pconstruct.id(method_name);			// method
	//stack_pos -= vm.table[class_name].parameters[method_name];		// pop parameters 
	_symbol sym;
	sym.pos = stack_pos - context.back().pos;
	sym.type = vm.table.id(class_name);
	context.back().symbol.insert(symbol_name.wstring(), sym); // save this symbol and it's absolute stack position 
	stack_pos++;													// increment stack pos

	return true;
}

bool sco::parse_scall (unsigned& t)
{
	if (0==pass)
	{
		code_size +=3;
		t += 3;
		owlsl::text method_name;
		if (!parse_method_name(t, method_name)) return false;
		return true;
	}

	tokenizer::token* token = &tokens[t];
	add_code( token->type->id, token->text);
	token = &tokens[++t];
	owlsl::text class_name = token->text;
	if (!vm.table.contains(class_name))
	{
		set_last_error(owlsl::text("Undefined type: '") + class_name + owlsl::text("'"), tokens[t].column, tokens[t].line);
		return false;
	}
	add_code( vm.table.id(class_name), class_name);								// class name
	t+=2;														// ignore comma
	token = &tokens[t];
	owlsl::text method_name;
	if (!parse_method_name(t, method_name)) return false;
	if (!vm.table[class_name].smethod.contains(method_name))
	{
		set_last_error(owlsl::text("Undefined method: '") + method_name + owlsl::text("'"), tokens[t].column, tokens[t].line);
		return false;
	}
	add_code( vm.table[class_name].smethod.id(method_name), class_name + owlsl::text("::") + method_name);		// method

	int to_pop = vm.table[class_name].sparameters[method_name] + 1;	// pop parameters
	if (vm.table[class_name].sretval.contains(method_name))		// if return value
	{
		to_pop--;												// push return value
	}

	stack_pos -=  to_pop;  // pop parameters 

	return true;
}

bool sco::parse_return0 (unsigned& t)
{
	if (0==pass)
	{
		code_size ++;
		return true;
	}
	
	add_code( opcode::sreturn0, "sreturn0"); // push using position relative to the context	

	return true;
}

bool sco::parse_return1 (unsigned& t)
{
	if (0==pass)
	{
		code_size ++;
		return true;
	}

	add_code( opcode::sreturn1, "sreturn1"); // push using position relative to the context	
	stack_pos--;
	return true;
}


void sco::parse_pushctx ()
{
	if (1==pass)
	{
		push_context();
		add_code( opcode::pushctx, "pushctx");
	}
	else
	{
		code_size++;
	}
}

void sco::parse_popctx ()
{
	if (1==pass)
	{
		pop_context();
		add_code( opcode::popctx, "popctx");
	}
	else
	{
		code_size++;
	}
}

bool sco::parse_jump (unsigned& t)
{
	if (0==pass)
	{
		t += 2;
		code_size += 2;
		return true;
	}

	add_code( opcode::jump, "jump"); 
	tokenizer::token* token = &tokens[++t];
	if (!table.label.find(token->text.wstring()))
	{
		set_last_error(owlsl::text("Undefined label: '") + token->text + owlsl::text("'"), tokens[t].column, tokens[t].line);
		return false;
	}

	add_code( table.label.value(), token->text);
	
	return true;
}

bool sco::parse_jumpt (unsigned& t)
{
	if (0==pass)
	{
		t += 2;
		code_size += 2;
		return true;
	}

	add_code( opcode::jumpt, "jumpt"); 
	tokenizer::token* token = &tokens[++t];
	if (!table.label.find(token->text.wstring()))
	{
		set_last_error(owlsl::text("Undefined label: '") + token->text + owlsl::text("'"), tokens[t].column, tokens[t].line);
		return false;
	}

	add_code( table.label.value(), token->text);
	
	stack_pos--; // jumpt pops the value it evaluated

	return true;
}

bool sco::parse_jumpf (unsigned& t)
{
	if (0==pass)
	{
		t += 2;
		code_size += 2;
		return true;
	}

	add_code( opcode::jumpf, "jumpf"); 
	tokenizer::token* token = &tokens[++t];
	if (!table.label.find(token->text.wstring()))
	{
		set_last_error(owlsl::text("Undefined label: '") + token->text + owlsl::text("'"), tokens[t].column, tokens[t].line);
		return false;
	}

	add_code( table.label.value(), token->text);
	
	stack_pos--; // jumpf pops the value it evaluated

	return true;
}

bool sco::parse_pop (unsigned& t)
{
	if (0==pass)
	{
		t += 1;
		code_size += 1;
		return true;
	}

	add_code( opcode::pop, "pop"); 
	
	stack_pos--; 

	return true;
}

void sco::push_context()
{
	context.push_back(_context());
	context.back().pos = stack_pos;
}

void sco::pop_context()
{
	stack_pos = context.back().pos;
	context.pop_back();
}

bool sco::find_symbol(const std::wstring& name, _symbol*& sym)
{
	for (int i=context.size()-1; i>=0; --i)
	{
		if (context[i].symbol.find(name))
		{
			sym = &context[i].symbol.value();
			return true;
		}
	}
	return false;
}

int sco::symbol_type(const std::wstring& name)
{
	for (int i=context.size()-1; i>=0; --i)
	{
		if (context[i].symbol.find(name))
		{
			return context[i].symbol.value().type;
		}
	}
	return -1;
}

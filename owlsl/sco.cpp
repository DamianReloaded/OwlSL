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

using namespace owlsl;

sco::sco()
{
	init_tokens();
	init_interface();
}

sco::~sco()
{

}

void sco::set_last_error(const owlsl::text& description, const unsigned& column, const unsigned& line)
{
	m_last_error.column = column+1;
	m_last_error.line = line+1;
	m_last_error.description = description;
}

bool sco::open(const owlsl::path& filepath)
{
	owlsl::wfile f;

	if (!f.open(filepath))
	{
		m_last_error.description = owlsl::text("Couldn't open file: ") + filepath;
		return false;
	}

	if (!tok.parse(f, tokens))
	{
		m_last_error = tok.last_error();
		return false;
	}

	return true;
}

bool sco::append (const owlsl::text& code)
{
	owlsl::wfile f;

	std::vector<uint8_t> c;
	owlsl::text::get_mark(c, owlsl::encoding::utf16le);
	code.to_array(c, owlsl::encoding::utf16le);

	f.open(&c[0], c.size());
	if (!tok.parse(f, tokens))
	{
		m_last_error = tok.last_error();
		return false;
	}
	return true;
}

bool sco::compile (const int& _pass)
{
	for (pass=_pass; pass<2; pass++)
	{
		if (!parse()) return false;
	}

	return true;
}

bool sco::parse()
{
	code_size = 0;
	stack_pos = 0; //vm.stack.size();
	context.clear();

	syntax_context = scosyntax::root;

	tokenizer::token* token;
	for (unsigned t=0; t<tokens.size(); ++t)
	{
		token = &tokens[t];

		// Ignore comments, eof, bof and eos
		if ( ignore(token->type->id) )
		{
			continue;
		}

		switch (syntax_context)
		{
			case scosyntax::root:
			{
				if (token->text == owlsl::text("class"))
				{
					syntax_context = scosyntax::classtoken;
					continue;
				}
				else if (token->text == owlsl::text(".main"))
				{
					// This would be global routines. Program starts running from here.
					vm.entry_point = vm.code.size();
					parse_pushctx();
					syntax_context = scosyntax::funcbody;
				}
				else if (token->text == owlsl::text(".host"))
				{
					syntax_context = scosyntax::host;
				}
				else
				{
					set_last_error(owlsl::text("Unexpected: '") + token->text + owlsl::text("'"), token->column, token->line);
					return false;
				}
			}
			break;

			case scosyntax::host:
			{
				if (token->text == owlsl::text("class"))
				{
					syntax_context = scosyntax::root;
					t--;
					continue;
				}

				if (token->text == owlsl::text(".main"))
				{
					syntax_context = scosyntax::root;
					t--;
					continue;
				}
				if (token->text != owlsl::text("ialloc"))
				{
					set_last_error(owlsl::text("Expected 'ialloc'. Found: '") + token->text + owlsl::text("'"), token->column, token->line);
					return false;
				}

				if (!parse_ialloc(t)) return false;
			}
			break;

			case scosyntax::classtoken:
			{
				if (token->type->id == tokenizer::token::symbol)
				{
					if (0==pass)
					{
						current_type = vm.new_type(token->text, true);
					}
					else
					{
						current_type = vm.table.id(token->text);
					}
					syntax_context = scosyntax::classname;
				}
				else
				{
					set_last_error(owlsl::text("Unexpected: '") + token->text + owlsl::text("'"), token->column, token->line);
					return false;
				}
			}
			break;

			case scosyntax::classname:
			{
				if (owlsl::text("{") == token->type->text)
				{
					syntax_context = scosyntax::classbody;
				}
				else
				{
					set_last_error(owlsl::text("Unexpected: '") + token->text + owlsl::text("'"), token->column, token->line);
					return false;
				}
			}
			break;

			case scosyntax::classbody:
			{
				if (owlsl::text("property") == token->type->text)
				{
					syntax_context = scosyntax::proptoken;
				}
				else if (owlsl::text("}") == token->type->text)
				{
					syntax_context = scosyntax::root;
				}
				else if (owlsl::text("function") == token->type->text)
				{
					syntax_context = scosyntax::functoken;
				}
				else
				{
					set_last_error(owlsl::text("Unexpected: '") + token->text + owlsl::text("'"), token->column, token->line);
					return false;
				}
			}
			break;

			case scosyntax::functoken:
			{
				if (token->type->id == tokenizer::token::symbol)
				{
					current_method_type_str = token->text;
					token = &tokens[++t];

					if (token->type->id != tokenizer::token::symbol)
					{
						set_last_error(owlsl::text("Expected function name. Got: '") + token->text + owlsl::text("'"), token->column, token->line);
						return false;
					}

					current_method_str = token->text + owlsl::text("(");
					num_params = 0;
					syntax_context = scosyntax::funcname;

					stack_pos = 0;
				}
				else
				{
					set_last_error(owlsl::text("Unexpected: '") + token->text + owlsl::text("'"), token->column, token->line);
					return false;
				}
			}
			break;

			case scosyntax::funcname:
			{
				if (owlsl::text("(") == token->type->text)
				{
					current_param.clear();
					syntax_context = scosyntax::funcparams;
				}
				else
				{
					set_last_error(owlsl::text("Unexpected: '") + token->text + owlsl::text("'"), token->column, token->line);
					return false;
				}
			}
			break;

			case scosyntax::paramcomma:
			{
				current_method_str += owlsl::text(", ");
			}
			case scosyntax::funcparams:
			{
				if (owlsl::text(")") == token->type->text)
				{
					parse_end_params();
				}
				else if (token->type->id == tokenizer::token::symbol)
				{
					current_method_str += token->text;
					num_params++;
					if (0 == pass)
					{
						if (!vm.table.contains(token->text))
						{
							set_last_error(owlsl::text("Undefined type: '") + token->text + owlsl::text("'"), token->column, token->line);
							return false;
						}
					}

					if (1==pass)
					{
						_symbol sym;
						sym.pos = 0; // we don't know the position yet as they are inverted on the stack
						sym.type = vm.table.id(token->text);
						current_param.push_back(sym);
					}

					syntax_context = scosyntax::paramtype;
				}
				else
				{
					set_last_error(owlsl::text("Unexpected: '") + token->text + owlsl::text("'"), token->column, token->line);
					return false;
				}
			}
			break;

			case scosyntax::paramtype:
			{
				if (token->type->id == tokenizer::token::symbol)
				{
					if (1 == pass)
					{
						current_param.back().name = token->text;
					}
					syntax_context = scosyntax::paramname;
				}
				else
				{
					set_last_error(owlsl::text("Unexpected: '") + token->text + owlsl::text("'"), token->column, token->line);
					return false;
				}
			}
			break;

			case scosyntax::paramname:
			{
				if (owlsl::text(")") == token->type->text)
				{
					parse_end_params();
				}
				else if (owlsl::text(",") == token->type->text)
				{
					syntax_context = scosyntax::paramcomma;
				}
				else
				{
					set_last_error(owlsl::text("Unexpected: '") + token->text + owlsl::text("'"), token->column, token->line);
					return false;
				}
			}
			break;

			case scosyntax::funcbody:
			{
				// Parse end
				if (owlsl::text("end") == token->type->text)
				{
					parse_popctx();

					if (1==pass)
					{
						add_code( opcode::end, "end" );
					}
					else
					{
						code_size++;
					}
					syntax_context = scosyntax::classbody;
				}
				// parse opcode
				else if (token->type->name == owlsl::text("opcode"))
				{
					if (!parse_function(t, token)) return false;
				}
				// parse label
				else if (token->text.subtext(token->text.length()-1, 1).wstring() == L":")
				{
					if (0 == pass)
					{
						std::wstring label_name = token->text.subtext(0, token->text.length()-1).wstring();
						if (!table.label.find(label_name))
						{
							table.label.insert(label_name, code_size-1);
						}
						else
						{
							set_last_error(owlsl::text("Label : '") + label_name + owlsl::text("' already defined."), token->column, token->line);
							return false;
						}
					}
					continue;
				}
				// error
				else
				{
					set_last_error(owlsl::text("Unexpected: '") + token->text + owlsl::text("'"), token->column, token->line);
					return false;
				}
			}
			break;

			case scosyntax::proptoken:
			{
				if (token->type->id == tokenizer::token::symbol)
				{
					current_property_str = token->text;
					syntax_context = scosyntax::propname;
				}
				else
				{
					set_last_error(owlsl::text("Unexpected: '") + token->text + owlsl::text("'"), token->column, token->line);
					return false;
				}
			}
			break;

			case scosyntax::propname:
			{
				if (owlsl::text(",") == token->type->text)
				{
					syntax_context = scosyntax::propcomma;
				}
				else
				{
					set_last_error(owlsl::text("Unexpected: '") + token->text + owlsl::text("'"), token->column, token->line);
					return false;
				}
			}
			break;

			case scosyntax::propcomma:
			{
				if (token->type->id == tokenizer::token::symbol)
				{
					if (0 == pass)
					{
						vm.table[current_type].sproperty.add(current_property_str, vm.table.id(token->text));
					}
					syntax_context = scosyntax::classbody;
				}
				else
				{
					set_last_error(owlsl::text("Unexpected: '") + token->text + owlsl::text("'"), token->column, token->line);
					return false;
				}
			}
			break;

			default:
			{
				set_last_error(owlsl::text("Unexpected: '") + token->text + owlsl::text("'"), token->column, token->line);
				return false;
			}
			break;
		}
	}

	if (0 == pass)
	{
		vm.cursor = code_size;
	}

	return true;
}

bool sco::parse_function (unsigned& t, tokenizer::token* token)
{
	switch(token->type->id)
	{
		case opcode::pushctx:
		{
			parse_pushctx();
		}
		break;
		case opcode::popctx:
		{
			parse_popctx();
		}
		break;
		case opcode::push:
		{
			if (!parse_push(t)) return false;
		}
		break;
		case opcode::palloc:
		{
			if (!parse_palloc(t)) return false;
		}
		break;
		case opcode::pcall:
		{
			if (!parse_pcall(t)) return false;
		}
		break;
		case opcode::salloc:
		{
			if (!parse_salloc(t)) return false;
		}
		break;
		case opcode::scall:
		{
			if (!parse_scall(t)) return false;
		}
		break;
		case opcode::sreturn0:
		{
			if (!parse_return0(t)) return false;
		}
		break;
		case opcode::sreturn1:
		{
			if (!parse_return1(t)) return false;
		}
		break;
		case opcode::jump:
		{
			if (!parse_jump(t)) return false;
		}
		break;
		case opcode::jumpt:
		{
			if (!parse_jumpt(t)) return false;
		}
		break;
		case opcode::jumpf:
		{
			if (!parse_jumpf(t)) return false;
		}
		break;
		case opcode::pop:
		{
			if (!parse_pop(t)) return false;
		}
		break;
	}

	return true;
}

void sco::parse_end_params()
{
	current_method_str += owlsl::text(")");
	if (0==pass)
	{
		vm.table[current_type].smethod.add(current_method_str, code_size); // no code location yet.
		vm.table[current_type].sparameters.add(current_method_str, num_params); 
		if (current_method_type_str != owlsl::text("void"))
		{
			vm.table[current_type].sretval.add(current_method_str, vm.table.id(current_method_type_str));
		}
	}

	parse_pushctx();

	// save parameters as variables into the current context
	if (1==pass)
	{
		unsigned psize = current_param.size();
		for (unsigned i=0; i<psize; i++)
		{
			current_param[i].parameter = true;
			current_param[i].pos = ((psize==1)?0:i-(psize-1));
			context.back().symbol.insert(current_param[i].name.wstring(), current_param[i]); // save this symbol and it's position relative to the context
		}
	}
	syntax_context = scosyntax::funcbody;
}

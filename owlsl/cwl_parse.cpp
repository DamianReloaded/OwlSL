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
#include <cmath> // std::pow

using namespace owlsl;

bool cwl::init_tokens()
{
	tok.type.add("\""			,	"string_literal"	,	tokenizer::token::string_literal);
	tok.type.add(";"			,	"end_of_statement"	,	tokenizer::token::end_of_statement);
	tok.type.add("\n"			,	"end_of_line"		,	tokenizer::token::end_of_line);
	tok.type.add("/*"			,	"block_comment"		,	tokenizer::token::block_comment);
	tok.type.add("//"			,	"line_comment"		,	tokenizer::token::comment);

	tok.type.add("="			,	"assignment"		,	tokenizer::token::operation);
	tok.type.add("+"			,	"addition"			,	tokenizer::token::operation);
	tok.type.add("+="			,	"addition_set"		,	tokenizer::token::operation);
	tok.type.add("-"			,	"substraction"		,	tokenizer::token::operation);
	tok.type.add("-="			,	"substraction_set"	,	tokenizer::token::operation);
	tok.type.add("*"			,	"multiplication"	,	tokenizer::token::operation);
	tok.type.add("*="			,	"multiplication_set",	tokenizer::token::operation);
	tok.type.add("/"			,	"division"			,	tokenizer::token::operation);
	tok.type.add("/="			,	"division_set"		,	tokenizer::token::operation);
	tok.type.add("=="			,	"equal"				,	tokenizer::token::operation);
	tok.type.add("!"			,	"not"				,	tokenizer::token::operation);
	tok.type.add("!="			,	"not_equal"			,	tokenizer::token::operation);
	tok.type.add("<"			,	"less"				,	tokenizer::token::operation);
	tok.type.add(">"			,	"greater"			,	tokenizer::token::operation);
	tok.type.add("<="			,	"less_equal"		,	tokenizer::token::operation);
	tok.type.add(">="			,	"greater_equal"		,	tokenizer::token::operation);
	tok.type.add("&&"			,	"logical_and"		,	tokenizer::token::operation);
	tok.type.add("||"			,	"logical_or"		,	tokenizer::token::operation);

	tok.type.add(","			,	"comma"				,	tokenizer::token::user_defined+1);
	tok.type.add("class"		,	"class"				,	tokenizer::token::user_defined+2);
	tok.type.add("{"			,	"curly_open"		,	tokenizer::token::user_defined+3);
	tok.type.add("}"			,	"curly_close"		,	tokenizer::token::user_defined+4);
	tok.type.add("property"		,	"class property"	,	tokenizer::token::user_defined+5);
	tok.type.add("function"		,	"class method"		,	tokenizer::token::user_defined+6);
	tok.type.add("("			,	"parentheses_open"	,	tokenizer::token::user_defined+7);
	tok.type.add(")"			,	"parentheses_close"	,	tokenizer::token::user_defined+8);
	tok.type.add("host"			,	"host"				,	tokenizer::token::user_defined+9);
	tok.type.add(":"			,	"colon"				,	tokenizer::token::user_defined+10);


	tok.separator.add(" ", true);
	tok.separator.add("\"", true);
	tok.separator.add("\t", true);
	tok.separator.add(",");
	tok.separator.add(";");
	tok.separator.add(":");
	tok.separator.add("\n");
	tok.separator.add("/");
	tok.separator.add("{");
	tok.separator.add("}");
	tok.separator.add("(");
	tok.separator.add(")");
	tok.separator.add("=");
	tok.separator.add("+");
	tok.separator.add("+=");
	tok.separator.add("-");
	tok.separator.add("-=");
	tok.separator.add("*");
	tok.separator.add("*=");
	tok.separator.add("/");
	tok.separator.add("/=");
	tok.separator.add("==");
	tok.separator.add("!");
	tok.separator.add("!=");
	tok.separator.add("<");
	tok.separator.add(">");
	tok.separator.add("<=");
	tok.separator.add(">=");
	tok.separator.add("&&");
	tok.separator.add("||");


	tok.blocks.string.set("\"", "\"");
	tok.blocks.comment.set("/*", "*/");
	tok.comment.set("//");


	return true;
}

bool cwl::ignore (const unsigned& id)
{
	return ( (id == tokenizer::token::end_of_line) || (id ==tokenizer::token::end_of_file) );
}

bool cwl::parse_statement (unsigned& t, const bool& must_end_statement)
{
	// Is a type
	if (is_type(&tokens[t]))
	{
		// Is a symbol
		if (is_symbol(&tokens[t+1]))
		{
			// Is a variable declaration
			if (!parse_declaration(t)) return false;
		}
		// Followed by a '<' (template parameters)
		else if (is_less(&tokens[t+1]))
		{
			if (!parse_templated_var(t)) return false;
		}
	}
	// Is a symbol
	else if (is_symbol(&tokens[t]))
	{
		// Is a "return" statement?
		if (is_return(&tokens[t]))
		{
			if (!parse_return(t)) return false;
		}
		// Is an "if" statement?
		else if (is_if(&tokens[t]))
		{
			if (!parse_if(t)) return false;
			//--t; // go back to the previous token so the function parse increments to the proper next statement
		}
		// Is a "for" statement?
		else if (is_for(&tokens[t]))
		{
			if (!parse_for(t)) return false;
		}
		// Is a while statement?
		else if (is_while(&tokens[t]))
		{
			if (!parse_while(t)) return false;
		}
		// Is a switch statement?
		else if (is_switch(&tokens[t]))
		{
			if (!parse_switch(t)) return false;
		}
		// Followed by =
		else if (is_assignment(&tokens[t+1]))
		{
			if (!parse_assignment(t)) return false;
			if(must_end_statement)
			{
				if (!is_end_of_statement(&tokens[t+1]))
				{
					set_last_error(owlsl::text("Expected: 'End of Statement'. Got: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
					return false;
				}
				//++t;
			}
		}
		else if (is_operator(&tokens[t+1]))
		{
			if (!parse_expression(t)) return false;
			if(must_end_statement)
			{
				if (!is_end_of_statement(&tokens[t]))
				{
					if (!is_end_of_statement(&tokens[t+1]))
					{
						set_last_error(owlsl::text("Expected: 'End of Statement'. Got: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
						return false;
					}
				}
			}
		}
		// Followed by a parentheses
		else if (is_open_parent(&tokens[t+1]))
		{
			if (!parse_expression(t)) return false;
			if(must_end_statement)
			{
				if (!is_end_of_statement(&tokens[t]))
				{
					if (!is_end_of_statement(&tokens[t+1]))
					{
						if (!is_end_of_statement(&tokens[t-1]))
						{
							set_last_error(owlsl::text("Expected: 'End of Statement'. Got: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
							return false;
						}
					}
				}
			}
		}
		else
		{
			set_last_error(owlsl::text("Syntax error: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
			return false;
		}
	}
	else
	{
		if (!parse_expression(t)) return false;
		if(must_end_statement)
		{
			if (!is_end_of_statement(&tokens[t]))
			{
				if (!is_end_of_statement(&tokens[t+1]))
				{
					set_last_error(owlsl::text("Expected: 'End of Statement'. Got: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
					return false;
				}
			}
		}
	}
	if(must_end_statement)
	{
		current_expression_type.clear();
		expression_depth.clear();
	}

	return true;
}

bool cwl::parse_while (unsigned& t)
{
	static const unsigned label_offset = 2000000;

	if (!is_open_parent(&tokens[++t]))
	{
		set_last_error(owlsl::text("Expected: '('. Got: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
		return false;
	}

	// Loop label
	++label_count;
	unsigned loop_label = label_count + label_offset;
	write_assembly( indent( owlsl::text((uint32_t)loop_label) + ":",0,0) + owlsl::endl );


	if (!parse_expression(++t)) return false;

	if (current_expression_type != "bool")
	{
		set_last_error(owlsl::text("Condition must resolve to boolean. ')'. Got: '") + current_expression_type + owlsl::text("'"), tokens[t].column, tokens[t].line);
		return false;
	}

	current_expression_type.clear(); // Reset the expression type

	// write the jump if true
	++label_count;
	unsigned true_label = label_count + label_offset;
	write_assembly( indent("jumpt",8,4) );
	write_assembly(  owlsl::text((uint32_t)true_label) + owlsl::endl );

	// write the jump to else (if jumpt isn't executed because condition is false)
	write_assembly( indent("jump",8,4) );
	write_assembly(  owlsl::text((uint32_t)true_label+1) + owlsl::endl );

	// Write the label jumpt will jump to if the condition is true
	write_assembly( indent( owlsl::text((uint32_t)true_label) + ":",0,0) + owlsl::endl );

	// Save the name of the label when the condition is false
	++label_count;
	label_depth.push_back(label_count);

	if (!is_close_parent(&tokens[t]))
	{
		if (!is_close_parent(&tokens[++t]))
		{
			set_last_error(owlsl::text("Expected: ')'. Got: '") + tokens[t].text , tokens[t].column, tokens[t].line);
			return false;
		}
	}

	++t; // skip close paren

	if (!is_open_curly(&tokens[t]))
	{
		set_last_error(owlsl::text("Expected: '{'. Got: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
		return false;
	}

	++t; //skip open curly

	push_context();
	write_assembly( indent("pushctx",8,4) + owlsl::endl );

	// Parse the code for the iteration
	++label_count;
	while (!is_close_curly(&tokens[t]))
	{
		//if (ignore(tokens[t+1].type->id)) while ( ignore(tokens[++t].type->id) ); // ignore characters
		if (!parse_statement(t))
		{
			return false;
		}

		if (is_end_of_statement(&tokens[t]))
		{
			++t; // skip end of statement
		}
		else if (is_end_of_statement(&tokens[t+1])) t+=2; // skip end of statement
	}
	--label_count;

	// Pop the context
	pop_context();
	write_assembly( indent("popctx",8,4) + owlsl::endl );

	// write the jump to beggining of loop
	write_assembly( indent("jump",8,4) );
	write_assembly(  owlsl::text((uint32_t)loop_label) + owlsl::endl );

	// Write the "end while" label
	unsigned end_label = label_depth.back() + label_offset;
	write_assembly( indent( owlsl::text((uint32_t)end_label) + ":",0,0) + owlsl::endl );
	label_depth.pop_back();

	t++; // skip curly close

	return true;
}

bool cwl::parse_for (unsigned& t)
{
	static const unsigned label_offset = 3000000;

	if (!is_open_parent(&tokens[++t]))
	{
		set_last_error(owlsl::text("Expected: '('. Got: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
		return false;
	}

	push_context();
	write_assembly( indent("pushctx",8,4) + owlsl::endl );

	if (!parse_statement(++t))
	{
		return false;
	}

	if (!is_end_of_statement(&tokens[t]))
	{
		set_last_error(owlsl::text("Expected: '('. Got: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
		return false;
	}
	++t; //ignore ;

	// Loop label
	++label_count;
	unsigned loop_label = label_count + label_offset;
	write_assembly( indent( owlsl::text((uint32_t)loop_label) + ":",0,0) + owlsl::endl );

	if (!parse_expression(t)) return false;

	if (current_expression_type != "bool")
	{
		set_last_error(owlsl::text("Condition must resolve to boolean. ')'. Got: '") + current_expression_type + owlsl::text("'"), tokens[t].column, tokens[t].line);
		return false;
	}

	current_expression_type.clear(); // Reset the expression type

	if (!is_end_of_statement(&tokens[++t]))
	{
		set_last_error(owlsl::text("Expected 'End of statement'. Got: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
		return false;
	}

	// write the jump if true
	++label_count;
	unsigned true_label = label_count + label_offset;
	write_assembly( indent("jumpt",8,4) );
	write_assembly(  owlsl::text((uint32_t)true_label) + owlsl::endl );

	// write the jump to else (if jumpt isn't executed because condition is false)
	write_assembly( indent("jump",8,4) );
	write_assembly(  owlsl::text((uint32_t)true_label+1) + owlsl::endl );

	// Write the label jumpt will jump to if the condition is true
	write_assembly( indent( owlsl::text((uint32_t)true_label) + ":",0,0) + owlsl::endl );

	// Save the name of the label when the condition is false
	++label_count;
	label_depth.push_back(label_count);

	// Save the code location for the operation to perform, we will parse it later.
	unsigned operation_pos = ++t;
	while (!is_close_parent(&tokens[t]))
	{
		++t;
		if (t>=tokens.size())
		{
			set_last_error(owlsl::text("Expected: ')'. Got: End of File.'") , tokens[t].column, tokens[t].line);
			return false;
		}
	}

	++t; // skip close parenthesys

	if (!is_open_curly(&tokens[t]))
	{
		set_last_error(owlsl::text("Expected: '{'. Got: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
		return false;
	}

	++t; // skip open curly

	push_context();
	write_assembly( indent("pushctx",8,4) + owlsl::endl );

	// Parse the code for the iteration
	++label_count;
	while (!is_close_curly(&tokens[t]))
	{
		//while ( ignore(tokens[++t].type->id) ); // ignore characters

		if (!parse_statement(t))
		{
			return false;
		}

		if (is_end_of_statement(&tokens[t]))
		{
			++t; // skip end of statement
		}
		else if (is_end_of_statement(&tokens[t+1])) t+=2; // skip end of statement
	}
	--label_count;

	// Save the position of the end curly to go back there after parsing the operation for the loop
	unsigned end_curly_pos = t;

	// Perform the operation each loop must do at the end
	t = operation_pos;

	if (!is_close_parent(&tokens[t])) // expression may be empty
	{
		if (!parse_expression(t, false)) return false;
	}

	pop_context();
	write_assembly( indent("popctx",8,4) + owlsl::endl );

	// write the jump to beggining of loop
	write_assembly( indent("jump",8,4) );
	write_assembly(  owlsl::text((uint32_t)loop_label) + owlsl::endl );

	// Write the "end for" label
	unsigned end_label = label_depth.back() + label_offset;
	write_assembly( indent( owlsl::text((uint32_t)end_label) + ":",0,0) + owlsl::endl );
	label_depth.pop_back();

	// Pop the context
	pop_context();
	write_assembly( indent("popctx",8,4) + owlsl::endl );

	t = end_curly_pos;
	t++; // skip curly close

	++label_count;
	++label_count;

	return true;
}

bool cwl::parse_switch (unsigned& t)
{
	static const unsigned label_offset = 4000000;

	if (!is_open_parent(&tokens[++t]))
	{
		set_last_error(owlsl::text("Expected: '('. Got: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
		return false;
	}

	// Save the code location for the switch expression, we will push it in each case
	unsigned switch_expression_pos = t;
	while (!is_close_parent(&tokens[t]))
	{
		++t;
		if (t>=tokens.size())
		{
			set_last_error(owlsl::text("Expected: ')'. Got: End of File.'") , tokens[t].column, tokens[t].line);
			return false;
		}
	}

	++t; // skip close parent

	if (!is_open_curly(&tokens[t]))
	{
		set_last_error(owlsl::text("Expected: '{'. Got: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
		return false;
	}

	++t; // skip open curly

	owlsl::text next_label_name;

	// Save the end_switch label
	++label_count;

	unsigned end_label = label_count + label_offset;
	owlsl::text end_switch_label_name =  owlsl::text((uint32_t)end_label);

	bool is_default_label = false;
	tokenizer::token* default_token = NULL;

	// Parse switch context
	while (!is_close_curly(&tokens[t]))
	{
		if (is_default_label) // Sintax error. Default must be the last label
		{
			if (is_close_curly(&tokens[t])) break;

			set_last_error(owlsl::text("Error. Case 'default' must be written last."), default_token->column, default_token->line);
			return false;
		}

		is_default_label = is_default(&tokens[t]);

		if (is_default_label)
		{
			default_token = &tokens[t];
		}

		if (is_case(&tokens[t]) || is_default_label)
		{
			if (!is_default_label)
			{
				unsigned case_expression_pos = ++t;

				// Push the switch expression
				t = switch_expression_pos;
				if (!parse_expression(t)) return false;
				owlsl::text switch_type = current_expression_type;
				clear_expression_type();

				// Push the case expression
				t = case_expression_pos;
				if (!parse_expression(t)) return false;
				owlsl::text case_type = current_expression_type;
				clear_expression_type();

				// Types must match
				if (switch_type != case_type)
				{
					set_last_error(owlsl::text("Invalid expression type. Expected '") + switch_type + owlsl::text("'. Got: '") + case_type + owlsl::text("'"), tokens[t].column, tokens[t].line);
					return false;
				}

				// Write == method
				if (!write_method_call(&tokens[t], switch_type, "==", case_type)) return false;

				// If comparision is false, jump to the next label
				++label_count;
				unsigned next_label = label_count + label_offset;
				next_label_name =  owlsl::text((uint32_t)next_label);
				write_assembly( indent("jumpf",8,4) );
				write_assembly( next_label_name + owlsl::endl );
			}

			if (!is_colon(&tokens[++t]))
			{
				set_last_error(owlsl::text("Expected: ':'. Got: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
			}

			++t; // skip colon

			if (!is_open_curly(&tokens[t]))
			{
				set_last_error(owlsl::text("Expected: '{'. Got: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
				return false;
			}

			++t; // skip open curly

			push_context();
			write_assembly( indent("pushctx",8,4) + owlsl::endl );

			// Parse the case context
			++label_count;
			while (!is_close_curly(&tokens[t]))
			{
				//while ( ignore(tokens[++t].type->id) ); // ignore characters
				if (!parse_statement(t))
				{
					return false;
				}

				if (is_end_of_statement(&tokens[t]))
				{
					++t; // skip end of statement
				}
				else if (is_end_of_statement(&tokens[t+1])) t+=2; // skip end of statement
			}


			++t; // skip close curly

			// Pop the context
			pop_context();
			write_assembly( indent("popctx",8,4) + owlsl::endl );

			// Jump to end switch label
			write_assembly( indent("jump",8,4) );
			write_assembly( end_switch_label_name + owlsl::endl );

			if (!is_default_label) // Sintax error. Default must be the last label
			{
				write_assembly( indent(next_label_name + ":",0,0) + owlsl::endl );
			}
		}
		//t++; // ignore case end curly
	}

	write_assembly( indent(end_switch_label_name + ":",0,0) + owlsl::endl );

	t++; // ignore switch end curly

	return true;
}

bool cwl::parse_if (unsigned& t)
{
	static const unsigned label_offset = 5000000;

	if (!is_open_parent(&tokens[++t]))
	{
		set_last_error(owlsl::text("Expected: '('. Got: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
		return false;
	}

	parsing_bool_condition = true;
	if (!parse_expression(t)) return false;
	parsing_bool_condition = false;

	if (current_expression_type != "bool")
	{
		set_last_error(owlsl::text("Condition must resolve to boolean. ')'. Got: '") + current_expression_type + owlsl::text("'"), tokens[t].column, tokens[t].line);
		return false;
	}

	clear_expression_type ();

	if (!is_close_parent(&tokens[t]))
	{
		if (!is_close_parent(&tokens[t-1]))
		{
			set_last_error(owlsl::text("Expected: ')'. Got: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
			return false;
		}
	}

	// write the jump if true
	++label_count;
	unsigned true_label = label_count + label_offset;

	write_assembly( indent("jumpt",8,4) );
	write_assembly(  owlsl::text((uint32_t)true_label) + owlsl::endl );

	// write the jump to else (if jumpt isn't executed because condition is false)
	write_assembly( indent("jump",8,4) );
	write_assembly(  owlsl::text((uint32_t)true_label+1) + owlsl::endl );

	// Write the label jumpt will jump to if the condition is true
	write_assembly( indent( owlsl::text((uint32_t)true_label) + ":",0,0) + owlsl::endl );

	// Save the name of the label when the condition is false
	++label_count;
	label_depth.push_back(label_count);

	if (!is_open_curly(&tokens[t]))
	{
		if (!is_open_curly(&tokens[++t]))
		{
			set_last_error(owlsl::text("Expected: '{'. Got: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
			return false;
		}
	}

	++t; // skip open curly

	push_context(); // is curly
	write_assembly( indent("pushctx",8,4) + owlsl::endl );

	++label_count;
	while (!is_close_curly(&tokens[t]))
	{
		if (!parse_statement(t))
		{
			return false;
		}

		if (is_end_of_statement(&tokens[t]))
		{
			++t; // skip end of statement
		}
		else if (is_end_of_statement(&tokens[t+1])) t+=2; // skip end of statement
	}
	--label_count;

	++t; // skip close curly

	pop_context(); // is close curly
	write_assembly( indent("popctx",8,4) + owlsl::endl );

	// write the jump to the end of the if block (jump over "else" if any)
	unsigned end_label = label_depth.back()+1 + label_offset;
	write_assembly( indent("jump",8,4) );
	write_assembly(  owlsl::text((uint32_t)end_label) + owlsl::endl );

	// Write the label jumpt will jump to if the condition is true
	unsigned tmp_label = label_depth.back() + label_offset;
	write_assembly( indent( owlsl::text((uint32_t)tmp_label) + ":",0,0) + owlsl::endl );

	if (!is_else(&tokens[t]))
	{
		write_assembly( indent( owlsl::text((uint32_t)tmp_label+1) + ":",0,0) + owlsl::endl );
		label_depth.pop_back();
		++label_count;
		return true; // No 'else' get out of here.
	}

	++t; // skip else

	// Is an "if" statement?
	if (is_if(&tokens[t]))
	{
		++label_count;
		if (!parse_if(t)) return false;
		--label_count;
		tmp_label = label_depth.back()+ 1 + label_offset;
		write_assembly( indent( owlsl::text((uint32_t)tmp_label) + ":",0,0) + owlsl::endl );
		label_depth.pop_back();
		++label_count;
		return true; // No 'else' get out of here.
	}

	if (!is_open_curly(&tokens[t]))
	{
		set_last_error(owlsl::text("Expected: '{'. Got: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
		return false;
	}

	++t; // skip open curly

	push_context(); // is curly
	write_assembly( indent("pushctx",8,4) + owlsl::endl );

	++label_count;
	++label_count;
	while (!is_close_curly(&tokens[t]))
	{
		if (!parse_statement(t))
		{
			return false;
		}

		if (is_end_of_statement(&tokens[t]))
		{
			++t; // skip end of statement
		}
		else if (is_end_of_statement(&tokens[t+1])) t+=2; // skip end of statement
	}
	--label_count;
	--label_count;

	pop_context(); // is close curly
	write_assembly( indent("popctx",8,4) + owlsl::endl );

	// Write the "end if" label
	tmp_label = label_depth.back()+1 + label_offset;
	write_assembly( indent( owlsl::text((uint32_t)tmp_label) + ":",0,0) + owlsl::endl );
	label_depth.pop_back();

	++label_count;
	++label_count;

	++t; // skip close curly

	return true;
}

bool cwl::parse_assignment	(unsigned& t)
{
	if (is_close_parent(&tokens[t]))
	{
		set_last_error(owlsl::text("Cannot assign to return value or unexpected parentheses."), tokens[t].column, tokens[t].line);
		return false;
	}

	owlsl::text symbol_name = tokens[t].text;
	owlsl::text type_name;
	_symbol* sym = NULL;
	bool use_this = false;
	if (find_context_var(symbol_name, sym, use_this))
	{
		type_name = sym->type;
	}
	else
	{
		if (is_literal(&tokens[t]))
		{
			set_last_error(owlsl::text("Cannot assign value to constant: '") + symbol_name + owlsl::text("'"), tokens[t].column, tokens[t].line);
		}
		else
		{
			set_last_error(owlsl::text("Undefined symbol: '") + symbol_name + owlsl::text("'"), tokens[t].column, tokens[t].line);
		}
		return false;
	}
	bool is_pcall = is_ptype(type_name);
	if (use_this)
	{
		symbol_name = owlsl::text("this.") + symbol_name;
	}
	else
	{
		if (symbol_name.contains("."))
		{
			set_last_error(owlsl::text("Property '") + symbol_name + owlsl::text("' is private."), tokens[t].column, tokens[t].line);
			return false;
		}
	}
	write_assembly( indent("push",8,4) + symbol_name + owlsl::endl );
	++t; // Ignore '='
	if (!parse_expression(++t, true)) return false;

	// Set the assignment function name
	owlsl::text func_name = owlsl::text("=(") + current_expression_type + ")";

	// Check operation exists
	if (sco.vm.table.contains(type_name))
	{
		if (!sco.vm.table[type_name].pmethod.contains(func_name))
		{
			set_last_error(owlsl::text("Unknown method: ") + type_name + owlsl::text(" ") + func_name, tokens[t].column, tokens[t].line);
			return false;
		}
	}

	if (is_pcall)
	{
		write_assembly( indent("pcall",8,4) );
	}
	else
	{
		write_assembly( indent("scall",8,4) );
	}

	//write_assembly( type_name + ", =(" + expression_depth.back() + ")" + owlsl::endl );
	write_assembly( type_name + ", =(" + current_expression_type + ")" + owlsl::endl );

	return true;
}

bool cwl::parse_return (unsigned& t)
{
	if (!is_end_of_statement(&tokens[t+1]))
	{
		if (!parse_expression(++t)) return false;
		write_assembly( indent("return1",8,4) + owlsl::endl );
		write_assembly( indent("jump",8,4) );
		write_assembly(  owlsl::text((uint32_t)label_function_end) + owlsl::endl );
	}
	else
	{
		write_assembly( indent("return0",8,4) );
		write_assembly( indent("jump",8,4) );
		write_assembly(  owlsl::text((uint32_t)label_function_end) + owlsl::endl );
	}

	write_assembly( owlsl::endl );
	if (!is_end_of_statement(&tokens[t+1]))
	{
		set_last_error(owlsl::text("Expected: end of statement got: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
		return false;
	}
	return true;
}

bool cwl::parse_func_call (unsigned& t, const owlsl::text& subcall_type)
{
	owlsl::text token_text = tokens[t].text;
	owlsl::text symbol_name;
	// Check if it is a subcall
	bool is_subcall = subcall_type.length() > 0;
	bool is_property = false;

	if (is_subcall)
	{
		token_text = token_text.subtext(1, token_text.length());
		if (symbol_name.contains("."))
		{
			set_last_error(owlsl::text("Property '") + symbol_name + owlsl::text("' is private."), tokens[t].column, tokens[t].line);
			return false;
		}
	}
	else
	{
		symbol_name = get_call_symbol(token_text);
		if (symbol_name.contains("."))
		{
			set_last_error(owlsl::text("Property '") + symbol_name + owlsl::text("' is private."), tokens[t].column, tokens[t].line);
			return false;
		}
	}
	owlsl::text func_name = get_call_function(token_text);

	unsigned func_tok_pos = t;
	unsigned subcall_pos = 0; // for when: func(param).func() (calling a function from the return type)

	_symbol* sym = NULL;
	bool use_this = false;
	if (symbol_name.length()>0 && !is_subcall)
	{
		if (!find_context_var(symbol_name, sym, use_this))
		{
			set_last_error(owlsl::text("Undefined symbol: '") + symbol_name + owlsl::text("'. If it is a constant, enclose it within parentheses to call methods from it."), tokens[t].column, tokens[t].line);
			return false;
		}
		if (use_this) symbol_name = owlsl::text("this.") + symbol_name;
		write_assembly( indent("push",8,4) + symbol_name + owlsl::endl );
	}

	if (!parse_func_params(++t)) return false;

	// If it's not inside an expression check for subcalls and end of line
	if (expression_depth.size()==0)
	{
		++t;
		if (tokens[t].text.find(".") == 0) //next character is a dot
		{
			subcall_pos = t; // Save the location of the subcall
			is_property = !is_open_parent(&tokens[t+1]);
		}
		else if (!is_end_of_statement(&tokens[t]))
		{
			set_last_error(owlsl::text("Expected: end of statement got: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
			return false;
		}
	}
	else
	{
		if (tokens[t+1].text.find(".") == 0) //next character is a dot
		{
			subcall_pos = t+1; // Save the location of the subcall
			is_property = !is_open_parent(&tokens[t+2]);
		}
	}

	// parse parameters
	owlsl::text param_types;
	std::vector<owlsl::text> param_types_list = current_param_types.back();
	for (unsigned i=0; i<current_param_types.back().size(); i++)
	{
		if (param_types.length()>0)
		{
			param_types += ", ";
		}
		param_types += current_param_types.back()[i];
	}
	current_param_types.pop_back();

	owlsl::text type = subcall_type;
	if ( is_subcall || (sym != NULL) )
	{
		if (sym != NULL) type = sym->type;

		if (is_ptype(type))
		{
			write_assembly( indent("pcall",8,4) );
		}
		else
		{
			write_assembly( indent("scall",8,4) );
		}
	}
	else
	{
		write_assembly( indent("push",8,4) );
		write_assembly( "this" );
		write_assembly( owlsl::endl );

		type = this->current_class_name;
		write_assembly( indent("scall",8,4) );
	}

	owlsl::text func_params = owlsl::text("(") + param_types + ")";
	owlsl::text complete_func_name = func_name + func_params;
	// Set the expression type to the return value if not void
	owlsl::text func_type = get_func_type(type, func_name, func_params);
	
	// If the function was found, write the assembly code
	if (func_type.length()>0)
	{
		write_assembly( type + ", " + complete_func_name + owlsl::endl );
	}
	else // If func_type is empty, the function wasn't found. Let's see if there is a function with 'object' as parameters and use that one.
	{
		size_t permutations = (size_t)(std::pow(2.0, (int)param_types_list.size())-1);
	
		for (size_t i=1; i<=permutations && i<7; i++)
		{
			// See if there is a function with some paramater as 'object' instead of the original given type
			func_params = owlsl::text("(")+ parse_obj_params(i,param_types_list) +")";
			owlsl::text complete_func_name = func_name + func_params;
			func_type = get_func_type(type, func_name, func_params);
			
			// If the function was found, write the assembly code
			write_assembly( type + ", " + complete_func_name + owlsl::endl );
			// Stop looking for functions
			if (func_type.length()>0) break;
		}
	}
	
	// Continue parsing
	if (func_type != "void") set_expression_type(func_type);
	if (1 == pass)
	{
		if (func_type.length()==0)
		{
			set_last_error(owlsl::text("Method not found: '") + type + "." + complete_func_name + owlsl::text("'"), tokens[func_tok_pos].column, tokens[t].line);
			return false;
		}

		if (func_type != "void")
		{
			// If this function isn't part of an expression or won't call another method, then pop that value because nobody will use it.
			if (expression_depth.size()==0 && (subcall_pos==0) )
			{
				write_assembly( indent("pop",8,4) ); // pop retval
				write_assembly( owlsl::endl );
			}
		}
		else
		{
			clear_expression_type();
		}
	}

	if (subcall_pos>0)
	{
		if (func_type == "void")
		{
			set_last_error(owlsl::text("Method '") + complete_func_name + owlsl::text("' returns 'void'. It cannot call a method."), tokens[func_tok_pos].column, tokens[t].line);
		}
		t = subcall_pos;
		if (is_property)
		{
			is_property = is_property; // TODO: Handle properties from return types.
		}
		else
		{
			return parse_func_call(t, func_type);
		}
	}

	++t; //ignore trailing ')'

	return true;
}

// Hack: only 7 levels of permutations (3 parameters) [because i'm cheap]
owlsl::text	cwl::parse_obj_params (const size_t& _perm, const std::vector<owlsl::text>& _params)
{
	owlsl::text params;
	
	for (unsigned i=0; i<_params.size(); i++)
	{
		if (params.length()>0)
		{
			params += ", ";
		}

		switch (_perm)
		{
			case 1: if (0==i)				params += "object"; else params +=_params[i]; break;
			case 2: if (1==i)				params += "object"; else params +=_params[i]; break;
			case 3: if (0==i||1==i)			params += "object"; else params +=_params[i]; break;
			case 4: if (2==i)				params += "object"; else params +=_params[i]; break;
			case 5: if (0==i||2==i)			params += "object"; else params +=_params[i]; break;
			case 6:	if (1==i||2==i)			params += "object"; else params +=_params[i]; break;
			case 7: if (0==i||1==i||2==i)	params += "object"; else params +=_params[i]; break;
		}
	}
	
	return params;
}

bool cwl::parse_func_params (unsigned& t)
{
	current_param_types.push_back(vector_text());

	// parse first parameter, if any
	if (is_close_parent(&tokens[t+1]))
	{
		++t;
		return true;
	}

	if (!parse_expression(++t, true)) return false;
	if (current_expression_type.length()>0)
	{
		current_param_types.back().push_back(current_expression_type);
		clear_expression_type();
		// no more parameters. exit
		if ( is_close_parent(&tokens[t]) && tokens[t+1].text.subtext(0,1) != "." ) //check there isn't a subcall.
		{
			if (tokens[t+1].text.subtext(0,1) != "," && tokens[t+1].text.subtext(0,1) != ";") // do not advance if comma
			{
				++t;
			}
			return true;
		}
		// parse remaining paramters;
		bool multiple_params = false;

		if (is_comma(&tokens[t+1]))
		{
			t++; //find comma if any
		}
		while (is_comma(&tokens[t]))
		{
			multiple_params = true;

			if (!parse_expression(++t)) return false;
			if (current_expression_type.length()>0)
			{
				current_param_types.back().push_back(current_expression_type);
				clear_expression_type();
			}
			else
			{
				set_last_error(owlsl::text("Expected expression '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
				return false;
			}
		}

		if (multiple_params) --t; //if multiple parameters, decrement the token count
	}

	if (!is_close_parent(&tokens[++t]))
	{
		if (!is_close_parent(&tokens[--t]))
		{
			if (!is_close_parent(&tokens[--t]))
			{
				set_last_error(owlsl::text("Expected ')'. Got: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
				return false;
			}
		}
	}

	return true;
}

bool cwl::parse_templated_var (unsigned& t, const bool& is_class_property)
{
	// specialize templated type
	size_t from = t;
	owlsl::text original_type = tokens[t].text;
	owlsl::text new_type = original_type;
	t += 2; //ignore '<'

	std::deque<owlsl::text> tparams;
	while (tokens[t].text != ">")
	{
		if (is_type(&tokens[t]))
		{
			if (is_less(&tokens[t+1]))
			{
				parse_templated_var(t);
				t--; // re-parse the new type
			}
			else
			{
				tparams.push_back(tokens[t].text);
				new_type += owlsl::text("@") + tokens[t].text + "@";
			}
		}
		else if (is_comma(&tokens[t]))
		{}
		else
		{
			set_last_error(owlsl::text("Unknown type: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
			return false;
		}
		t++;
	}

	// Check closure is present
	if (!is_greater(&tokens[t]))
	{
		set_last_error(owlsl::text("Expected: '>'. Got: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
		return false;
	}

	// Register specialized type.
	sco.vm.table[original_type].pspecialize(&sco, new_type, tparams);

	// Replace templated type by the new specialized type in the source code
	tokens.erase(tokens.begin()+from, tokens.begin()+t);
	tokens[from].text = new_type;
	tokens[from].type = tok.type.get("symbol");

	t = from; // Go back to the location of the type
	// Is a symbol
	if (is_symbol(&tokens[t+1]) && !is_class_property)
	{
		// Is a variable declaration
		if (!parse_declaration(t)) return false;
	}

	return true;
}

bool cwl::parse_declaration	(unsigned& t)
{
	bool is_pcall = is_ptype(&tokens[t]);
	owlsl::text type_name = tokens[t].text;
	owlsl::text symbol_name = tokens[++t].text;

	set_expression_type(type_name); //save the type

	// Check variable isn't already declared or that the name isn't a type.
	if (1==pass)
	{
		if (is_type(symbol_name))
		{
			set_last_error(owlsl::text("'") + symbol_name + owlsl::text("' is a type and can't be used to name a variable."), tokens[t].column, tokens[t].line);
			return false;
		}

		_symbol* sym = NULL;
		bool use_this;
		if (find_context_var(symbol_name, sym, use_this))
		{
			set_last_error(owlsl::text("Variable '") + symbol_name + owlsl::text("' already declared."), tokens[t].column, tokens[t].line);
			return false;
		}
	}

	owlsl::text func_params;
	bool has_params = is_open_parent(&tokens[t+1]);
	if (has_params)
	{
		if (!parse_func_params(++t)) return false;

		if (expression_depth.size()==0)
		{
			if (!is_end_of_statement(&tokens[++t]))
			{
				set_last_error(owlsl::text("Expected: end of statement got: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
				return false;
			}
		}

		// parse parameters
		owlsl::text param_types;
		for (unsigned i=0; i<current_param_types.back().size(); i++)
		{
			if (param_types.length()>0)
			{
				param_types += ", ";
			}
			param_types += current_param_types.back()[i];
		}
		current_param_types.pop_back();

		// construct parameters string
		func_params = owlsl::text("(") + param_types + ")";
	}

	if (is_pcall)
	{
		if (has_params)
		{
			owlsl::text complete_func_name = type_name + func_params;

			if (!sco.vm.table[type_name].pconstruct.contains(complete_func_name))
			{
				set_last_error(owlsl::text("Undefined constructor: '") + complete_func_name + owlsl::text("'"), tokens[t].column, tokens[t].line);
				return false;
			}

			write_assembly( indent("palloc",8,4) + symbol_name + ", " + type_name + ", " + complete_func_name + owlsl::endl );
		}
		else
		{
			write_assembly( indent("palloc",8,4) + symbol_name + ", " + type_name + ", " + sco.vm.table[type_name].pconstruct.name(0) + owlsl::endl );
		}
	}
	else
	{
		write_assembly( indent("salloc",8,4) + symbol_name + ", " + type_name + ", " + type_name + "()" + owlsl::endl  ); // default constructor. For now...
	}

	if (!has_params) ++t;

	add_context_var(symbol_name, type_name);

	if (is_end_of_statement(&tokens[t]))
	{
		return true;
	}
	else if (is_assignment(&tokens[t]))
	{
		if (!parse_assignment(--t)) return false;

		if (!is_end_of_statement(&tokens[t]))
		{
			if (!is_end_of_statement(&tokens[t+1]))
			{
				set_last_error(owlsl::text("Expected: end of statement got: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
				return false;
			}
			else
			{
				++t;
			}
		}
	}
	else if (is_operator(&tokens[t]))
	{
		if (!parse_expression(--t, true)) return false;
	}

	return true;
}

bool cwl::parse_expression	(unsigned& t, const bool& is_func_param)
{
	// Increment expression depth so we know we are parsing an expression elsewhere
	expression_depth.push_back(current_expression_type);

	if (!parse_terms(t))
	{
		if (expression_depth.size()>0) expression_depth.pop_back();
		return false;
	}

	tokenizer::token* token = &tokens[t];

	owlsl::text left_type = expression_depth.back();
	while (true)
	{
		if (is_equal(token))
		{
			if (!parse_terms(++t)) return false;
			//expression_depth.push_back(current_expression_type);
			if (!write_method_call(token, left_type, "==", expression_depth.back())) return false;
			//expression_depth.pop_back();
			current_expression_type = "bool";
		}
		else if (is_not_equal(token))
		{
			if (!parse_terms(++t)) return false;
			//expression_depth.push_back(current_expression_type);
			if (!write_method_call(token, left_type, "!=", expression_depth.back())) return false;
			//expression_depth.pop_back();
			current_expression_type = "bool";
		}
		else if (is_less(token))
		{
			if (!parse_terms(++t)) return false;
			//expression_depth.push_back(current_expression_type);
			if (!write_method_call(token, left_type, "<", expression_depth.back())) return false;
			//expression_depth.pop_back();
			current_expression_type = "bool";
		}
		else if (is_less_equal(token))
		{
			if (!parse_terms(++t)) return false;
			//expression_depth.push_back(current_expression_type);
			if (!write_method_call(token, left_type, "<=", expression_depth.back())) return false;
			//expression_depth.pop_back();
			current_expression_type = "bool";
		}
		else if (is_greater(token))
		{
			if (!parse_terms(++t)) return false;
			//expression_depth.push_back(current_expression_type);
			if (!write_method_call(token, left_type, ">", expression_depth.back())) return false;
			//expression_depth.pop_back();
			current_expression_type = "bool";
		}
		else if (is_greater_equal(token))
		{
			if (!parse_terms(++t)) return false;
			//expression_depth.push_back(current_expression_type);
			if (!write_method_call(token, left_type, ">=", expression_depth.back())) return false;
			//expression_depth.pop_back();
			current_expression_type = "bool";
		}
		else if (is_logical_and(token))
		{
			if (!parse_terms(++t)) return false;
			//expression_depth.push_back(current_expression_type);
			if (!write_method_call(token, left_type, "&&", expression_depth.back())) return false;
			//expression_depth.pop_back();
			current_expression_type = "bool"; // set_expression_type("bool"); //?
		}
		else if (is_logical_or(token))
		{
			if (!parse_terms(++t)) return false;
			//expression_depth.push_back(current_expression_type);
			if (!write_method_call(token, left_type, "||", expression_depth.back())) return false;
			//expression_depth.pop_back();
			current_expression_type = "bool";
		}
		else
		{
			if (is_end_of_statement(&tokens[t]))
			{
				--t;
				//if (!is_func_param && expression_depth.size()>0) expression_depth.pop_back();
				return true;
			}

			if (is_func_param && is_close_parent(&tokens[t])) break;

			if (!is_end_of_statement(&tokens[t+1]))
			{
				// check for subcall
				if (factor_is_constant)
				{
					if (tokens[t].text.find(".") == 0)
					{
						if (!parse_func_call(t, left_type)) return false;
						current_expression_type = expression_depth.back();
						return true;
					}
					else if (tokens[t+1].text.find(".") == 0)
					{
						parse_func_call(++t, left_type);
						current_expression_type = expression_depth.back();
						return true;
					}
				}

				if (!is_comma(&tokens[t]) && !is_close_parent(&tokens[t]))
				{
					--t;
				}
			}
			break;
		}

		if (is_end_of_statement(&tokens[t]))
		{
			--t;
			expression_depth.pop_back();
			return true;
		}

		token = &tokens[++t];
	}

	/*if (is_func_param && expression_depth.size()>0)
	{
		current_expression_type = expression_depth.back();
	}*/
	//if (expression_depth.size()>0) expression_depth.pop_back();
	return true;
}

bool cwl::parse_terms (unsigned& t)
{
	if (!parse_term(t))
	{
		if (expression_depth.size()>0) expression_depth.pop_back();
		return false;
	}

	if (is_boolean_operator(&tokens[t]))
	{
		return true;
	}

	tokenizer::token* token = &tokens[++t];

	owlsl::text left_type = expression_depth.back();
	while (true)
	{
		if (is_addition(token))
		{
			if (is_addition(&tokens[t-1]))
			{
				set_last_error(owlsl::text("Post increment not implemented. Use pre increment."), tokens[t].column, tokens[t].line);
				return false;
			}
			if (!parse_term(++t)) return false;
			if (!write_method_call(token, left_type, "+", expression_depth.back())) return false;
			current_expression_type = left_type;
		}
		else if (is_addition_set(token))
		{
			if (factor_is_constant)
			{
				set_last_error(owlsl::text("Cannot assign to a constant or return value."), tokens[t].column, tokens[t].line);
				return false;
			}

			if (!parse_term(++t)) return false;
			if (!write_method_call(token, left_type, "+=", expression_depth.back())) return false;
			current_expression_type = left_type;
		}
		else if (is_substraction(token))
		{
			if (is_substraction(&tokens[t-1]))
			{
				set_last_error(owlsl::text("Post decrement not implemented. Use pre decrement."), tokens[t].column, tokens[t].line);
				return false;
			}
			if (!parse_term(++t)) return false;
			if (!write_method_call(token, left_type, "-", expression_depth.back())) return false;
			current_expression_type = left_type;
		}
		else if (is_substraction_set(token))
		{
			if (factor_is_constant)
			{
				set_last_error(owlsl::text("Cannot assign to a constant or return value."), tokens[t].column, tokens[t].line);
				return false;
			}

			if (!parse_term(++t)) return false;
			if (!write_method_call(token, left_type, "-=", expression_depth.back())) return false;
			current_expression_type = left_type;
		}
		else if (is_close_parent(token)) // was a function call
		{
			current_expression_type = left_type;
			break;
		}
		else
		{
			if (!parsing_bool_condition && current_expression_type !="bool")
			{
				current_expression_type = left_type;
			}
			break;
		}

		if (is_end_of_statement(&tokens[t]))
		{
			--t;
			return true;
		}

		if (is_operator(&tokens[t]))
		{
			token = &tokens[t];
			continue;
		}

		token = &tokens[++t];

		if (is_close_parent(token)) break;
	}

	return true;
}

bool cwl::parse_term (unsigned& t)
{
	if (!parse_factor(t)) return false;

	tokenizer::token* token = &tokens[t];
	if (!is_end_of_statement(token)) token = &tokens[++t];

	owlsl::text left_type = current_expression_type;
	while (true)
	{
		if (is_multiplication(token))
		{
			current_expression_type = expression_depth.back();
			if (!parse_factor(++t)) return false;
			if (!write_method_call(token, current_expression_type, "*", expression_depth.back())) return false;
			current_expression_type = left_type;
		}
		else if (is_multiplication_set(token))
		{
			if (factor_is_constant)
			{
				set_last_error(owlsl::text("Cannot assign to a constant or return value."), tokens[t].column, tokens[t].line);
				return false;
			}

			current_expression_type = expression_depth.back();
			if (!parse_factor(++t)) return false;
			if (!write_method_call(token, current_expression_type, "*=", expression_depth.back())) return false;
			current_expression_type = left_type;
		}
		else if (is_division(token))
		{
			current_expression_type = expression_depth.back();
			if (!parse_factor(++t)) return false;
			if (!write_method_call(token, current_expression_type, "/", expression_depth.back())) return false;
			current_expression_type = left_type;
		}
		else if (is_division_set(token))
		{
			if (factor_is_constant)
			{
				set_last_error(owlsl::text("Cannot assign to a constant or return value."), tokens[t].column, tokens[t].line);
				return false;
			}

			current_expression_type = expression_depth.back();
			if (!parse_factor(++t)) return false;
			if (!write_method_call(token, current_expression_type, "/=", expression_depth.back())) return false;
			current_expression_type = left_type;
		}
		else if (is_assignment(token))
		{
			if (!parse_assignment(--t)) return false;
		}
		else
		{
			if (!is_operator(&tokens[t+1]) || is_close_parent(&tokens[t]) || is_end_of_statement(&tokens[t]) ) // if
			{
				--t;
			}
			break;
		}
		token = &tokens[++t];
	}

	return true;
}

bool cwl::parse_factor (unsigned& t)
{
	factor_is_constant = false;

	tokenizer::token* token = &tokens[t];

	bool is_literal = false;

	bool unary_not = false;
	if (is_not(token))
	{
		unary_not = true;
		token = &tokens[++t]; // advance token
	}

	bool unary_dec = false;
	if (is_substraction(token))
	{
		if (is_substraction(&tokens[t+1]))
		{
			t+=2;
			token = &tokens[t]; // advance token
			unary_dec = true;
		}
	}

	bool unary_neg = false;
	if (is_substraction(token) && !is_substraction(&tokens[t+1]))
	{
		unary_neg = true;
		token = &tokens[++t]; // advance token
	}

	bool unary_inc = false;
	if (is_addition(token))
	{
		if (is_addition(&tokens[t+1]))
		{
			t+=2;
			token = &tokens[t]; // advance token
			unary_inc = true;
		}
	}

	bool unary_add = false;
	if (is_addition(token) && !is_addition(&tokens[t+1]))
	{
		unary_add = true;
		token = &tokens[++t]; // advance token
	}

	if (is_operator(token))
	{
		if (is_multiplication(token))
		{
			if (is_division(&tokens[t+1]))
			{
				set_last_error(owlsl::text("Syntax error: '/'."), tokens[t+1].column, tokens[t+1].line);
				return false;
			}
		}

		if (!parse_factor(t)) return false;
	}
	else if (is_open_parent(token))
	{
		unsigned last_t = t+1;
		if (!parse_expression(++t)) return false;
		if (last_t == t) ++t;
		if (!is_close_parent(&tokens[t]))
		{
			set_last_error(owlsl::text("Expected: ')' got: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
			return false;
		}
		factor_is_constant = true;
	}
	else if (is_number(token))
	{
		is_literal = true;
		factor_is_constant = true;
		write_assembly( indent("push",8,4) + token->text + owlsl::endl );
		set_expression_type("number");
	}
	else if (is_string(token))
	{
		is_literal = true;
		factor_is_constant = true;
		write_assembly( indent("push",8,4) + "\"" + token->text + "\"" + owlsl::endl );
		set_expression_type("text");
	}
	else if (is_boolean(token))
	{
		is_literal = true;
		factor_is_constant = true;
		write_assembly( indent("push",8,4) + token->text + owlsl::endl );
		set_expression_type("bool");
	}
	else if (is_symbol(token))
	{
		// check if is reference access operator '->'
		if ( is_substraction(&tokens[t+1]) && is_greater(&tokens[t+2]) )
		{
			tokens[t].text += owlsl::text(".ref");
			tokens[t+1].text = "(";
			tokens[t+1].type = tok.type.get("parentheses_open");
			tokens[t+2].text = ")";
			tokens[t+2].type = tok.type.get("parentheses_close");
			tokens[t+3].text = owlsl::text(".") + tokens[t+3].text;
			return parse_statement(t, false); // return and re-parse
		}

		bool is_temp_var = false;

		// Followed by a parentheses
		if (!is_type(token->text) && is_open_parent(&tokens[t+1]))
		{
			// Is a function call
			if (!parse_func_call(t)) return false;
			--t;
			factor_is_constant = true;
		}
		else if (is_this(&tokens[t]))
		{
			write_assembly( indent("push",8,4) + "this" + owlsl::endl );
			set_expression_type(current_class_name);
		}
		else
		{
			owlsl::text symbol_name = token->text;
			if (symbol_name.contains("."))
			{
				set_last_error(owlsl::text("Property '") + symbol_name + owlsl::text("' is private."), tokens[t].column, tokens[t].line);
				return false;
			}

			_symbol* sym = NULL;
			bool use_this = false;
			if (find_context_var(token->text, sym, use_this))
			{
				if (use_this) symbol_name = owlsl::text("this.") + symbol_name;
			}
			else if (is_type(token->text)) // temporary instance
			{
				is_temp_var = true;

				// Specialize templated types, if present
				if (pass==1)
				{
					// is templated?
					unsigned var_name_pos = t+1;
					if ( is_less(&tokens[var_name_pos]) )
					{
						size_t open_lesses = 0;
						size_t close_lesses = 0;
						// Find the end of the templated type
						while(!at_end_of_file(var_name_pos))
						{
							if ( is_less(&tokens[var_name_pos]) ) open_lesses++;
							if ( is_greater(&tokens[var_name_pos]) )
							{
								close_lesses++;
								if (open_lesses == close_lesses)
								{
									var_name_pos++;
									break;
								}
							}

							var_name_pos++;
						}
						if (at_end_of_file(var_name_pos))
						{
							set_last_error(owlsl::text("Type template paramenter incomplete. Expected '>'"), tokens[t+1].column, tokens[t+1].line);
							return false;
						}
					}

					if (!is_open_parent(&tokens[var_name_pos]))
					{
						set_last_error(owlsl::text("Expected '('"), tokens[var_name_pos].column, tokens[var_name_pos].line);
						return false;
					}

					// find end of constructor
					size_t eol_pos = var_name_pos;
					{
						size_t open_pars = 0;
						size_t close_pars = 0;
						// Find the end of the templated type
						while(!at_end_of_file(eol_pos))
						{
							if ( is_open_parent(&tokens[eol_pos]) ) open_pars++;
							if ( is_close_parent(&tokens[eol_pos]) )
							{
								close_pars++;
								if (open_pars == close_pars)
								{
									eol_pos++;
									break;
								}
							}

							eol_pos++;
						}
						if (at_end_of_file(var_name_pos))
						{
							set_last_error(owlsl::text("Expected ')'"), tokens[var_name_pos].column, tokens[var_name_pos].line);
							return false;
						}
					}

					// Insert a temporary variable name as a token;
					symbol_name = owlsl::text("@@OWL_TEMP_SYMBOL") + owlsl::text((int32_t)++temp_var_count) + "@@" ;
					owlsl::tokenizer::token tmp_token;
					tmp_token.text = symbol_name;
					tmp_token.type = tok.type.get("symbol");
					tmp_token.line = tokens[var_name_pos].line;
					tmp_token.column = tokens[var_name_pos].column+1;
					tokens.insert( tokens.begin()+(var_name_pos), tmp_token);
					tmp_token.text = ";";
					tmp_token.type = tok.type.get(";");
					tmp_token.line = tokens[var_name_pos].line;
					tmp_token.column = tokens[var_name_pos].column+1;
					tokens.insert( tokens.begin()+(eol_pos+1), tmp_token);
				}
				else
				{
					symbol_name = tokens[t+1].text;
				}

				// Is a symbol
				if (is_symbol(&tokens[t+1]))
				{
					// Is a variable declaration
					if (!parse_declaration(t)) return false;
				}
				// Followed by a '<' (template parameters)
				else if (is_less(&tokens[t+1]))
				{
					if (!parse_templated_var(t)) return false;
					use_this = false;
				}
				t+=2; //ignore ';'
				find_context_var(symbol_name, sym, use_this);
			}
			else
			{
				set_last_error(owlsl::text("Undefined symbol: '") + symbol_name + owlsl::text("'"), token->column, token->line);
				return false;
			}

			if (!is_temp_var) // Don't push temporaries because palloc already pushes it
			{
				write_assembly( indent("push",8,4) + symbol_name + owlsl::endl );
			}

			set_expression_type(sym->type);
		}
	}
	else
	{
		set_last_error(owlsl::text("Unexpected: '") + tokens[t].text + owlsl::text("'"), tokens[t].column, tokens[t].line);
		return false;
	}

	if (unary_not)
	{
		if (!write_method_call(token, expression_depth.back(), "!", "")) return false;
	}

	if (unary_neg)
	{
		if (!write_method_call(token, expression_depth.back(), "-", "")) return false;
	}

	if (unary_add)
	{
		if (!write_method_call(token, expression_depth.back(), "+", "")) return false;
	}

	if (unary_inc)
	{
		if (is_literal)
		{
			if (!write_method_call(token, expression_depth.back(), "inc_literal", "")) return false;
		}
		else
		{
			if (!write_method_call(token, expression_depth.back(), "++", "")) return false;
		}
	}

	if (unary_dec)
	{
		if (is_literal)
		{
			if (!write_method_call(token, expression_depth.back(), "dec_literal", "")) return false;
		}
		else
		{
			if (!write_method_call(token, expression_depth.back(), "--", "")) return false;
		}
	}
	return true;
}


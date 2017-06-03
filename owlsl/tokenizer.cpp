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

#include "tokenizer.h"

using namespace owlsl;

tokenizer::tokenizer() 
{
	type.add(" end_of_file"			, "end of file"			, token::end_of_file);
	type.add(" symbol"				, "symbol"				, token::symbol);
	type.add(" number_literal"		, "number literal"		, token::number_literal);
	type.add(" boolean_literal"		, "boolean literal"		, token::boolean_literal);
}

tokenizer::~tokenizer()
{

}

bool tokenizer::is_block(const owlsl::text& str, unsigned& column, owlsl::wfile::wchars& wc)
{
	unsigned keyword_size = str.length();
	if (keyword_size==0) return false;

	std::wstring tmp;
	unsigned len = (column+(keyword_size-1));
	if (len<wc.size())
	{
		for (unsigned k=column; k<=len; k++)
		{
			tmp += wc[k];
		}
	}
	if (str == tmp)
	{
		// Parse string literal
		column=len+1;
		return true;
	}

	return false;
}

bool tokenizer::parse(owlsl::wfile& wf, tokens& tkns)
{
	token			current_token;
	std::wstring	current_char;

	// Iterate all text lines
	owlsl::wfile::wcharlines& wl = wf.lines();
	for(unsigned line=0; line<wl.size(); line++)
	{
		// Iterate all characters
		owlsl::wfile::wchars& wc = wl[line];
		for(unsigned column=0; column<wc.size(); column++)
		{
			// Get current character
			current_char = std::wstring(1, wc[column]);

			// If is separator 
			if (separator.exists(current_char))
			{
				// check if it is a string block
				if (is_block(blocks.string.start(), column, wc))
				{
					if (!parse_string_literal(current_token, wf, column, line)) return false; // Error parsing string literal
					// Add it
					tkns.push_back(current_token);
					current_token.text.clear();
					current_token.type = NULL;
					continue;
				}

				// check if it is a line comment block
				if (is_block(this->comment.m_comment, column, wc))
				{
					if (!parse_line_comment(current_token, wf, column, line)) return false; // Error parsing line comment
					// Add it
					tkns.push_back(current_token);
					current_token.text.clear();
					current_token.type = NULL;
					continue;
				}

				// check if it is a block comment
				if (is_block(blocks.comment.start(), column, wc))
				{
					if (!parse_block_comment(current_token, wf, column, line)) return false; // Error parsing block comment
					// Add it
					tkns.push_back(current_token);
					current_token.text.clear();
					current_token.type = NULL;
					wc = wl[line]; // set the correct line after parsing the block comment
					continue;
				}

				//if current token isn't empy
				if (current_token.text.length()>0)
				{
					// Add it
					current_token.type = determine_type(current_token.text);
					tkns.push_back(current_token);
					current_token.text.clear();
					current_token.type = NULL;
				}

				// Add separator If is not whitespace
				if (!separator.is_space(current_char))
				{
					if (!parse_separator(current_token, wf, column, line)) return false; // token type not defined.
					tkns.push_back(current_token);
					current_token.text.clear();
					current_token.type = NULL;
				}
				continue;
			}

			// If is not whitespace
			if (!separator.is_space(current_char))
			{
				if (current_token.text.length()==0)
				{
					current_token.column = column;
					current_token.line = line;
				}

				current_token.text += current_char;
			}
		}

		// New line is a separator
		if (current_token.text.length()>0)
		{
			current_token.type = determine_type(current_token.text);
			tkns.push_back(current_token);
			current_token.text.clear();
			current_token.type = NULL;
		}

		// Check if newline was added as a separator (maybe it is end_of_statement)
		if (separator.exists("\n"))
		{
			current_token.text = "\n";
			current_token.type = determine_type(current_token.text);
			current_token.column = wc.size()-1;
			current_token.line = line;
			tkns.push_back(current_token);
			current_token.text.clear();
			current_token.type = NULL;
		}
	}

	//append(token::end_of_file,tkns,0,wl.size());

	return true;	
}

bool tokenizer::parse_string_literal(token& tk, owlsl::wfile& wf, unsigned& column, unsigned& line)
{
	unsigned keyword_size = blocks.string.end().length();
	if (keyword_size == 0)
	{
		set_last_error("No token defined for end of string literal.", column, line);
		return false; // 
	}

	tk.column = column;
	tk.line = line;
	tk.type = type.get(blocks.string.start());

	std::wstring	tmp;
	std::wstring	current_char;
	owlsl::wfile::wchars& wc = wf.lines()[line];
	// read the rest of this line
	for (column=column; column<wc.size(); column++)
	{
		current_char = std::wstring(1, wc[column]);
		tmp = L"";
		unsigned len = (column+(keyword_size-1));
		if (len<wc.size())
		{
			for (unsigned k=column; k<=len; k++)
			{
				tmp += wc[k];
			}
		}
		if (blocks.string.end() == tmp)
		{
			column = len;
			return true;
		}

		tk.text += current_char;
	}
	
	set_last_error("Unexpected end of file while parsing string literal.", column, line);
	return false; 
}

bool tokenizer::parse_line_comment(token& tk, owlsl::wfile& wf, unsigned& column, unsigned& line)
{
	tk.column = column;
	tk.line = line;
	tk.type = type.get(comment.m_comment);

	owlsl::wfile::wchars& wc = wf.lines()[line];
	// read the rest of this line
	for (column=column; column<wc.size(); column++)
	{
		tk.text += std::wstring(1, wc[column]);
	}
	tk.text.trim();
	return true;
}

bool tokenizer::parse_block_comment(token& tk, owlsl::wfile& wf, unsigned& column, unsigned& line)
{
	bool trim_it = true;
	unsigned keyword_size = blocks.comment.end().length();
	if (keyword_size == 0)
	{
		set_last_error("No token defined for end of block comment.", column, line);
		return false; // no end of block defined
	}

	tk.column = column;
	tk.line = line;
	tk.type = type.get(blocks.comment.start());

	std::wstring	tmp;
	std::wstring	current_char;
	owlsl::wfile::wcharlines& wl = wf.lines();
	// read the rest of this line
	for (line=line; line<wl.size(); line++)
	{
		owlsl::wfile::wchars& wc = wl[line];
		for (column=column; column<wc.size(); column++)
		{
			current_char = std::wstring(1, wc[column]);
			tmp = L"";
			unsigned len = (column+(keyword_size-1));
			if (len<wc.size())
			{
				for (unsigned k=column; k<=len; k++)
				{
					tmp += wc[k];
				}
			}
			if (blocks.comment.end() == tmp)
			{
				column = len+1;
				if (trim_it) tk.text.trim();
				return true;
			}

			tk.text += current_char;
		}
		tk.text += L"\n";
		trim_it = false;  //Do not trim the text if it is multiline
		column=0;
	}

	set_last_error("Unexpected end of file while parsing block comment.", column, line);
	return false; 
}

bool tokenizer::parse_separator(token& tk, owlsl::wfile& wf, unsigned& column, unsigned& line)
{
	token_type* current_type = NULL;
	tk.column = column;
	tk.line = line;

	std::wstring	current_char;
	owlsl::wfile::wchars& wc = wf.lines()[line];
	// read the rest of this line
	current_char = std::wstring(1, wc[column]);
	
	current_type = type.get(current_char);
	while ( (token::symbol != current_type->id) && (token::string_literal != current_type->id) && (token::block_comment != current_type->id) && (token::comment != current_type->id) )
	{
		tk.type = current_type;
		tk.text += current_char;
		++column;
		if (column>=wc.size()) return true; // If EOL that's it
		current_char = std::wstring(1, wc[column]);
		current_type = type.get(tk.text + current_char);
		if (token::symbol == current_type->id)
		{
			--column;
			return true; // This character doesn't belong here. But we're done.
		}
	}

	if(tk.text.length()==0) //if ((token::symbol == current_type->id) && this->separator.exists(current_char))
	{
		set_last_error(L"Couldn't find a matching token type for '" + current_char + L"'.", column, line);
		return false;
	}

	--column;
	return true;
}

void tokenizer::append(const unsigned& id, tokens& tkns, const unsigned& column, const unsigned& line)
{
	token tk;
	tk.column = column;
	tk.line = line;
	tk.type = type.get(id);
	tkns.push_back(tk);
}

tokenizer::token_type* tokenizer::determine_type(const owlsl::text& str)
{
	if (str.is_number()) return type.get(token::number_literal);
	if (str.equals(owlsl::text("true"))) return type.get(token::boolean_literal);
	if (str.equals(owlsl::text("false"))) return type.get(token::boolean_literal);
	return type.get(str);
}
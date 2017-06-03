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
#include "document.h"

using namespace owlsl::xml;

document::document()
{
	m_error = false;
}

document::~document()
{

}

node* document::root()
{
	return &m_root;
}

owlsl::text& document::error_description()
{
	return m_error_description;
}

bool document::parse (owlsl::mfile& f)
{
	owlsl::wfile m_file;
	m_file.open(f);

    std::wstring current_char;

    for (size_t line=0; line<m_file.lines().size(); line++)
    {
        for (size_t col = 0; col<m_file.lines()[line].size(); col++)
        {
            current_char = std::wstring(1, m_file.lines()[line][col]);

            if (current_char == L"<")
            {
                parse_node(m_file, line, col, current_char, &m_root);
                continue;
            }

			if (m_error)
			{
				return false;
			}
        }
    }

	return true;
}

void document::parse_node(owlsl::wfile& m_file, size_t& line, size_t& col, std::wstring& current_char, node* parent)
{
	bool at_slash			= false;
	bool at_node_name		= false;
	bool at_prop_name		= false;
	bool at_prop_value		= false;
	bool at_inner			= false;
	bool at_end_tag			= false;
	bool at_expecting_quote = false;
	bool at_comment			= false;

	owlsl::text text;
	owlsl::text name;

	node* n = new node();

    for (; line<m_file.lines().size(); line++)
    {
        for (; col<m_file.lines()[line].size(); col++)
        {
            current_char = std::wstring(1, m_file.lines()[line][col]);

			if (at_comment)
			{
				if (current_char == L"-")
				{
					std::wstring next_char = std::wstring(1, m_file.lines()[line][col+1]);
					if (next_char == L"-")
					{
						col++;
						next_char = std::wstring(1, m_file.lines()[line][col+1]);
						if (next_char == L">")
						{
							col++;
							at_comment = false;

						}
					}
				}
				continue;
			}

            if (current_char == L"<")
            {
				if (at_inner)
				{
					if (col+1<m_file.lines()[line].size())
					{
						std::wstring next_char = std::wstring(1, m_file.lines()[line][col+1]);
						if (next_char == L"/")
						{
							if (text.length()>0)
							{
								owlsl::text inner_text = text;
								inner_text.trim();
								if (inner_text.length()==0) // Trim if only has spaces
								{
									n->inner_text(inner_text);
								}
								else
								{
									n->inner_text(text);
								}
								text.clear();
							}
							at_inner = false;
							continue;
						}

						if (next_char == L"!")
						{
							col++;
							next_char = std::wstring(1, m_file.lines()[line][col+1]);
							if (next_char == L"-")
							{
								col++;
								next_char = std::wstring(1, m_file.lines()[line][col+1]);
								if (next_char == L"-")
								{
									col++;
									at_comment = true;
									continue;
								}
							}
						}
					}

					parse_node(m_file, line, col, current_char, n);
				}
				else
				{
					at_node_name = true;
				}

                continue;
            }

			if ((current_char == L"/" || current_char == L"?") && !at_prop_value && !at_inner)
			{
				//if (at_inner || at_prop_value) break;

				if (at_prop_name)
				{
					at_prop_name = false;
					if (text.length()==0)
					{
						if (current_char == L"?")
						{
							parent->add_node(n);
						}
						else
						{
							if (col+1<m_file.lines()[line].size())
							{
								std::wstring next_char = std::wstring(1, m_file.lines()[line][col+1]);
								if (next_char == L">")
								{
									parent->add_node(n);
								}
							}
						}
						at_slash = true;
						at_end_tag = true;
					}
					else
					{
						m_error = true;
						set_error(m_file, "Unexpected slash", line, col);
						return;
					}
				}
				else if(at_node_name)
				{
					if (text.length()==0 && current_char == L"/")
					{
						at_node_name = false;
						at_end_tag = true;
					}
				}
				continue;
			}

			if (current_char == L">")
			{
				if (at_slash)
				{
					if (!at_end_tag)
					{
						parent->add_node(n);
					}
					return;
				}
				else
				{
					if (at_node_name)
					{
						if (text.length()>0)
						{
							at_node_name = false;
							n->name(text);
							text.clear();
							at_prop_name = true;
						}
					}
					parent->add_node(n);
					at_inner = true;
				}
				continue;
			}

			if (current_char == L" " && !at_inner && !at_prop_value)
			{
				if (at_node_name)
				{
					if (text.length()>0)
					{
						at_node_name = false;
						n->name(text);
						text.clear();
						at_prop_name = true;
						continue;
					}
					else
					{
						m_error = true;
						set_error(m_file, "Space found before node name", line, col);
						return;
					}
				}

				continue;
			}

			if (current_char == L"=" && at_prop_name && text.length()>0)
			{
				at_prop_name = false;
				name = text;
				text.clear();
				at_expecting_quote = true;

				continue;
			}

			if (current_char == L"\"")
			{
				if (at_expecting_quote)
				{
					at_expecting_quote = false;
					at_prop_value = true;
					continue;
				}

				if (at_prop_value)
				{
					at_prop_value = false;
					n->add_property(name, text);
					name.clear();
					text.clear();
					at_prop_name = true;
				}

				continue;
			}

			text += current_char;
        }

		col = 0; //end of line
    }
}

void document::set_error(owlsl::wfile&	m_file, owlsl::text text, const size_t& line, const size_t& col)
{
	m_error_description.set(owlsl::text("Error loading file [") + m_file.file_path() + "]: " + owlsl::text(text+". Line: ") + (uint32_t)line + owlsl::text(", Col: ") + (uint32_t)col);
}

owlsl::text get_xml(node* n, int32_t& level)
{
	owlsl::text text;

	bool has_name = n->name().length()>0;
	if (has_name)
	{
		text += owlsl::text("<") + n->name();
		text.fill_left(text.length()+(level*3), L" ");
	}

	n->first_property();
	while (n->found_property())
	{
		text += " ";
		text += n->property_name() + "=\"" + n->property_value() + "\"";
		n->next_property();
	}

	n->first_node();
	if (n->found())
	{
		if (has_name)
		{
			if (n->inner_text().length()>0)
			{
				text += owlsl::text(">") + n->inner_text();
			}
			else
			{
				text += owlsl::text(">\n");
			}
		}

		while(n->found())
		{
			level++;
			text += get_xml(n->current_node(), level);
			level--;
			n->next_node();
		}

		if (has_name)
		{
			owlsl::text end_tag(owlsl::text("</") + n->name() + ">\n");
			text += end_tag.fill_left(end_tag.length()+(level*3), L" ");
		}
	}
	else
	{
		if (has_name)
		{
			if (n->inner_text().length()>0)
			{
				text += owlsl::text(">") + n->inner_text() + owlsl::text("</") + n->name() + ">\n";
			}
			else
			{
				text += owlsl::text("/>\n");
			}
		}
	}
	return text;
}

owlsl::text document::xml()
{
	int32_t level = 0;
	owlsl::text text = get_xml(&m_root, level);
	return text;
}

bool document::write (owlsl::mfile& f)
{
	owlsl::text text;
	text += "<?xml version=\"1.0\" encoding=\"utf-16\"?>";
	text += L"\n";

	write_nodes(root(), text);

	// write little endian code
	std::string header = "  ";
	header[0] = (unsigned char)0xFF;
	header[1] = (unsigned char)0xFE;
	f.append(header);

	// write xml
	return f.append(text.wstring());
}

bool document::write_nodes (node* parent, owlsl::text& text)
{
	bool has_children = false;

	for(parent->first_node(); parent->found(); parent->next_node())
	{
		write_node(parent->current_node(), text);
		has_children = true;
	}

	return has_children;
}


void document::write_node (node* node, owlsl::text& text)
{
	if (node->name().to_lower() == owlsl::text("xml"))
		return;

	text += L"<" + node->name().wstring();

	// Write properties
	for(node->first_property(); node->found_property(); node->next_property())
	{
		text += L" " + node->property_name().wstring() + L"=\"" + node->property_value().wstring() + L"\"";
	}

	node->first_node();
	if (node->found())
	{
		text += L">\n";
		write_nodes(node, text);
		text += L"</" + node->name().wstring() + L">\n";
	}
	else if (node->inner_text().length())
	{
		text += L">\n";
		text += node->inner_text();
		text += L"</" + node->name().wstring() + L">\n";
	}
	else
	{
		text += L"/>\n";
	}
}

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
#include "node.h"

using namespace owlsl::xml;

node::node() 
{

}

node::~node()
{
	for (std::multimap<std::wstring, node*>::iterator item = m_child_nodes.begin(); item != m_child_nodes.end(); item++)
	{
		delete (*item).second;
	}
}

owlsl::text& node::name ()
{
	return m_name;
}

owlsl::text& node::inner_text ()
{
	return m_inner_text;
}

void node::name (const owlsl::text& val)
{
	m_name = val;
}

void node::inner_text (const owlsl::text& val)
{
	m_inner_text = val;
}

void node::add_property (const owlsl::text& name, const owlsl::text& value)
{
	m_properties.insert(name.wstring(), value, true);
}

void node::add_node (node* _node)
{
	m_current_node = m_child_nodes.insert(std::make_pair(_node->name().wstring(),_node));
}

bool node::find_property (const owlsl::text& name)
{
	return m_properties.find(name.wstring());
}

bool node::next_property ()
{
	return m_properties.move_next();
}

owlsl::text& node::property_value ()
{
	return m_properties.value();
}

owlsl::text node::property_name ()
{
	return owlsl::text(m_properties.key());
}

bool node::find_node (const owlsl::text& name)
{
    m_results       = m_child_nodes.equal_range(name.wstring());
    m_current_node  = m_results.first;
    return (m_results.first != m_child_nodes.end()) && (found()) && (name.wstring()==(*m_results.first).first);
}

bool node::found()
{
    return m_current_node != m_results.second;
}

bool node::found_property ()
{
	return !m_properties.eof();
}

bool node::next_sibling_node ()
{
    m_current_node++;
    return found();
}

node* node::current_node ()
{
	if (!found()) return NULL;
	return (*m_current_node).second;
}

void node::first_node ()
{
	m_current_node = m_child_nodes.begin();
	m_results.first = m_current_node;
	m_results.second = m_child_nodes.end();
}

bool node::next_node ()
{
	m_current_node++;
	return found();
}

void node::first_property ()
{
	m_properties.move_first();
}

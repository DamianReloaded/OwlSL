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

#include "mfile.h"
#include <iostream>
#include <iterator>
#include <sstream>
#include <fstream>
#include <string.h>

using namespace owlsl;

typedef std::istream_iterator<uint8_t> istream_iterator;
typedef std::ostream_iterator<uint8_t> ostream_iterator;

mfile::mfile() : m_pos(0)
{
	m_is_open = false;
}

mfile::~mfile()
{
	
}

bool mfile::is_open()
{
	return m_is_open;
}

void mfile::set_as_open()
{
	m_is_open = true;
}

bool mfile::open(const owlsl::text& file_path)
{
	std::ifstream file(file_path.string().c_str(), std::ios_base::binary);
    if (!file.is_open())
    {
        //m_last_error = L"Error opening file: " + file_path.wstring();
        return false;
    }

	std::streampos sfsize = file.tellg();
	file.seekg( 0, std::ios::end );
    sfsize= file.tellg() - sfsize;
	file.seekg( 0, std::ios::beg );

    file >> std::noskipws; // makes the stream read withespaces
	std::copy(std::istream_iterator<uint8_t>(file), std::istream_iterator<uint8_t>(), std::back_inserter(m_buffer));
    file.close();

	size_t fsize = sfsize;
    if (m_buffer.size() != fsize)
    {
        return false;
    }

    m_file_path = file_path;
	m_is_open = true;
    return true;
}

bool mfile::save(const owlsl::text& file_path)
{
    std::ofstream file(file_path.string().c_str(), std::ios_base::binary);
    if (!file.is_open())
    {
        //m_last_error = L"Error creating file: " + file_path.wstring();
        return false;
    }

    std::copy(m_buffer.begin(), m_buffer.end(), std::ostream_iterator<uint8_t>(file, ""));
    file.close();

/*	size_t fsize = file.size();
    if (m_buffer.size() != fsize)
    {
        std::wstringstream ss;
        ss << L"Number of bytes written differ from buffer size. File: " << fsize << L" bytes, Buffer:" << m_buffer.size() << L" bytes.";
        m_last_error = ss.str();
        return false;
    }
    m_last_error.set(L"");*/
    return true;
}

void mfile::seek(size_t pos)
{
    m_pos = pos;
}

size_t mfile::read(uint8_t* buff, size_t amount)
{
    std::copy(m_buffer.begin()+m_pos, m_buffer.begin()+(m_pos+(amount)), buff);
    m_pos += amount;
    return amount;
}

template <class T>
bool append_t (std::vector<uint8_t>& buffer, const T& data)
{
	buffer.insert(buffer.end(), data.begin(), data.end());

	return true;
}

bool mfile::append (const std::string& data)
{
	return append_t<std::string>(m_buffer, data);
}

bool mfile::append (const std::wstring& data)
{
// Get a pointer to the data:
    const wchar_t* wstr_data = data.data();

    // Cast to an unsigned char*:
    const uint8_t* uchar_data = reinterpret_cast<const uint8_t*>(wstr_data);

    // Determine size of data:
    unsigned int data_size = data.size() * sizeof(wchar_t);

    // Add to vector:
	m_buffer.insert(m_buffer.end(), uchar_data, uchar_data + data_size);

	return true;
}

bool mfile::append (const std::vector<uint8_t>& data)
{
    return append_t<std::vector<uint8_t> >(m_buffer, data);
}

bool mfile::append (const uint8_t* buffer, const size_t& size)
{
    m_buffer.insert(m_buffer.end(), &buffer[0], &buffer[size]);

    return true;
}

void mfile::append (const owlsl::text& text, const encoding::id& encoding)
{
	text.to_array(m_buffer, encoding);
}

void mfile::append (const encoding::id& encoding)
{
	owlsl::text::get_mark(m_buffer, encoding);
}

void mfile::close()
{
	m_is_open = false;
	std::vector<uint8_t>().swap(m_buffer);
}

std::vector<uint8_t>& mfile::buffer()
{
    return m_buffer;
}

const uint32_t mfile::size()
{
    return m_buffer.size();
}

void mfile::resize(uint32_t nsize)
{
    m_buffer.resize(nsize);
}

const owlsl::text& mfile::last_error()
{
    return m_last_error;
}

const owlsl::text& mfile::file_path()
{
    return m_file_path;
}

void mfile::set_file_path(const owlsl::text& val)
{
    m_file_path = val;
}

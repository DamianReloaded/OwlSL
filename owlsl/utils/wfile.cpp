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
#include "wfile.h"

using namespace owlsl;

wfile::wfile()
{
    m_is_unicode = false;
	m_file = NULL;
	m_file_owned = false;
}

wfile::~wfile()
{
	close();
}

bool wfile::open(const owlsl::text& file_path)
{
	m_file = new mfile();
	m_file_owned = true;
    if (!m_file->open(file_path)) return false;
    get_lines();
    m_file->close();
    return true;
}

bool wfile::open(const uint8_t* buffer, const size_t& size)
{
	m_file = new mfile();
	m_file_owned = true;
    m_file->append(buffer, size);
    get_lines();
    m_file->close();
    return true;
}

bool wfile::open (owlsl::mfile& f)
{
	m_file = &f;
	m_file_owned = false;
    get_lines();
    return true;
}

void wfile::get_lines()
{
    if (m_file->size() > 1)
    {
        if (is_unicode(&m_file->buffer()[0]))
        {
            read_unicode();
            return;
        }
    }
    read_ascii();
}

void wfile::close()
{
    m_file->close();
	if(m_file_owned) delete m_file;
    m_is_unicode = false;
    m_lines.clear();
}

const owlsl::text& wfile::file_path()
{
    return m_file->file_path();
}

bool wfile::is_unicode(const uint8_t* buffer)
{
    m_is_unicode = ( (0xFF==(uint8_t)buffer[0] && 0xFE==(uint8_t)buffer[1]) || (0xFE==(uint8_t)buffer[0] && 0xFF==(uint8_t)buffer[1]) );
    return m_is_unicode;
}

bool wfile::is_unicode()
{
    return m_is_unicode;
}

bool wfile::is_little_endian(const uint8_t* buffer)
{
    return (0xFF==(uint8_t)buffer[0] && 0xFE==(uint8_t)buffer[1]);
}

void wfile::read_unicode()
{
    if (is_little_endian(&m_file->buffer()[0]))
    {
        read_unicode_little_endian();
    }
    else
    {
        read_unicode_big_endian();
    }
}

void wfile::read_unicode_little_endian()
{
    const uint8_t* buffer = &m_file->buffer()[0];
    const size_t size = m_file->size();
    size_t new_size = (size-2)/2;
    if (new_size<1)
    {
        return;
    }
    m_lines.resize(1);

    size_t line = 0;
    size_t advance = 0;
    for (size_t i=2; i<size; i+=2)
    {
        while ( (advance = is_eol_unicode(buffer, i)) )
        {
            if ((i+advance<size))
            {
                i += advance;
                line++;
                m_lines.resize(line+1);
            }
            else
            {
                return;
            }
        }

        m_lines[line].push_back( (wchar_t)(*(uint16_t*)(&buffer[i])) );
    }
}

void wfile::read_unicode_big_endian()
{


}

void wfile::read_ascii()
{
    const uint8_t* buffer = &m_file->buffer()[0];
    const size_t size = m_file->size();

    if (size<1)
    {
        return;
    }
    m_lines.resize(1);

    size_t line = 0;
    size_t advance = 0;
    for (size_t i=0; i<size; i++)
    {
        while ( (advance = is_eol_ascii(buffer, i)) )
        {
            if (i+advance<size)
            {
                i += advance;
                line++;
                m_lines.resize(line+1);
            }
            else
            {
                return;
            }
        }

        m_lines[line].push_back((wchar_t)buffer[i]);
    }
}

size_t wfile::is_eol_unicode(const uint8_t* buffer, const size_t& pos)
{
    if ( (0x0D==(uint8_t)buffer[pos]) && (0x00==(uint8_t)buffer[pos+1]) )
    {
        if ( (0x0A==(uint8_t)buffer[pos+2]) && (0x00==(uint8_t)buffer[pos+3]) )
        {
            return 4;
        }

        return 2;
    }

    if ( (0x0A==(uint8_t)buffer[pos]) && (0x00==(uint8_t)buffer[pos+1]) )
    {
        if ( (0x0D==(uint8_t)buffer[pos+2]) && (0x00==(uint8_t)buffer[pos+3]) )
        {
            return 4;
        }

        return 2;
    }

    return 0;
}

size_t wfile::is_eol_ascii(const uint8_t* buffer, const size_t& pos)
{
    if (0x0D==(uint8_t)buffer[pos])
    {
        if (0x0A==(uint8_t)buffer[pos+1])
        {
            return 2;
        }

        return 1;
    }

    if (0x0A==(uint8_t)buffer[pos])
    {
        if (0x0D==(uint8_t)buffer[pos+1])
        {
            return 2;
        }

        return 1;
    }

    return 0;
}

wfile::wcharlines& wfile::lines()
{
	return m_lines;
}
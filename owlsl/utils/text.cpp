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

#include "text.h"
#include <vector>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <sstream>
#include <iomanip>
#include <wctype.h>
#include <cstdlib>
#include <iterator>

using namespace owlsl;

text::text()
{

}

text::~text()
{

}

text::text(const char* val)
{
	set(val);
}

text::text(const wchar_t* val)
{
	set(val);
}

text::text(const std::string& val)
{
	set(val);
}

text::text(const std::wstring& val)
{
	set(val);
}

text::text(const int32_t& val)
{
	set(val);
}

text::text(const uint32_t& val)
{
	set(val);
}

text::text(const float& val)
{
	set(val);
}

void text::set(const char* val)
{
	m_string = text::to_wstring(std::string(val));
}

void text::set(const wchar_t* val)
{
	m_string = val;
}

void owlsl::text::set(const owlsl::text& val)
{
	m_string = val.m_string;
}

void text::set(const std::string& val)
{
	m_string = text::to_wstring(val);
}

void text::set(const std::wstring& val)
{
	m_string = val;
}

void text::set(const int32_t& val)
{
	m_string = text::to_wstring(val);
}

void text::set(const uint32_t& val)
{
	m_string = text::to_wstring(val);
}

void text::set(const float& val)
{
	m_string = text::to_wstring(val);
}

std::string text::string() const
{
	return text::to_string(m_string);
}

const std::wstring& text::wstring() const
{
	return m_string;
}

bool text::equals(const std::string& val) const
{
	return m_string == text::to_wstring(val);
}

bool text::equals(const std::wstring& val) const
{
	return m_string == val;
}

bool text::equals(const text& val) const
{
	return m_string == val.m_string;
}

std::wstring text::to_wstring(const std::string& val)
{
    std::wstring temp(val.size(),L' ');
    std::copy(val.begin(), val.end(), temp.begin());
    return temp;
}

std::string text::to_string(const std::wstring& val)
{
    std::locale const loc("");
    wchar_t const* from = val.c_str();
    std::size_t const len = val.size();
    std::vector<char> buffer(len + 1);
    std::use_facet<std::ctype<wchar_t> >(loc).narrow(from, from + len, '_', &buffer[0]);
    return std::string(&buffer[0], &buffer[len]);
}

// To boolean
bool text::to_bool() const
{
	return text::to_bool(m_string);
}

bool text::to_bool(const std::wstring& val)
{
    std::wstringstream converter;
    bool value = false;
    converter.setf(std::ios::boolalpha);
    converter << val;
    converter >> value;
    return value;
}

bool text::to_bool(const std::string& val)
{
    std::stringstream converter;
    bool value = false;
    converter.setf(std::ios::boolalpha);
    converter << val;
    converter >> value;
    return value;
}

// To integer
int32_t	text::to_int32() const
{
	return to_int32(m_string);
}

int32_t text::to_int32(const std::wstring& str_val)
{
    std::wstringstream converter;
    int32_t value = 0;
    converter << str_val;
    converter >> value;
    return value;
}

int32_t text::to_int32(const std::string& str_val)
{
    std::stringstream converter;
    int32_t value = 0;
    converter << str_val;
    converter >> value;
    return value;
}

// To float
float text::to_float() const
{
	return to_float(m_string);
}

float text::to_float(const std::wstring& str_val)
{
    std::wstringstream converter;
    float value = 0;
    converter.precision(4);
    converter.fill('0');
    converter.setf( std::ios::fixed, std::ios::basefield );
    converter << str_val;
    converter >> value;
    return value;
}

float text::to_float(const std::string& str_val)
{
    std::stringstream converter;
    float value = 0;
    converter.precision(4);
    converter.fill('0');
    converter.setf( std::ios::fixed, std::ios::basefield );
    converter << str_val;
    converter >> value;
    return value;
}

std::wstring text::to_wstring(const bool& val)
{
	std::wstringstream converter;
	std::wstring value;
    converter.setf(std::ios::boolalpha);
    converter << val;
    converter >> value;
    return value;

}

std::string  text::to_string(const bool& val)
{
	std::stringstream converter;
	std::string value;
    converter.setf(std::ios::boolalpha);
    converter << val;
    converter >> value;
    return value;
}

std::wstring text::to_wstring(const int32_t& val)
{
	std::wstringstream converter;
	std::wstring value;
    converter << val;
    converter >> value;
    return value;
}

std::string  text::to_string(const int32_t& val)
{
	std::stringstream converter;
	std::string value;
    converter << val;
    converter >> value;
    return value;
}

std::wstring text::to_wstring(const uint32_t& val)
{
	std::wstringstream converter;
	std::wstring value;
    converter << val;
    converter >> value;
    return value;
}

std::string  text::to_string(const uint32_t& val)
{
	std::stringstream converter;
	std::string value;
    converter << val;
    converter >> value;
    return value;
}


std::wstring text::to_wstring(const float& val)
{
	std::wstringstream converter;
	std::wstring value;
    converter << val;
    converter >> value;
    return value;
}

std::string  text::to_string(const float& val)
{
	std::stringstream converter;
	std::string value;
    converter << val;
    converter >> value;
	return value;
}

void text::from_bool(const bool& val)
{
	m_string = text::to_wstring(val);
}

void text::from_int32(const int32_t& val)
{
	m_string = text::to_wstring(val);
}

void text::from_float(const float& val)
{
	m_string = text::to_wstring(val);
}

text text::to_text(const bool& val)
{
	text tmp;
	tmp.from_bool(val);
	return tmp;
}

text text::to_text(const int32_t& val)
{
	text tmp;
	tmp.from_int32(val);
	return tmp;
}

text text::to_text(const float& val)
{
	text tmp;
	tmp.from_float(val);
	return tmp;
}


text text::to_lower () const
{
	owlsl::text ret = *this;
	std::transform(ret.m_string.begin(), ret.m_string.end(), ret.m_string.begin(), ::tolower);
	return ret;
}

text text::to_lower (const owlsl::text& val)
{
	owlsl::text ret = val;
	return ret.to_lower();
}

text text::to_upper () const
{
	owlsl::text ret = *this;
	std::transform(ret.m_string.begin(), ret.m_string.end(), ret.m_string.begin(), ::toupper);
	return ret;
}

text text::to_upper (const owlsl::text& val)
{
	owlsl::text ret = val;
	return ret.to_upper();
}

// Max lenght a string can has
size_t text::max_length ()
{
	return std::wstring::npos;
}

const bool text::contains (const owlsl::text& val) const
{
	return std::wstring::npos != find(val);
}

// Find substring
const size_t text::find (const owlsl::text& val, const size_t& pos) const
{
	return text::find(*this, val.wstring(), pos);
}

const size_t text::find (const owlsl::text &str, const owlsl::text& val, const size_t& pos)
{
	return text::find (str.m_string, val.m_string, pos);
}

const size_t text::find (const std::wstring &str, const std::wstring& val, const size_t& pos)
{
	return str.find(val, pos);
}

// Find last occurence of substring
const size_t text::find_last (const owlsl::text& val) const
{
	return text::find_last(*this, val.wstring());
}

const size_t text::find_last (const owlsl::text &str, const owlsl::text& val)
{
	return text::find_last(str.m_string, val.m_string);
}

const size_t text::find_last (const std::wstring &str, const std::wstring& val)
{
	return str.find_last_of(val);
}

// Replace characters
void text::replace (const owlsl::text &find, const owlsl::text &replace)
{
	text::replace(m_string, find.wstring(), replace.wstring());
}

void text::replace(std::string &str, const std::string &find, const std::string &replace)
{
    std::string::size_type pos=0;
    while((pos=str.find(find, pos))!=std::string::npos)
    {
        str.erase(pos, find.length());
        str.insert(pos, replace);
        pos+=replace.length();
    }
}

void text::replace(std::wstring &str, const std::wstring &find, const std::wstring &replace)
{
    std::wstring::size_type pos=0;
    while((pos=str.find(find, pos))!=std::wstring::npos)
    {
        str.erase(pos, find.length());
        str.insert(pos, replace);
        pos+=replace.length();
    }
}

const size_t text::count (const owlsl::text& val) const
{
	return count(m_string, val.wstring());
}

const size_t text::count (const std::wstring &str, const std::wstring& val)
{
	unsigned count = 0;
    std::wstring::size_type pos=0;
    while((pos=str.find(val, pos))!=std::wstring::npos)
    {
		pos++;
		count++;
    }
	return count;
}

text& text::erase(const size_t& pos, const size_t& length)
{
	m_string.erase(pos, length);
	return *this;
}

text& text::insert(const size_t& pos, const text& t)
{
	m_string.insert(pos, t.m_string);
	return *this;
}

text& text::trim_left()
{
	text::trim_left(m_string);
	return *this;
}

std::string& text::trim_left(std::string& str)
{
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return str;
}

std::wstring& text::trim_left(std::wstring& str)
{
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return str;
}

text& text::trim_right()
{
	text::trim_right(m_string);
	return *this;
}

std::string& text::trim_right(std::string& str)
{
    str.erase(std::find_if(str.rbegin(), str.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), str.end());
	return str;
}

std::wstring& text::trim_right(std::wstring& str)
{
	str.erase(std::find_if(str.rbegin(), str.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), str.end());
	return str;
}

text& text::trim()
{
	text::trim(m_string);
	return *this;
}

std::string& text::trim(std::string& str)
{
	return trim_left(trim_right(str));
}

std::wstring& text::trim(std::wstring& str)
{
	return trim_left(trim_right(str));
}

text& text::fill_left(const int& amount, const owlsl::text& character)
{
	text::fill_left(m_string, amount, character.wstring());
	return *this;
}

std::string& text::fill_left(std::string& str, const int& amount, const std::string& character)
{
	std::stringstream ss;
	ss << std::right << std::setw(amount) << std::setfill(character[0]) << str;
	str = ss.str();
	return str;
}

std::wstring& text::fill_left(std::wstring& str, const int& amount, const std::wstring& character)
{
	std::wstringstream ss;
	ss << std::right << std::setw(amount) << std::setfill(character[0]) << str;
	str = ss.str();
	return str;
}

text& text::fill_right(const int& amount, const owlsl::text& character)
{
	text::fill_right(m_string, amount, character.wstring());
	return *this;
}

std::string& text::fill_right(std::string& str, const int& amount, const std::string& character)
{
	std::stringstream ss;
	ss << std::left << std::setw(amount) << std::setfill(character[0]) << str;
	str = ss.str();
	return str;
}

std::wstring& text::fill_right(std::wstring& str, const int& amount, const std::wstring& character)
{
	std::wstringstream ss;
	ss << std::left << std::setw(amount) << std::setfill(character[0]) << str;
	str = ss.str();
	return str;
}

bool text::is_number() const
{
	return is_number(m_string);
}

bool text::is_number(const owlsl::text& val)
{
	owlsl::text t = val;
	t.replace(L",",L"");t.replace(L".",L"");
	const std::wstring& s = t.wstring();
    std::wstring::const_iterator it = s.begin();
	while (it != s.end() && iswdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

text text::subtext(const size_t& pos, const size_t& size) const
{
	return text(m_string.substr(pos,size));
}

void text::to_array	(std::vector<uint8_t>& arr, const encoding::id& encoding) const
{
	switch (encoding)
	{
		case encoding::ascii:
		{
			std::string str8 = string();
			uint8_t* data = (uint8_t*)str8.data();
			arr.insert(arr.end(), &data[0], &data[str8.size()]);
		}
		break;

		case encoding::utf16le:
		{
			static const size_t size_of_char = sizeof(wchar_t);
			switch(size_of_char)
			{
				case 2:
				{
					char* data = (char*)m_string.data();
					std::copy(data, data+bytes(), std::back_inserter(arr));
				}
				break;
				case 4:
				{
					size_t size = bytes();
					char* data = (char*)m_string.data();
					for (size_t i=0; i<size; i+=4)
					{
						arr.push_back(data[i]);
						arr.push_back(data[i+1]);
					}
				}
				break;
			}
		}
		break;

		default: return;
	}
}

void text::get_mark	(std::vector<uint8_t>& arr, const encoding::id& encoding)
{
	switch (encoding)
	{
		case encoding::ascii: return;

		case encoding::utf16le:
		{
			arr.push_back(0xFF);
			arr.push_back(0xFE);
		}
		break;

		case encoding::utf16be:
		{
			arr.push_back(0xFE);
			arr.push_back(0xFF);
		}
		break;

		case encoding::utf32le:
		{
			arr.push_back(0xFF);
			arr.push_back(0xFE);
			arr.push_back(0x00);
			arr.push_back(0x00);
		}
		break;

		case encoding::utf32be:
		{
			arr.push_back(0x00);
			arr.push_back(0x00);
			arr.push_back(0xFE);
			arr.push_back(0xFF);
		}
		break;

		case encoding::utf8:
		{
			arr.push_back(0xEF);
			arr.push_back(0xBB);
			arr.push_back(0xBF);
		}
		break;

		default:
		{

		}
		break;
	}
}
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

#ifndef OWL_SCRIPT_text_H
#define OWL_SCRIPT_text_H

#include "../_export.h"
#include <string>
#include <vector>

namespace owlsl {

	struct encoding
	{
		enum id {ascii, utf8, utf16le, utf16be, utf32le, utf32be};
	};

	class OWL_SCRIPT text : public owlsl::dllnew
	{
		public:
								 text		();
			virtual 			~text		();

								 text		(const char* val);
								 text		(const wchar_t* val);
								 text		(const std::string& val);
								 text		(const std::wstring& val);
								 text		(const int32_t& val);
								 text		(const uint32_t& val);
								 text		(const float& val);

			void				 set		(const char* val);
			void				 set		(const wchar_t* val);
			void				 set		(const text& val);
			void				 set		(const std::string& val);
			void				 set		(const std::wstring& val);
			void				 set		(const int32_t& val);
			void				 set		(const uint32_t& val);
			void				 set		(const float& val);

			std::string			 string		() const;
			const std::wstring&  wstring	() const;
			
			bool				 equals		(const std::string& val) const;
			bool				 equals		(const std::wstring& val) const;
			bool				 equals		(const text& val) const;

			bool				 to_bool	() const;
			static bool			 to_bool	(const std::wstring& val);
			static bool			 to_bool	(const std::string& val);

			int32_t				 to_int32	() const;
			static int32_t		 to_int32	(const std::wstring& str_val);
		    static int32_t		 to_int32	(const std::string& str_val);

			float				 to_float	() const;
			static float		 to_float	(const std::wstring& str_val);
			static float		 to_float	(const std::string& str_val);

			void				 from_bool	(const bool& val);
			void				 from_int32	(const int32_t& val);
			void				 from_float	(const float& val);

			static text			 to_text	(const bool& val);
			static text			 to_text	(const int32_t& val);
			static text			 to_text	(const float& val);

			static std::wstring  to_wstring	(const std::string& val);
			static std::string   to_string	(const std::wstring& val);
			static std::wstring  to_wstring	(const bool& val);
			static std::string   to_string	(const bool& val);
			static std::wstring  to_wstring	(const int32_t& val);
			static std::string   to_string	(const int32_t& val);
			static std::wstring  to_wstring	(const uint32_t& val);
			static std::string   to_string	(const uint32_t& val);
			static std::wstring  to_wstring	(const float& val);
			static std::string   to_string	(const float& val);

			text				 to_lower	() const;
			static text			 to_lower	(const owlsl::text& val);

			text				 to_upper	() const;
			static text			 to_upper	(const owlsl::text& val);

			static size_t		 max_length ();

			const bool			 contains	(const owlsl::text& val) const;
			const size_t		 find		(const owlsl::text& val, const size_t& pos=0) const;
			static const size_t	 find		(const owlsl::text &str, const owlsl::text& val, const size_t& pos=0);
			static const size_t	 find		(const std::wstring &str, const std::wstring& val, const size_t& pos=0);

			const size_t		 find_last	(const owlsl::text& val) const;
			static const size_t	 find_last	(const owlsl::text &str, const owlsl::text& val);
			static const size_t	 find_last	(const std::wstring &str, const std::wstring& val);

			void				 replace	(const owlsl::text &find, const owlsl::text &replace);
			static void			 replace	(std::string &str, const std::string &find, const std::string &replace);
			static void			 replace	(std::wstring &str, const std::wstring &find, const std::wstring &replace);

			const size_t		 count		(const owlsl::text& val) const;
			static const size_t  count		(const std::wstring &str, const std::wstring& val);

			text&				 erase		(const size_t& pos, const size_t& length);

			text&				 insert		(const size_t& pos, const text& t);

			text				 subtext	(const size_t& pos, const size_t& size) const;

			text&				 trim_left	();
			static std::string&  trim_left	(std::string& str);
			static std::wstring& trim_left	(std::wstring& str);

			text&				 trim_right	();
			static std::string&  trim_right	(std::string& str);
			static std::wstring& trim_right	(std::wstring& str);

			text&				 trim		();
			static std::string&  trim		(std::string& str);
			static std::wstring& trim		(std::wstring& str);

			text&				 fill_left	(const int& amount, const owlsl::text& character);
			static std::string&  fill_left	(std::string& str, const int&  amount, const std::string& character);
			static std::wstring& fill_left	(std::wstring& str, const int&  amount, const std::wstring& character);

			text&				 fill_right	(const int&  amount, const owlsl::text& character);
			static std::string&  fill_right	(std::string& str, const int&  amount, const std::string& character);
			static std::wstring& fill_right	(std::wstring& str, const int&  amount, const std::wstring& character);

			size_t				 length		() const	{ return m_string.size(); }
			size_t				 bytes		() const	{ return m_string.size() * sizeof(wchar_t); }
			void				 clear		()			{ m_string = L""; }

			bool				 is_number	() const;
			static bool			 is_number	(const owlsl::text& val);

			void				 to_array	(std::vector<uint8_t>& arr, const encoding::id& encoding=encoding::ascii) const;
			static void			 get_mark	(std::vector<uint8_t>& arr, const encoding::id& encoding);

		// Operators
		public:

			void		operator =      		(const text& val)				{ set (val); }
			bool		operator ==     		(const text& val)		const	{ return equals(val); }
			bool		operator !=     		(const text& val)		const	{ return !equals(val); }

			text		operator + 				(const text& val)		const	{ text t; t.m_string = m_string + val.m_string; return t; }
			text		operator + 				(const int32_t& val)	const	{ text t; t.m_string = m_string + text::to_wstring(val); return t; }
			text		operator +				(const uint32_t& val)	const	{ text t; t.m_string = m_string + text::to_wstring(val); return t; }
			text		operator + 				(const float& val)		const	{ text t; t.m_string = m_string + text::to_wstring(val); return t; }

			text&		operator +=    			(const text& val)				{ m_string += val.m_string; return *this; }
			text&		operator +=				(const int32_t& val)			{ m_string += text::to_wstring(val); return *this; }
			text&		operator +=				(const uint32_t& val)			{ m_string += text::to_wstring(val); return *this; }
			text&		operator +=				(const float& val)				{ m_string += text::to_wstring(val); return *this; }

						operator const wchar_t*	() const						{ return m_string.c_str(); }

		protected:

			std::wstring m_string;
	};

	static const owlsl::text endl("\n");
}
#endif //OWL_SCRIPT_string_H

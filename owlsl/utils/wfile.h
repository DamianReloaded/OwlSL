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
#ifndef OWL_SCRIPT_wfile_H
#define OWL_SCRIPT_wfile_H

#include "mfile.h"

namespace owlsl {

    class OWL_SCRIPT wfile : public owlsl::dllnew
    {
        public:
            typedef std::vector<wchar_t>    wchars;
            typedef std::vector<wchars>		wcharlines;

									wfile();
            virtual					~wfile();

            bool					open						(const owlsl::text& file_path);
            bool					open						(const uint8_t* buffer, const size_t& size);
			bool					open						(owlsl::mfile& f);
            void					close						();
            bool					is_unicode					();
			const owlsl::text&		file_path					();
			wcharlines&				lines						();

        protected:
            void					get_lines					();
            bool					is_unicode					(const uint8_t* buffer);
            bool					is_little_endian			(const uint8_t* buffer);
            void					read_unicode				();
            void					read_unicode_little_endian	();
            void					read_unicode_big_endian		();
            void					read_ascii					();
            size_t					is_eol_unicode				(const uint8_t* buffer, const size_t& pos);
            size_t					is_eol_ascii				(const uint8_t* buffer, const size_t& pos);

            bool					m_is_unicode;
			bool					m_file_owned;
            owlsl::mfile*				m_file;
			wcharlines				m_lines;
    };
}

#endif // OWL_SCRIPT_wfile_H

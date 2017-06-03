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

#ifndef OWL_SCRIPT_mfile_H
#define OWL_SCRIPT_mfile_H

#include "../utils/text.h"
#include <vector>

namespace owlsl {

	class OWL_SCRIPT mfile : public owlsl::dllnew
	{
		public:
										mfile			();
			virtual						~mfile			();

			bool						open			(const owlsl::text& file_path);
            bool						save			(const owlsl::text& file_path);

            void						seek			(size_t pos);
            size_t						read			(uint8_t* buff, size_t amount);

            bool						append			(const std::string& data);
			bool						append			(const std::wstring& data);
            bool						append			(const std::vector<uint8_t>& data);
            bool						append			(const uint8_t* buffer, const size_t& size);
			void						append			(const owlsl::text& text, const encoding::id& encoding=encoding::ascii);
			void						append			(const encoding::id& encoding);

            void						close			();
            std::vector<uint8_t>&		buffer			();
            const uint32_t				size			();
            void						resize			(uint32_t nsize);
            const text&					last_error		();
            const owlsl::text&			file_path		();
            void						set_file_path	(const owlsl::text& val);

			bool						is_open			();
			void						set_as_open		();

        protected:
            size_t						m_pos;
            std::vector<uint8_t>		m_buffer;
            text						m_last_error;
            owlsl::text					m_file_path;
			bool						m_is_open;
	};
}

#endif // OWL_SCRIPT_mfile_H
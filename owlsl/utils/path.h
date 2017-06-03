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
#ifndef OWL_SCRIPT_path_H
#define OWL_SCRIPT_path_H

#include <owlsl/utils/text.h>

namespace owlsl {

	class OWL_SCRIPT path : public text
	{
		public:
			static const unsigned max_path_length = 4096;

		public:
						path();
			virtual		~path();

						path						(const char* val);
						path						(const wchar_t* val);
						path						(const text& val);

			path		parent_path					() const;
			path		filename					() const;
			path		filename_without_extension	() const;
			owlsl::text	extension					() const;

			bool		exists						() const;
			bool		delete_file					() const;

			bool		is_root						() const;
			bool		is_accessible				() const;
			bool		is_folder					() const;
			bool		is_file						() const;

			path		real_path					() const;
			size_t		file_size					() const;

		// Operators
		public:
			path	operator /    	(const text& val)		  { return path(m_string + L"/" + val.wstring()); }
			path&	operator /=    	(const text& val)		  { m_string += L"/" + val.wstring(); return *this;}
	};

}

#endif // OWL_SCRIPT_path_H
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
#ifndef owl_size_H
#define owl_size_H

#include "../_export.h"

namespace owlsl {

	template <class T>
	class size
	{
		public:
            size()
			{
				set((T)0, (T)0);
			}

            size(const T& pwidth, const T& pheight)
			{
				set(pwidth, pheight);
			}

			void set(const T& pwidth, const T& pheight)
            {
                m_width = pwidth;
                m_height = pheight;
            }

			const T&	width()  const { return m_width; }
			const T&	height() const { return m_height; }

			void		width(const T& val)  { m_width = val; }
			void		height(const T& val) { m_height = val; }

		protected:
            T m_width;
            T m_height;
    };

	typedef owlsl::size<float>	sizef;
	typedef owlsl::size<double>	sized;
	typedef owlsl::size<int32_t>	sizei;
	typedef owlsl::size<uint32_t>	sizeu;
}

#endif // owl_size_H
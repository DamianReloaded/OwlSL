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
#ifndef OWL_SCRIPT_point_H
#define OWL_SCRIPT_point_H

#include "../_export.h"

namespace owlsl {

	template <class T>
	class point
	{
		public:
            point()
			{
				set((T)0, (T)0);
			}

            point(const T& px, const T& py)
			{
				set(px, py);
			}

			void set(const T& px, const T& py)
            {
                m_x = px;
                m_y = py;
            }

			void set(point<float>& p)
            {
                m_x = p.x();
                m_y = p.y();
            }

			const T& x() const { return m_x; }
			const T& y() const { return m_y; }

			void x(const T& val) { m_x = val; }
			void y(const T& val) { m_y = val; }

		protected:
            T m_x;
            T m_y;
    };

	typedef owlsl::point<float>	pointf;
	typedef owlsl::point<int32_t>	pointi;
}

#endif //OWL_SCRIPT_point_H
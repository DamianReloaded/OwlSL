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
#ifndef OWL_SCRIPT_rectangle_H
#define OWL_SCRIPT_rectangle_H

#include "point.h"
#include "size.h"

namespace owlsl {

	template <class T>
	class rectangle
	{
		public:
					rectangle()	{}
			virtual ~rectangle() {}

			rectangle(const T& px, const T& py, const T& pwidth, const T& pheight)
			{
				set(px, py, pwidth, pheight);
			}

			rectangle(const owlsl::point<T>& _location, const owlsl::size<T>& _size)
			{
				set(_location, _size);
			}

			void set(const T& px, const T& py, const T& pwidth, const T& pheight)
			{
				m_location.set(px, py);
				m_size.set(pwidth, pheight);
			}

			void set(const owlsl::point<T>& _location, const owlsl::size<T>& _size)
			{
				m_location = _location;
				m_size	   = _size;
			}

			const point<T>& location() const
			{ 
				return m_location; 
			}

			void location(const point<T>& val) { m_location = val; }
			void location(const T& px, const T& py) { m_location.set(px,py); }

			const owlsl::size<T>&  size()     const
			{ 
				return m_size; 
			}

			void size(const owlsl::sizef& val)	{ m_size.set((T)val.width(), (T)val.height()); }
			void size(const owlsl::sizei& val)	{ m_size.set((T)val.width(), (T)val.height()); }
			void size(const T& w, const T& h )	{ m_size.set(w, h); }

			const bool intersects(const rectangle<T>& rect) const
			{
				const T& this_x2 = m_location.x()+m_size.width();
				const T& this_y2 = m_location.y()+m_size.height();
				const T& parent_x2 = rect.m_location.x()+rect.m_size.width();
				const T& parent_y2 = rect.m_location.y()+rect.m_size.height();
				if (m_location.x() > parent_x2) return false;
				if (m_location.y() > parent_y2) return false;
				if (this_x2 < rect.m_location.x()) return false;
				if (this_y2 < rect.m_location.y()) return false;
				return true;
			}

			const bool set_intersection(const rectangle<T>& rect)
			{
				const T& this_x2 = m_location.x()+m_size.width();
				const T& this_y2 = m_location.y()+m_size.height();
				const T& parent_x2 = rect.m_location.x()+rect.m_size.width();
				const T& parent_y2 = rect.m_location.y()+rect.m_size.height();

				if ( (m_location.x() > parent_x2) || (m_location.y() > parent_y2) || (this_x2 < rect.m_location.x()) || (this_y2 < rect.m_location.y()) )
				{
					m_size.width(0);
					m_size.height(0);
					m_location.x(0);
					m_location.y(0);
					return false;
				}

				// left limit
				if (m_location.x()<rect.m_location.x())
				{
					m_size.width(m_size.width()- (rect.m_location.x()-m_location.x()));
					m_location.x(rect.m_location.x());
				}

				// top limit
				if (m_location.y()<rect.m_location.y())
				{
					m_size.height(m_size.height()- (rect.m_location.y()-m_location.y()));
					m_location.y(rect.m_location.y());
				}

				// rigm_size.height()t limit
				if (m_location.x()+m_size.width() > rect.m_location.x()+rect.m_size.width())
				{
					m_size.width(m_size.width()- ((m_location.x()+m_size.width()) - (rect.m_location.x()+rect.m_size.width()))) ;
				}

				// bottom limit
				if (m_location.y()+m_size.height() > rect.m_location.y()+rect.m_size.height())
				{
					m_size.height(m_size.height() - ((m_location.y()+m_size.height()) - (rect.m_location.y()+rect.m_size.height())));
				}

				return true;
			}

			const bool pointinside(const point<T>& p) const
			{
				return pointinside(p.x(), p.y());
			}

			const bool pointinside(const T& x, const T& y) const
			{
				return (x >= m_location.x())&&(x<=m_location.x()+m_size.width())&&
					   (y >= m_location.y())&&(y<=m_location.y()+m_size.height());
			}

		protected:
			owlsl::point<T> m_location;
            owlsl::size<T>  m_size;
    };

	typedef owlsl::rectangle<float>		rectanglef;
	typedef owlsl::rectangle<double>		rectangled;
	typedef owlsl::rectangle<int32_t>		rectanglei;
	typedef owlsl::rectangle<uint32_t>	rectangleu;
}

#endif //OWL_SCRIPT_rectangle_H
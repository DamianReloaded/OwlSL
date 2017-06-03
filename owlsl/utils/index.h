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
#ifndef OWL_SCRIPT_index_H
#define OWL_SCRIPT_index_H

#include <vector>
#include <owlsl/utils/map.h>
#include <owlsl/utils/text.h>

namespace owlsl {

	template <class T>
	class index
	{
		public:
			int add(const owlsl::text& name, const T& t)
			{
				m_by_id.push_back(t);
				m_by_name.insert(name.wstring(), m_by_id.size()-1);
				return m_by_id.size()-1;
			}

			const unsigned& id(const owlsl::text& name)
			{
				m_by_name.find(name.wstring());
				return m_by_name.value();
			}

			const owlsl::text name(const unsigned& id)
			{
				for (m_by_name.move_first(); !m_by_name.eof(); m_by_name.move_next())
				{
					if (m_by_name.value() == id)
					{
						return owlsl::text(m_by_name.key());
					}
				}
				return owlsl::text();
			}

			T& by_name(const owlsl::text& name)
			{
				m_by_name.find(name.wstring());
				return m_by_id[m_by_name.value()];
			}

			T& by_id(const unsigned& id)
			{
				return m_by_id[id];
			}

			bool contains(const owlsl::text& name)
			{
				return m_by_name.find(name.wstring());
			}

			bool contains(const unsigned& id)
			{
				return (id < m_by_id.size());
			}

			unsigned size()
			{
				return m_by_id.size();
			}

			T& operator [] (const unsigned& id) { return by_id(id); }
			T& operator [] (const owlsl::text& name) { return by_name(name); }

		private:
			owlsl::map<std::wstring, unsigned> m_by_name;
			std::vector<T>					 m_by_id;
	};

}

#endif // OWL_SCRIPT_index_H
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
#ifndef INCLUDE_OWL_SCRIPT_map_H
#define INCLUDE_OWL_SCRIPT_map_H

#include <iostream>
#include <assert.h>
#include <map>

namespace owlsl {

	template <class TKey, class TVal>
	class map
	{
		protected:
					 std::map<TKey, TVal>				m_map;
			typename std::map<TKey, TVal>::iterator		m_item;

		public:
			// Construction
										map()						{ m_item = m_map.begin(); }
			virtual						~map()						{ }

			// Iterator management
			inline void					move_first() 				{ m_item = m_map.begin(); }
			inline void					move_last() 				{ m_item = m_map.end(); }
			inline bool					move_next() 				{ ++m_item; return !eof(); }
			inline bool					move_previous() 			{if (!bof()) {--m_item; return true;} else return false; }
			inline bool					bof() 						{ return (m_map.begin()==m_item); }
			inline bool					eof() 						{ return (m_map.end()==m_item); }
			inline const TKey&			key() 						{ assert(m_item!=m_map.end()); return (*m_item).first; }
			inline TVal&				value() 					{ assert(m_item!=m_map.end()); return (*m_item).second; }
			inline void					key(TKey& key) 		        { (*m_item).first = key; }
			inline void					value(TVal& val) 			{ (*m_item).second = val; }
			inline bool					erase(const TKey& key) 		{ return (m_map.erase(key) > 0); }
			inline bool					find(const TKey& key) 		{ return (m_item = m_map.find( key ))!=m_map.end(); }
			inline bool 	operator	() (const TKey& key)		{ return find(key);}
			inline void					clear() 					{ std::map<TKey, TVal>().swap(m_map); }
			inline size_t				size() 						{ return m_map.size(); }

			inline bool insert(const TKey& key, const TVal& val, bool replace=false)
			{
				return m_map.insert(std::make_pair(key,val)).second; // will be false if the element already exists
            }

	};
}

#endif //INCLUDE_OWL_SCRIPT_map_H

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

#ifndef owl_script_heap_H
#define owl_script_heap_H

#include "_export.h"
#include <vector>

namespace owlsl {

	class vm;

	template <class T>
	struct OWL_SCRIPT heap_obj
	{
		T*  instance;
		int pos;
	};

	struct OWL_SCRIPT var : public owlsl::dllnew
	{
		var () : instance(NULL), type(-1), owned(false), refcount(0) {}

		void*		instance;
		int			type;
		bool		owned;
		unsigned	refcount;

	};

	class OWL_SCRIPT stype : public owlsl::dllnew
	{
		public:
							stype() {}
			virtual			~stype() {}
			std::vector<int> prop;
	};

	class OWL_SCRIPT heap : public owlsl::dllnew
	{
		public:
						heap();
			virtual		~heap();

			void preallocate();
			int	 new_var();
			void terminate();

			template <class T>
			int palloc(const unsigned& type, T* p, const bool& _owned = true)
			{
				unsigned pos = new_var();
				var& v = mem[pos];
				v.instance = p;
				v.type = type;
				v.owned = _owned;
				return pos;
			}

			template <class T>
			int pdalloc(const int& pos)
			{
				var& v = mem[pos];

				if (NULL == v.instance) return 0;

				T* p = cast<T>(pos);
				delete p;

				v.instance = NULL;
				v.type = -1;
				v.owned = false;
				v.refcount = 0;
				unused.push_back(pos);
				return 0;
			}

			template <class T>
			T* cast(const int& pos)
			{
				return static_cast<T*>(mem[pos].instance);
			}

			void	size_factor(const unsigned& value);

			std::vector<var>		mem;
			std::vector<unsigned>	unused;
			owlsl::vm*		vm;
			unsigned				m_size_factor;

	};

}

#endif // owl_script_heap_H

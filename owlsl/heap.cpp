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

#include "heap.h"
#include "vm.h"
using namespace owlsl;

heap::heap()
{
	size_factor(20);
}

heap::~heap()
{

}

void heap::preallocate()
{
	unsigned prevsize = mem.size();
	unsigned num = prevsize+m_size_factor;
	mem.resize(num);
	for(unsigned i=prevsize; i<num; i++) unused.push_back(i);
}

int heap::new_var()
{
	if (0==unused.size()) preallocate();
	unsigned pos = unused.back();
	unused.pop_back();
	return pos;
}

void heap::terminate()
{
	for (unsigned i=0; i<mem.size(); i++)
	{
		var& v = mem[i];
		if (v.instance != NULL)
		{
			vm->pdestruct(i);
		}
	}
}

void heap::size_factor(const unsigned& value)
{
	if (mem.size()<value)
	{
		m_size_factor = value-mem.size();
		preallocate();
	}
	else
	{
		m_size_factor = value;
		preallocate();
	}
	m_size_factor = value;
}
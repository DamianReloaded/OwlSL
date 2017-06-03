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
#ifndef owl_script_vm_H
#define owl_script_vm_H

#include "_export.h"
#include <deque>
#include <owlsl/utils/index.h>
#include "heap.h"

namespace owlsl {

	class sco;

	struct OWL_SCRIPT opcode
	{
		enum id
		{
			pushctx, popctx, push, pushs, pop, palloc, pdalloc, pcall, salloc, sdalloc, scall, sreturn0, sreturn1, jump, end, pushc, ialloc, jumpt, jumpf, max_opcode
		};
	};

	class OWL_SCRIPT vm : public owlsl::dllnew
	{
		public:
			typedef void (vm::*fop)(); 
			typedef std::vector<fop> optable;

		public:
			optable				op;

			void				pushctx				(); // push context
			void				popctx				();	// pop context
			void				push				();	// push heap pos into the stack
			void				pushs				();	// push pos contained in stack location again into the stack
			void				pop					();	// pop last stack value
			void				palloc				();	// allocate new variable
			void				pdalloc				();	// deallocate variable
			void				pcall				();	// call an exposed program method
			void				salloc				();	// allocate new variable
			void				sdalloc				();	// deallocate variable
			void				scall				();	// call an script defined method
			void				sreturn0			();	// exit function
			void				sreturn1			();	// save a return value into rtx register and exit function
			void				jump				();	// jump to a code location
			void				end					();	// exit a script method
			void				pushc				();	// push a script defined class property class.prop1.prop2 etc
			void				ialloc				();	// allocate a variable that will hold an instance managed by the program
			void				jumpt				();	// jump to a code location if the top of the stack is boolean true
			void				jumpf				();	// jump to a code location if the top of the stack is boolean false

			int					run					(unsigned cur=0);
			int					run_main			();
			void				ins					(const int& val);
			void				ins					(const int& val, const owlsl::text& dbg_text);

			void				dec_ref_count		(const int& pos);

			void operator << (const int& val)		{ ins(val); }

			std::vector<int>		code;
			std::vector<owlsl::text>	code_dbg;
			unsigned				cursor;
			unsigned				entry_point;

		public:
			typedef int	 (*func_int_void)  (vm*);
			typedef int	 (*func_int_int)   (vm*, const int&);
			typedef void (*func_void_int)  (vm*, const int&);
			typedef void (*func_void_void) (vm*);

			typedef void (*func_specialize) (sco*, const owlsl::text&, std::deque<owlsl::text>&);

			typedef owlsl::index<owlsl::index<int> > param_index;

		public:
							vm();
			virtual			~vm();

			int				new_type		(const owlsl::text& name, const bool& scripted = false);
			int				pconstruct		(const int& type, const int& constructor);
			void			pdestruct		(const int& pos);
			void			pcallmethod		(const int& type, const int& method);
			void			preturn			(const int& pos);
			int				push			(const int& pos);
			int				pop				(const unsigned& amount);
			void			push_context	();
			void			pop_context		();
			int				salloc			(const int& type, const int& constructor);
			int				sconstruct		(const int& type, const int& constructor);
			void			sdestruct		(const int& pos);
			void			scallmethod		(const int& type, const int& method, const bool& static_call=false);

			int				heap_pos		(const int& relative_pos); // Given a relative position in the stack returns heap pos

			template <class T>
			T* stack_cast(const int& pos)
			{
				T* p = heap.cast<T>(stack[(stack.size()-1)+pos]);
				return p;
			}

			template <class T>
			T* heap_cast(const int& pos)
			{
				T* p = heap.cast<T>(pos);
				return p;
			}

			template <class T>
			heap_obj<T>	alloc (const owlsl::text& type_name, const int& constructor = 0)
			{
				heap_obj<T> v;
				v.pos = pconstruct(table.id(type_name), constructor);
				v.instance = heap.cast<T>(v.pos);
				push(v.pos);
				return v;
			}

			template <class T>
			heap_obj<T>	alloc (const int& type_id, const int& constructor = 0)
			{
				heap_obj<T> v;
				v.pos = pconstruct(type_id, constructor);
				v.instance = heap.cast<T>(v.pos);
				push(v.pos);
				return v;
			}

			struct OWL_SCRIPT vtable : public owlsl::dllnew
			{
											vtable (const bool& _scripted) : pdestruct(NULL), scripted(_scripted) {}

				/** Exposed from program **/
				int							constructor	(const owlsl::text& name, func_int_int f)		{ return pconstruct.add(name,f); }
				int							destructor	(const owlsl::text& name, func_int_int f)		{ pdestruct = f; return 0; }
				int							method		(const owlsl::text& name, func_void_void f)	{ return pmethod.add(name, f); }
				
				owlsl::index<int>				pparameters;
				owlsl::index<int>				pretval;
				owlsl::index<func_int_int>	pconstruct;	 // Exposed constructors from program
				func_int_int				pdestruct;   // Exposed destructor from program
				owlsl::index<func_void_void>	pmethod;	 // Exposed methods from program

				func_specialize				pspecialize;

				/** Exposed from script **/
				int							constructor	(const owlsl::text& name, const int& f)	{ return sconstruct.add(name,f); }
				int							destructor	(const owlsl::text& name, const int& f)	{ sdestruct = f; return 0; }
				int							method		(const owlsl::text& name, const int& f)	{ return smethod.add(name, f); }
				
				owlsl::index<int>				sparameters;
				owlsl::index<int>				sretval;
				owlsl::index<int>				sconstruct;	 // Exposed constructors from script
				int							sdestruct;   // Exposed destructor from script
				owlsl::index<int>				smethod;	 // Exposed methods from script
				owlsl::index<int>				sproperty;	 // Exposed property/variable from script. Value is the type of variable

				bool						scripted;
			};

			owlsl::index<vtable>			table;
			owlsl::heap			heap;
			std::vector<unsigned>		stack;
			std::vector<unsigned>		context;
			std::vector<int>			func_stack;
			std::vector<unsigned>		callstack;
			int							rtx;					// return value register
	};

}

#endif // owl_script_vm_H
#include <owlsl/cwl.h>

// ------------------------------------------------------------------------------------------------
// C++ class
// ------------------------------------------------------------------------------------------------
class cconsole
{
	public:
		void print (const owlsl::text& text, const bool& callback)
		{
			std::wcout << text.wstring() <<std::endl;
			if (callback) 
			{
				on_print.call_begin();
					on_print.new_parameter<owlsl::text>("text", "Parameter from c++");
				on_print.call_end();
			}
		}

		owlsl::callback on_print;

} cons; // Global instance of the C++ class to be accessed from the script

// ------------------------------------------------------------------------------------------------
// Interface for the C++ class method 'print'
// ------------------------------------------------------------------------------------------------
static void cconsole_print(owlsl::vm* vm)
{
	// Get parameter (parameters are inverted on the stack, being 0 the last, -1 the previous etc.
	bool		callback = vm->stack_cast<owlsl::type::boolean>(0)->get(); // top of the stack
	owlsl::text&	param	 = *vm->stack_cast<owlsl::text>(-1); // top of the stack -1

	// Get this pointer (as the parameter are 0 and 1, this pointer is -2)
	cconsole*	obj		 = vm->stack_cast<cconsole>(-2); // top of the stack - 2
	
	// Call print method
	obj->print(param, callback);
	
	// pop parameter, pop this (it's up to you to pop them, if you don't they'll stay and messs everything else up)
	vm->pop(3);

	// if you wanted to return something, you would instance and push a variable here.
}

// ------------------------------------------------------------------------------------------------
// Interface for the C++ class method 'set_callback'
// ------------------------------------------------------------------------------------------------
static void cconsole_on_print(owlsl::vm* vm)
{
	// Get instance and set callback
	vm->stack_cast<cconsole>(-2)->on_print.set(vm); // two parameters (0, -1) and this (-2)
	// callback::set() pops the parameters
}

// ------------------------------------------------------------------------------------------------
// Allocator for the 'host' directive. 
// ------------------------------------------------------------------------------------------------
static int icconsole(owlsl::vm* vm, const int& type)
{
	return vm->heap.palloc<cconsole>(type, &cons, false);
}

// ------------------------------------------------------------------------------------------------
// C++ application entry point
// ------------------------------------------------------------------------------------------------
int main()
{
	// Instance the script manager
	owlsl::cwl cwl;
	cwl.heap_size(50);

	// Add internal type
	cwl.host.type("cconsole");
	// Default constructor/destructor
	cwl.host.constructor("cconsole", "cconsole()", &owlsl::type::base::alloc<cconsole>);
	cwl.host.destructor("cconsole", &owlsl::type::base::dalloc<cconsole>);
	// Declare some methods
	cwl.host.method("cconsole", "void print(text, bool)", &cconsole_print);
	cwl.host.method("cconsole", "void on_print(object, text)", &cconsole_on_print);

	// This constructor is used by the host directive to create a script variabe holding the global instance declared in this cpp file ('cons').
	cwl.host.constructor("host", "cconsole()", &icconsole);

	// Load the script file
	if (!cwl.open("../../scripts/hello_world.c"))
	{
		// Couldn't load, show the error.
		std::wcout << L"[" << cwl.last_error().line << L", " << cwl.last_error().column << L"] " << cwl.last_error().description.wstring() <<std::endl;
	}
	else
	{
		// Compile C syntax into assembly syntax and then compile that into bytecode
		if (cwl.compile())
		{
			// Instance and run some class and method
			std::cout << "----------------------------------------------------------------------------" <<std::endl;
			std::cout << "PROGRAM OUTPUT" <<std::endl;
			std::cout << "----------------------------------------------------------------------------" <<std::endl;
			std::cout << std::endl;

			// Allocate and push an instance of the script class 'test'
			cwl.script.alloc("Program");
						
			// Call method 'init' from script class 'test'
			cwl.script.call("Program", "Main()");

			// Deallocate everything
			cwl.sco.vm.heap.terminate();
		}
		else
		{
			// Couldn't compile, show the error
			std::wcout <<L"Error " << L"[" << cwl.last_error().line << L", " << cwl.last_error().column << L"] " << cwl.last_error().description.wstring() <<L" - File '" << cwl.last_error().file.wstring() << L"'" << std::endl;
			std::cout << "----------------------------------------------------------------------------" <<std::endl;
		}

		// Show the assemby
		std::cout << std::endl;
		std::cout << "----------------------------------------------------------------------------" <<std::endl;
		std::cout << "ASSEMBLY" <<std::endl;
		std::cout << "----------------------------------------------------------------------------" <<std::endl;
		std::wcout << cwl.assembly.wstring() <<std::endl;
	}

	// Save the assembly to an unicode16 little endian file
	owlsl::mfile f;
	// Append byte order mark
	f.append(owlsl::encoding::utf16le);
	// Append assemby
	f.append(cwl.assembly, owlsl::encoding::utf16le);
	// Save it.
	f.save("../../scripts/test.sco");

	// Show the state of the virtual machine
	std::cout << "----------------------------------------------------------------------------" <<std::endl;
	std::cout << "VM STATE" <<std::endl;
	std::cout << "----------------------------------------------------------------------------" <<std::endl;
	std::cout << std::endl;
	std::wcout << L"Heap: "  << cwl.sco.vm.heap.mem.size() - cwl.sco.vm.heap.unused.size() <<std::endl;
	std::wcout << L"Stack: "  << cwl.sco.vm.stack.size() <<std::endl;
	std::wcout << L"Press Enter to end..." <<std::endl;
	std::cin.get();

	return 0;
}

/*
	Hello world example script file.
	Author: Damián Reloaded
*/

// We want to access a C++ instance of type 'cconsole' and we want to name it 'console'.
host cconsole console();

// Declare a script class. You can declare it in multiple files and they will join as one.
class script_class
{
	text val;

	// Print some text to the console
	void say(text param1)
	{
		val = "Using script_class: " + param1;
		console.print(val, true);
	}

	// This get's called from c++
	void on_console_print(text param2)
	{
		console.print("Script callback called: " + param2, false);
	}

	text test_return()
	{
		text t = "Hello";
		return t;
	}
}

// This is the class we are going to instance from c++
class test
{
	// Member variabes
	text		 val;
	script_class sclass;

	// Method we are going to call from c++
	void init(text value)
	{
		// Initialize callback
		console.on_print(sclass, "on_console_print(text)");

		if (true)
		{
			for (number a = 0; a<2; ++a) 
			{
				bool ran = false;
				while (ran == false)
				{
					switch (a)
					{
						case 0:
						{
							console.print("Using host console: " + value + " - iteration: " + a.text(), true);
						}

						case 1:
						{
							sclass.say(value + " - iteration: " + a.text());
						}

						default:
						{
							// Never ran
						}
					}
					ran = !ran;
				}
			}
		}
		else if (1 <= 2)
		{
			sclass.say(value);
		}
		else
		{
			console.print("Called from c++: " + value, true);
		}

		console.print("Return value: " + sclass.test_return(), true);
	}
}

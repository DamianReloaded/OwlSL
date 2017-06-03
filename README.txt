# Owl Scripting Language (OwlSL)

Read more at: http://owlsl.blogspot.com.ar/

This is a scripting engine I wrote in my spare time, for fun.
It’s my first attempt at writing a scripting system and it's very
unoptimized but it could serve well for non time-critical stuff 
like program initialization/configuration and UI management.

Features:
- zlib License.
- Strongly Typed C++/C# like syntax.
- if/else, for, while, switch control structures.
- Method overloading.
- Can be easily embeded in any c++ application.
- Has methods for helping interfacing c++ quite easily
- Script Callbacks
- Compiles on windows and linux.
- UTF16 strings
- Can parse UTF16 script files.

What it lacks:
- Namespaces
- Inheritance
- Some keywords (break, continue, etc)


Visual Studio 2008 Express solution file can be found in:
current/test/script_hello_world/win32/vs2008/hello_world.sln

Code::Blocks workspace file can be found in:
current/test/script_hello_world/linux/codeblocks/hello_world.workspace

Special thanks to all the programmers around the world who have shared
their knowledge and wisdom, to the entire open source community and to 
the following sites:

gamedev.net
stackoverflow.com
codeproject.com

Owl Scripting Language (OwlSL)

This is a scripting engine I wrote in my spare time, for fun. It’s my first attempt at writing a scripting system and for that (and for now) it’s QUITE SLOW (OwlSL) and buggy but it could serve well for non time-critical stuff like program initialization/configuration and UI management.

Features:
- zlib License.
- Strongly Typed C++/C# like syntax.
- if/else, for, while, switch control structures.
- Can be easily embeded in any c++ application.
- Has methods for helping interfacing c++ quite easily
- Script Callbacks
- Compiles on windows and linux.
- Language customization
- UTF16 strings
- Can parse UTF16 script files.

What it lacks:
- Member hiding
- Namespaces
- Inheritance

Visual Studio 2008 Express solution file can be found in:

current/test/win32/script_hello_world.sln

I haven't had time to update the Code::Blocks projects for Linux yet.


Special thanks to all the programmers around the world who have shared their knowledge and wisdom, to the entire open source community and to the following sites:
gamedev.net
stackoverflow.com
codeproject.com
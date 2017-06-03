/*
Owl Utils

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

#include <windows.h>
#include <errno.h>
#include "../path.h"
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
using namespace owlsl;

path::path()
{

}

path::~path()
{

}

path::path(const char* val)
{
	set(val);
}

path::path(const wchar_t* val)
{
	set(val);
}

path::path(const text& val)
{
	set(val);
}

path path::parent_path() const
{
	path tmp = subtext(0, find_last(L"/"));
	if (tmp.length()==0)
	{
		tmp = "/";
	}
	return tmp;
}

path path::filename() const
{
	size_t pos = find_last(L"/");
	if (pos == max_length())
	{
		return *this;
	}

	return subtext(pos+1, length()-pos);
}

path path::filename_without_extension() const
{
	text tmp = filename();

	size_t pos = tmp.find_last(L".");
	if (pos == max_length())
	{
		return tmp;
	}

	return tmp.subtext(0, pos);
}

bool path::exists() const
{
	path tmp = real_path();
	return tmp.is_folder() || tmp.is_file();
}

bool path::delete_file() const
{
	try
	{
		if( remove( string().c_str() ) != 0 )
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	catch(std::exception ex)
	{
	}
	return false;
}

owlsl::text path::extension () const
{
	text tmp = filename();

	size_t pos = tmp.find_last(L".");
	if (pos == max_length())
	{
		return *this;
	}

	return tmp.subtext(pos+1, length()-pos);
}

bool path::is_root () const
{
	return (parent_path().parent_path()=="/");
}

bool path::is_accessible () const
{
	path tmp = real_path();
	try
	{
		struct stat info;
		return ( stat( tmp.string().c_str(), &info ) != 0 );
	}
	catch(std::exception ex)
	{
	}
	return false;
}

bool path::is_folder () const
{
	path tmp = real_path();

	try
	{
		struct stat info;
		int ret = stat( tmp.string().c_str(), &info );

		if( ret != 0 )
		{
			return false;
		}
		else if( info.st_mode & S_IFDIR )
		{
			return true;
		}
	}
	catch(std::exception ex)
	{
	}

	return false;
}

bool path::is_file () const
{
	path tmp = real_path();

	try
	{
		struct stat info;
		int ret = stat( tmp.string().c_str(), &info );

		if( ret != 0 )
		{
			return false;
		}
		else if( info.st_mode & S_IFDIR )
		{
			return false;
		}
		return true;
	}
	catch(std::exception ex)
	{
	}

	return false;
}

char *realpath(const char *path, char resolved_path[path::max_path_length])
{
  char *return_path = 0;

  if (path) //Else EINVAL
  {
    if (resolved_path)
    {
      return_path = resolved_path;
    }
    else
    {
      //Non standard extension that glibc uses
      return_path = (char*)malloc(path::max_path_length);
    }

    if (return_path) //Else EINVAL
    {
      //This is a Win32 API function similar to what realpath() is supposed to do
      size_t size = GetFullPathNameA(path, path::max_path_length, return_path, 0);

      //GetFullPathNameA() returns a size larger than buffer if buffer is too small
      if (size > path::max_path_length)
      {
        if (return_path != resolved_path) //Malloc'd buffer - Unstandard extension retry
        {
          size_t new_size;

          free(return_path);
          return_path = (char*)malloc(size);

          if (return_path)
          {
            new_size = GetFullPathNameA(path, size, return_path, 0); //Try again

            if (new_size > size) //If it's still too large, we have a problem, don't try again
            {
              free(return_path);
              return_path = 0;
              errno = ENAMETOOLONG;
            }
            else
            {
              size = new_size;
            }
          }
          else
          {
            //I wasn't sure what to return here, but the standard does say to return EINVAL
            //if resolved_path is null, and in this case we couldn't malloc large enough buffer
            errno = EINVAL;
          }  
        }
        else //resolved_path buffer isn't big enough
        {
          return_path = 0;
          errno = ENAMETOOLONG;
        }
      }

      //GetFullPathNameA() returns 0 if some path resolve problem occured
      if (!size) 
      {
        if (return_path != resolved_path) //Malloc'd buffer
        {
          free(return_path);
        }

        return_path = 0;

        //Convert MS errors into standard errors
        switch (GetLastError())
        {
          case ERROR_FILE_NOT_FOUND:
            errno = ENOENT;
            break;

          case ERROR_PATH_NOT_FOUND: case ERROR_INVALID_DRIVE:
            errno = ENOTDIR;
            break;

          case ERROR_ACCESS_DENIED:
            errno = EACCES;
            break;

          default: //Unknown Error
            errno = EIO;
            break;
        }
      }

      //If we get to here with a valid return_path, we're still doing good
      if (return_path)
      {
        struct stat stat_buffer;

        //Make sure path exists, stat() returns 0 on success
        if (stat(return_path, &stat_buffer)) 
        {
          if (return_path != resolved_path)
          {
            free(return_path);
          }

          return_path = 0;
          //stat() will set the correct errno for us
        }
        //else we succeeded!
      }
    }
    else
    {
      errno = EINVAL;
    }
  }
  else
  {
    errno = EINVAL;
  }

  return return_path;
}

path path::real_path () const
{
	char resolved_path[1024];
	realpath(string().c_str(), resolved_path);
	path tmp(&resolved_path[0]);
	tmp.replace(L"\\", L"/");
	return tmp;
}

size_t path::file_size () const
{
	if (is_file())
	{
		std::ifstream in(string().c_str(), std::ios::binary | std::ios::ate);
		return (size_t)in.tellg(); 
	}

	return 0;
}
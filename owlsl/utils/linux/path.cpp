#include <owlsl/utils/path.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
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
	if (is_file())
	{
		try
		{
			FILE *fp = fopen(string().c_str(),"r");
			if( fp )
			{
				return true;
				fclose(fp);
			} else {
				return false;
			}
		}
		catch(std::exception ex)
		{
			return false;
		}
	}

	return is_folder();
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
	try
	{
		struct stat info;
		return ( stat( string().c_str(), &info ) != 0 );
	}
	catch(std::exception ex)
	{
	}
	return false;
}

bool path::is_folder () const
{
	try
	{
		struct stat info;
		int ret = stat( string().c_str(), &info );

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
	try
	{
		struct stat info;
		int ret = stat( string().c_str(), &info );

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

path path::real_path () const
{
	char resolved_path[PATH_MAX];
	realpath(string().c_str(), resolved_path);
	path tmp(&resolved_path[0]);
	return tmp;
}

size_t path::file_size () const
{
	if (is_file())
	{
		std::ifstream in(string().c_str(), std::ios::binary | std::ios::ate);
		return in.tellg();
	}

	return 0;
}

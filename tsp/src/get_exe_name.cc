#include <iostream>


static void print_exe_name(std::string&& name)
{
	size_t ndx = name.find_last_of("/");
	if (ndx != std::string::npos)
	{
		name = name.substr(ndx + 1, std::string::npos);
	}
	
	if (name.at(0) == '.')
	{
		name = name.substr(1, std::string::npos);
	}
	ndx = name.find(".");
	if (ndx != std::string::npos)
	{
		name = name.substr(0, ndx);
	}
	else
	{
		name = name + ".out";
	}
	
	std::cout << name << std::endl;
}

#ifdef HAVE_MAIN

int main(int argc, char **argv)
{
	for (int i=1;i<argc;i++)
	{
		print_exe_name(std::string{argv[i]});
	}
}

#endif

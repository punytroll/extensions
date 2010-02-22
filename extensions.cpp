#include <dirent.h>
#include <sys/stat.h>

#include <deque>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>

enum Mode
{
	COUNT,
	LIST
};

std::map< std::string, unsigned int > g_Counts;
std::map< std::string, std::list< std::string > > g_Lists;
Mode g_Mode(COUNT);
bool g_Pure(false);

void ReadFile(const std::string & Path);
void ReadDirectory(const std::string & Path);
void ReadItem(const std::string & Path);

void ReadFile(const std::string & Path)
{
	std::string File(Path.substr(Path.rfind('/')));
	std::string::size_type ExtensionDot(File.rfind('.'));
	std::string Extension;
	
	if(ExtensionDot != std::string::npos)
	{
		Extension = File.substr(ExtensionDot + 1);
	}
	switch(g_Mode)
	{
	case COUNT:
		{
			g_Counts[Extension]++;
			
			break;
		}
	case LIST:
		{
			if(g_Lists.find(Extension) != g_Lists.end())
			{
				g_Lists[Extension].push_back(Path);
			}
			
			break;
		}
	}
}

void ReadDirectory(const std::string & Path)
{
	DIR * Directory(opendir(Path.c_str()));
	
	if(Directory != 0)
	{
		while(true)
		{
			dirent * DirectoryEntry(readdir(Directory));
			
			if(DirectoryEntry == 0)
			{
				break;
			}
			if((std::string(".") == DirectoryEntry->d_name) || (std::string("..") == DirectoryEntry->d_name))
			{
				continue;
			}
			ReadItem(Path + '/' + DirectoryEntry->d_name);
		}
		closedir(Directory);
	}
}

void ReadItem(const std::string & Path)
{
	struct stat Stat;
	
	if(stat(Path.c_str(), &Stat) == 0)
	{
		if(S_ISDIR(Stat.st_mode))
		{
			ReadDirectory(Path);
		}
		else
		{
			ReadFile(Path);
		}
	}
}

int main(int argc, char ** argv)
{
	std::deque< std::string > Arguments(argv + 1, argv + argc);
	
	if(Arguments.size() == 0)
	{
		std::cerr << "Usage: " << argv[0] << " <paths>" << std::endl;

		return 1;
	}
	while((Arguments.begin() != Arguments.end()) && (Arguments.front().substr(0, 2) == "--"))
	{
		if(Arguments.front().substr(0, 6) == "--pure")
		{
			g_Pure = true;
		}
		if(Arguments.front().substr(0, 7) == "--list=")
		{
			g_Mode = LIST;
			g_Lists[Arguments.front().substr(7)] = std::list< std::string >();
		}
		Arguments.pop_front();
	}
	while(Arguments.begin() != Arguments.end())
	{
		ReadItem(Arguments.front());
		Arguments.pop_front();
	}
	switch(g_Mode)
	{
	case COUNT:
		{
			unsigned int MaxExtensionLength(0);
			
			for(std::map< std::string, unsigned int >::iterator Begin = g_Counts.begin(); Begin != g_Counts.end(); ++Begin)
			{
				if(Begin->first.length() > MaxExtensionLength)
				{
					MaxExtensionLength = Begin->first.length();
				}
			}
			
			std::map< std::string, unsigned int >::iterator Begin(g_Counts.begin());
			
			while(Begin != g_Counts.end())
			{
				std::cout << std::setw(MaxExtensionLength) << std::left << Begin->first << ' ' << Begin->second << std::endl;
				++Begin;
			}
			
			break;
		}
	case LIST:
		{
			for(std::map< std::string, std::list< std::string > >::iterator List = g_Lists.begin(); List != g_Lists.end(); ++List)
			{
				if(g_Pure == false)
				{
					if(List != g_Lists.begin())
					{
						std::cout << '\n';
					}
					std::cout << List->first << ":" << std::endl;
				}
				for(std::list< std::string >::iterator Path = List->second.begin(); Path != List->second.end(); ++Path)
				{
					std::cout << *Path << std::endl;
				}
			}
			
			break;
		}
	}
}

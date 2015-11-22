#ifndef GTKTOOLS_H
#define GTKTOOLS_H

#include <string>
using namespace std;

class GTKTools
{
	public:
		static string get_current_theme_name();
		static string get_open_file_name(const char* title = "Open File", const char* filter_name = "All Files", 
										   const char* filter_pattern = "*");
};

#endif // GTKTOOLS_H

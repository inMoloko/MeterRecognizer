#include <iostream>
#include <string>
#include "Helpers.h"

using std::string;

namespace Helpers
{
	/**
	* Возвращает имя файла по полному пути
	*/
	string getFileName(const string& s) {

		char sep = '/';

#ifdef _WIN32
		sep = '\\';
#endif

		size_t i = s.rfind(sep, s.length());
		if (i != string::npos) {
			return(s.substr(i + 1, s.length() - i));
		}

		return("");
	}
}


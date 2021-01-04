#ifndef _JIGSAW_FILE_UTIL_H_
#define _JIGSAW_FILE_UTIL_H_
#include <sstream>

namespace Jigsaw {
	namespace File_IO {

		/// <summary>
		/// A simple struct that encapsulates some basic information about a file and an active file descriptor. It is used with the 'load'
		/// functions to provide all information that's needed to properly load the given resource.
		/// </summary>
		struct FILE_DATA {
			size_t file_size;
			wchar_t* file_name;
			FILE* file;
		};

		FILE_DATA GetResourceFile(const char* file_path); 

		std::istringstream GetStringStreamFromFile(const FILE_DATA& file_data);

		bool GetFileIfExists(const std::string& file_path, FILE_DATA* out_data);
	}
}
#endif


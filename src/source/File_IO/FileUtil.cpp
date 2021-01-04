#include "FileUtil.h"
#include <algorithm>
#include <string>
#include <filesystem>

namespace Jigsaw {
	namespace File_IO {
		FILE_DATA GetResourceFile(const char* file_path) {
			// load the file needed to initialize the resource
			FILE* resource_file;

			errno_t error = fopen_s(&resource_file, file_path, "rb");


			fseek(resource_file, 0, SEEK_END);
			size_t size = ftell(resource_file);
			fseek(resource_file, 0, SEEK_SET);

			// copying wstr of path for use in FILE_DATA
			std::filesystem::path path(file_path);
			size_t size_wstr = path.wstring().size();
			wchar_t* c_str = new wchar_t[path.wstring().size() + 1];
			path.wstring().copy(c_str, size_wstr, 0);
			c_str[size_wstr] = 0;

			return { size, c_str, resource_file };
		}

		std::istringstream GetStringStreamFromFile(const FILE_DATA& file_data) {
			char* file_read = new char[file_data.file_size + 1];
			file_read[file_data.file_size] = '\0';
			fread_s(file_read, file_data.file_size, file_data.file_size, 1, file_data.file);
			std::string str(file_read);
			delete[] file_read;

			return std::move(std::istringstream(str));
		}

		bool GetFileIfExists(const std::string& file_path, FILE_DATA* out_data) {
			bool exists = std::filesystem::exists(file_path);
			if (exists) {
				*out_data = GetResourceFile(file_path.c_str());
			}
			return exists;
		}

	}
}
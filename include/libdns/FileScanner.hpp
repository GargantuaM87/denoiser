#ifndef FILE_SCANNER_HPP
#define FILE_SCANNER_HPP

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace dns {
	class FileInfo {
		public:
			FileInfo(std::filesystem::path path);

			std::string g_name() const { return name; }
			std::filesystem::path g_path() const { return path; }
			std::uintmax_t g_size() const { return size; }
		private:
			std::string name;
			std::filesystem::path path;
			std::uintmax_t size = 0;
	};
}

#endif 

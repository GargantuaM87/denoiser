#ifndef FILE_SCANNER_HPP
#define FILE_SCANNER_HPP

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <vector>

namespace dns {
	struct FileInfo {
		std::string name;
		std::filesystem::path path;
		std::uintmax_t size = 0;
		bool isDirectory;
		std::vector<FileInfo> children;

		FileInfo(std::string name, std::filesystem::path path, bool isDirectory, std::size_t size = 0) : name(name), path(path), size(size), isDirectory(isDirectory) {}
	};
}

#endif 

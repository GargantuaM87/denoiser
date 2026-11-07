#ifndef FILE_SCANNER_HPP
#define FILE_SCANNER_HPP

#include <cstddef>
#include <filesystem>

namespace dns {
	struct FileInfo {
		std::filesystem::path path;
		std::size_t size;

		FileInfo(std::filesystem::path path, std::size_t size) : path(path), size(size) {}
	};
}

#endif 

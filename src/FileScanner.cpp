#include "../include/libdns/FileScanner.hpp"
#include <filesystem>

dns::FileInfo::FileInfo(std::filesystem::path path) {
	name = path.filename();
	this->path = path;
	size = std::filesystem::file_size(path);	
}

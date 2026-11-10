#include "../include/libdns/FileScanner.hpp"
#include "../include/libdns/Error.hpp"
#include <filesystem>

dns::FileInfo::FileInfo(std::filesystem::path path) {
	if(!std::filesystem::exists(path)) {
		dns::Error::send("This path does not exist!");
	}
	name = path.filename();
	this->path = path;
	size = std::filesystem::file_size(path);	
}

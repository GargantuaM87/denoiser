#include "../include/libdns/DirectoryNode.hpp"
#include <cstdint>
#include <filesystem>
#include <vector>

dns::DirectoryNode::DirectoryNode(std::filesystem::path path) {
	this->path = path;
	name = path.filename();

	size = aggregate_directory();
}

uintmax_t dns::DirectoryNode::aggregate_directory() {
	std::uintmax_t directorySize = 0;

	for(const auto& entry : std::filesystem::directory_iterator{path}) {
		if(entry.is_regular_file()) {
			size += entry.file_size();
			files.push_back({entry.path()}); 
		}
		else if(entry.is_directory()) {
			dns::DirectoryNode directory{entry.path()};
			subdirectories.push_back(directory);
		}
	}
	size = directorySize;
	return directorySize;	
}

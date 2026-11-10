#include "../include/libdns/DirectoryNode.hpp"
#include "../include/libdns/Error.hpp"
#include <cstdint>
#include <filesystem>
#include <vector>

dns::DirectoryNode::DirectoryNode(std::filesystem::path path) {
	if(!std::filesystem::exists(path))
   	dns::Error::send("This path does not exist!");

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

void dns::DirectoryNode::set_dir(const dns::DirectoryNode&& other) {
	this->name = other.name;
	this->path = other.path;
	this->size = other.size;
	this->subdirectories = other.subdirectories;
	this->files = other.files;
}

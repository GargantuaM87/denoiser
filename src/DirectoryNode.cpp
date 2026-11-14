#include "../include/libdns/DirectoryNode.hpp"
#include "../include/libdns/Error.hpp"
#include <cstdint>
#include <filesystem>
#include <set>
#include <system_error>
#include <tuple>
#include <vector>
#include <unistd.h>

dns::FileInfo::FileInfo(std::filesystem::path path) {
	if(!std::filesystem::exists(path)) {
		dns::Error::send("This path does not exist!");
	}
	name = path.filename();
	this->path = path;

	if(std::filesystem::is_directory(path))
		isDirectory = true;	

	size = isDirectory ? 0 : std::filesystem::file_size(path);
}

dns::DirectoryNode::DirectoryNode(std::filesystem::path path) {
	if(!std::filesystem::exists(path))
   	dns::Error::send("This path does not exist!");
	if(access(path.c_str(), R_OK) != 0)
		dns::Error::send_errno("Has no read access!");
	if(!std::filesystem::is_directory(path))
		dns::Error::send("This path is not a directory!");

	this->path = path;
	this->name = path.filename();
	this->size = aggregate_directory();

	flatline_contents();
}

dns::DirectoryNode::DirectoryNode(std::filesystem::path path, std::set<std::string> ignoreSet) {
	if(!std::filesystem::exists(path))
   	dns::Error::send("This path does not exist!");
	if(access(path.c_str(), R_OK) != 0)
		dns::Error::send_errno("Has no read access!");

	this->path = path;
	this->name = path.filename();
	this->ignoreSet = ignoreSet;
	this->size = aggregate_directory();

	flatline_contents();
}

uintmax_t dns::DirectoryNode::aggregate_directory() {
	std::uintmax_t directorySize = 0;
	std::error_code ec;

	for(const auto& entry : std::filesystem::directory_iterator{path, std::filesystem::directory_options::skip_permission_denied, ec}) {
		if(ec) {
			dns::Error::send("Something went wrong: " + ec.message() + "\n");
		}

		if(ignoreSet.find(entry.path().filename()) != ignoreSet.end())
			continue;

		else if(entry.is_regular_file()) {
			directorySize += entry.file_size();
			files.push_back({entry.path()}); 
		}
		else if(entry.is_directory()) {
			dns::DirectoryNode directory{entry.path()};
			subdirectories.push_back(directory);
			directorySize += directory.g_size();
		}
	}
	return directorySize;	
}

void dns::DirectoryNode::flatline_contents() {
	std::error_code ec;

	for(const auto& entry : std::filesystem::recursive_directory_iterator{path, std::filesystem::directory_options::skip_permission_denied, ec}) {
		if(ec)
			dns::Error::send("Something went wrong: " + ec.message() + "\n");

		if(ignoreSet.find(entry.path().filename()) != ignoreSet.end())
			continue;

		else if(entry.is_regular_file()) {
			num_files += 1;
		}
		else if(entry.is_directory()) {
			num_directories += 1;
		}
		children.push_back({entry.path()});
	}
}

void dns::DirectoryNode::set_dir(const dns::DirectoryNode&& other) {
	this->name = other.name;
	this->path = other.path;
	this->size = other.size;
	this->subdirectories = other.subdirectories;
	this->files = other.files;
}

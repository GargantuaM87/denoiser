#ifndef DIRECTORY_NODE_HPP
#define DIRECTORY_NODE_HPP

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>
#include "./FileScanner.hpp"


namespace dns {
	struct DirectoryNode {
		std::string name;
		std::filesystem::path path;
		std::uintmax_t size;
		std::vector<DirectoryNode> subdirectories;
		std::vector<FileInfo> files;

		DirectoryNode(std::string name, std::filesystem::path path, std::uintmax_t size = 0) : name(name), path(path), size(size) {}
		
	};

}

#endif

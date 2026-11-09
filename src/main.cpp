#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <filesystem>
#include <iterator>
#include <set>
#include <string>
#include <tuple>
#include <vector>
#include "../include/libdns/FileScanner.hpp"
#include "../include/libdns/DirectoryNode.hpp"


void inspectDirectory(std::filesystem::path filePath) {
	if(!std::filesystem::exists(filePath) or !std::filesystem::is_directory(filePath)) return;
	std::vector<dns::FileInfo> files;
	std::uint32_t regular_files = 0;
	std::uint32_t directories = 0;

	for(const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator{filePath}) {
		if(entry.is_regular_file()) {
			//dns::FileInfo file{entry.path(), entry.file_size()};
			//files.push_back(file);
			regular_files++;
		}
		else if(entry.is_directory())
			directories++;	
	}

	std::cout << "The Directory " << std::filesystem::absolute(filePath) << " has " << regular_files << " files and " << directories << " subdirectories.\n";
   std::cout << "There is a total of " << regular_files + directories << " entries.";
}

uintmax_t aggregateDirectory(dns::DirectoryNode& node, std::filesystem::path path) {
	// base cases
	if(!std::filesystem::exists(path) or std::filesystem::is_empty(path))
		return 0;
	
	uintmax_t directorySize = 0;
	std::vector<dns::DirectoryNode> subdir;
	std::vector<dns::FileInfo> files;
	for(const auto& entry : std::filesystem::directory_iterator{path}) {
		if(entry.is_regular_file()) {
			directorySize += entry.file_size();
			files.push_back({entry.path().filename(), entry.path(), entry.file_size()});
		}
		else if(entry.is_directory()) {
			dns::DirectoryNode directory{entry.path().filename(), entry.path()};
			directorySize += aggregateDirectory(directory, entry.path());
			directory.size = directorySize;
			subdir.push_back(directory);
		}
	}
	node.subdirectories = std::move(subdir);
	node.files = std::move(files);
  	return directorySize;	
}

void buildTree(const dns::DirectoryNode& root, std::string prefix, std::set<std::string> ignoreSet) {

	std::cout << prefix << root.name << std::endl;
	std::string tmprefix = prefix;
	tmprefix = tmprefix.substr(0, tmprefix.size() - 3);
	prefix = tmprefix + "   ";
	for(const auto& entry : root.subdirectories) {
		if(ignoreSet.find(entry.name) != ignoreSet.end()) continue;
		buildTree(entry, prefix + "|- ", ignoreSet);
	}
}

int main(int args, const char** argv) {
	if(args < 2) return 1;

	std::filesystem::path path = argv[1]; 

	if(!std::filesystem::exists(path)) {
		std::cout << "This file does not exist!\n";
		return 1;
	}

	std::cout << "Path: " << std::filesystem::absolute(path) << "\n";

	if(std::filesystem::is_regular_file(path)) {
		std::cout << "Size: " 
					 << std::filesystem::file_size(path)
					 << " bytes\n";
	}
	//inspectDirectory(path);
	dns::DirectoryNode rootDir{path.filename(), path};
	std::set<std::string> ignoreSet{".git", "build"};
	rootDir.size = aggregateDirectory(rootDir, path);
	buildTree(rootDir, "", ignoreSet);
	/*std::cout << rootDir.size << "\n";

	for(const auto& dir : rootDir.subdirectories) {
		std::cout << dir.name << "\n";
		std::cout << dir.size << "\n";
	}*/
}



#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <filesystem>
#include <iterator>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>
#include <editline/readline.h>
#include "../include/libdns/FileScanner.hpp"
#include "../include/libdns/DirectoryNode.hpp"
#include "../include/libdns/Error.hpp"

namespace {
	bool is_prefix(std::string_view str, std::string_view of) {
		if(str.size() > of.size()) return false;

		return std::equal(str.begin(), str.end(), of.begin());
	}

	std::vector<std::string> split(std::string_view str, char delimiter) {
		std::vector<std::string> out {};
		std::stringstream ss {std::string{str}};
		std::string item;

		while(std::getline(ss, item, delimiter)) {
			out.push_back(item);
		}
		return out;
	}
}

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

void build_tree(const dns::DirectoryNode& root, std::string prefix, std::set<std::string> ignoreSet) {

	std::cout << prefix << root.name << std::endl;
	std::string tmprefix = prefix;
	tmprefix = tmprefix.substr(0, tmprefix.size() - 3);
	prefix = tmprefix + "   ";
	for(const auto& entry : root.subdirectories) {
		if(ignoreSet.find(entry.name) != ignoreSet.end()) continue;
		std::cout << prefix + "|" << std::endl;
		build_tree(entry, prefix + "|- ", ignoreSet);
	}

	for(const auto& entry : root.files) {
		if(ignoreSet.find(entry.name) != ignoreSet.end()) continue;
		std::cout << prefix + "|- " << entry.name << std::endl;
	}
}

void build_tree_command(std::vector<std::string>& args) {
	if(args.size() < 2) {
		return;
	}
	std::filesystem::path path = args[1];
	
	if(!std::filesystem::exists(path)) {
		dns::Error::send("This path does not exist!");
	}

	dns::DirectoryNode rootDir{path.filename(), path};
	std::set<std::string> ignoreSet{".git", "build"};
	rootDir.size = aggregateDirectory(rootDir, path);
	build_tree(rootDir, "", ignoreSet);
}


void handle_command(std::string_view line) {
	auto args = split(line, ' ');
	auto command = args[0];

	if(is_prefix(command, "tree")) {
		build_tree_command(args);
	}
}

void main_loop() {
	char* line = nullptr;
	while ((line = readline("dns> ")) != nullptr) {
		std::string line_str;

		if(line == std::string_view("")) {
			free(line);
			if(history_length > 0) {
				line_str = history_list()[history_length - 1]->line;
			}
		}
		else {
			line_str = line;
			add_history(line);
			free(line);
		}
		if(!line_str.empty()) {
			try {
				handle_command(line_str);
			}
			catch(const dns::Error& err) {
				std::cout << err.what() << std::endl;
			}
		}
	}
}

int main(int args, const char** argv) {
	if(args == 1) {
		try {
			main_loop();
		}	
		catch(const dns::Error& err) {
			std::cout << err.what() << std::endl;
		}
	}
	return 0;
}






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

void build_tree(const dns::DirectoryNode& root, std::string prefix, std::set<std::string> ignoreSet) {

	std::cout << prefix << root.g_name() << std::endl;
	std::string tmprefix = prefix;
	tmprefix = tmprefix.substr(0, tmprefix.size() - 3);
	prefix = tmprefix + "   ";
	for(const auto& entry : root.g_subdirectories()) {
		if(ignoreSet.find(entry.g_name()) != ignoreSet.end()) continue;
		std::cout << prefix + "|" << std::endl;
		build_tree(entry, prefix + "|- ", ignoreSet);
	}

	for(const auto& entry : root.g_files()) {
		if(ignoreSet.find(entry.g_name()) != ignoreSet.end()) continue;
		std::cout << prefix + "|- " << entry.g_name() << std::endl;
	}
}

void build_tree_command(dns::DirectoryNode& dir, std::vector<std::string>& args) {
	if(args.size() > 1) {
		return;
	}

	std::set<std::string> ignoreSet{".git", "build", "CMakeFiles"};
	build_tree(dir, "", ignoreSet);
}

void set_directory_command(dns::DirectoryNode& dir, std::vector<std::string>& args) {
	if(args.size() == 2) {
		std::filesystem::path path = args[1];

		if(!std::filesystem::exists(path) or !std::filesystem::is_directory(path))
			dns::Error::send("Invalid path!");

		dns::DirectoryNode tempDir{path};
	   dir.set_dir(std::move(tempDir));	
	}
}


void handle_command(dns::DirectoryNode& dir, std::string_view line) {
	auto args = split(line, ' ');
	auto command = args[0];

	if(is_prefix(command, "tree")) {
		build_tree_command(dir, args);
	}
	if(is_prefix(command, "setdir")) {
		set_directory_command(dir, args);
	}
}

void main_loop(dns::DirectoryNode& dir, const char** argv) {
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
				handle_command(dir, line_str);
			}
			catch(const dns::Error& err) {
				std::cout << err.what() << std::endl;
			}
		}
	}
}

int main(int args, const char** argv) {
	if(args == 2) {
		try {
			dns::DirectoryNode dir{argv[1]};
			main_loop(dir, argv);
		}	
		catch(const dns::Error& err) {
			std::cout << err.what() << std::endl;
		}
	}
	return 0;
}






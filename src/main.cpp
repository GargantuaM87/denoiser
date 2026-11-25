#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <ios>
#include <iostream>
#include <filesystem>
#include <iterator>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>
#include <iomanip>
#ifdef DNS_USE_READLINE
#include <readline/history.h>
#include <readline/readline.h>
#else
#include <editline/readline.h>
#endif
#include <fmt/format.h>
#include <fmt/ranges.h>
#include "../include/libdns/FileScanner.hpp"
#include "../include/libdns/DirectoryNode.hpp"
#include "../include/libdns/Error.hpp"



void main_loop(dns::DirectoryNode& dir) {
	char* line = nullptr;
	dns::FileScanner fs{};
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
				fs.handle_command(dir, line_str);
			}
			catch(const dns::Error& err) {
				std::cout << err.what() << std::endl;
			}
		}
	}
}

int main(int args, const char** argv) {
	if(args >= 2) {
		try {
			// Make it so the user can add in their own arguments later for the ignore vector
			std::set<std::string> ignoreSet;
			if(args > 2) {
				for(int i = 2; i < args; i++) {
					ignoreSet.insert(argv[i]);
				}
			}
			dns::DirectoryNode dir{argv[1], ignoreSet};
			main_loop(dir);
		}	
		catch(const dns::Error& err) {
			std::cout << err.what() << std::endl;
		}
	}
	return 0;
}




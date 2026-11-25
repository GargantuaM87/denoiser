#include "../include/libdns/FileScanner.hpp"
#include "../include/libdns/Error.hpp"
#include "../include/libdns/DirectoryNode.hpp"
#include "../include/libdns/Timer.hpp"
#include <cctype>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fmt/format.h>
#include <ios>
#include <iostream>
#include <string>
#include <string_view>
#include <sys/types.h>
#include <unordered_map>
#include <algorithm>
#include <vector>

#define COLORS\
	X(BLACK, 30)\
	X(RED, 31)\
	X(GREEN, 32)\
	X(YELLOW, 33)\
	X(BLUE, 34)\
	X(PURPLE, 35)\
	X(CYAN, 36)\
	X(WHITE, 37)\

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
	enum Colors : uint8_t {
		#define X(value, a) value = (a), 
			COLORS
		#undef X
	};

	std::string ansi(std::string text, int color, int bold = 0) {
		std::string res = fmt::format("\e[{};{}m{}\e[0m", bold, color, text);
		return res;	
	}

	std::string humanize_size(std::uintmax_t bytes) {
		const char* units[] = {"B", "KB", "MB", "GB", "TB"};
		int i = 0;
		double size = static_cast<double>(bytes);

		while(size >= 1000.0 && i < 4) {
			size /= 1000.0;
			++i;
		}

		std::ostringstream ss;
		ss << std::fixed << std::setprecision(i == 0 ? 0 : 1) << size << ' ' << units[i];
		return ss.str();
	}

	std::uintmax_t byte_size(const std::string_view& str) {
		std::unordered_map<std::string, std::uint8_t> conversionMap = {{"B", 0}, {"KB", 1}, {"MB", 2}, {"GB", 3}, {"TB", 4}};

		std::string size = "";
		std::string denomination = "";

		for(char c : str) {
			if(std::isdigit(c))
				size += c;
			else if(std::isalpha(c))
				denomination += std::toupper(c);
		}
		if(conversionMap.find(denomination) == conversionMap.end())
			dns::Error::send("Enter a valid memory denomination");	

		std::uintmax_t result = std::stoi(size);
		result = result * std::pow(1000, conversionMap[denomination]);
		return result;
	} 
}

void print_help(const std::vector<std::string>& args) {
	if(args.size() == 1) {
		std::cerr << R"(Available Commands:
	tree - Commands for visualizing a directory tree
	setdir - Commands for changing the current operating directory 
	search - Commands for finding and filtering for specific file information
)";
	}
	else if(is_prefix(args[1], "tree")) {
		std::cerr << R"(Available Commands:
	tree - output directory in tree format
	tree <option>
	
	-h, --human
	output directory tree with human readable sizes
)";
	}
	else if(is_prefix(args[1], "setdir")) {
		std::cerr << R"(Available Commands:
	setdir - change the current operating directory
	setdir <args>
)";
	}
	else if(is_prefix(args[1], "search")) {
		std::cerr << R"(Available Commands:
	search - find and filter files and directories
	search <option> <args>

	-l, --largest <n>
	output n largest files and directories

	-l --dir <n>
	filter for directories

	-l --file <n>
	filter for files

	--min-size <N>B, <N>KB, <N>MB, <N>GB, or <N>TB 
	filter for files that are equal to or greater than N

	--extension <s>
	filter for files or directories with the file extension s
)";
	}
	else {
		std::cerr << "No help available on that\n";
	}
}


void build_tree(const dns::DirectoryNode& root, std::string prefix, bool human) {
	std::cout << prefix << ansi(root.g_name(), BLUE, 1) << "   " << ansi((human ? humanize_size(root.g_size()) : ""), WHITE, 1) << std::endl;
	std::string tmprefix = prefix;
	tmprefix = tmprefix.substr(0, tmprefix.size() - 3);
	prefix = tmprefix + "   ";

	for(const auto& entry : root.g_subdirectories()) {
		std::cout << prefix + "|" << std::endl;
		build_tree(entry, prefix + "|- ", human);
	}

	for(const auto& entry : root.g_files()) {		
		std::cout << prefix + "|- " << entry.g_name() << "  "  << ansi((human ? humanize_size(entry.g_size()) : ""), WHITE, 1) << std::endl;	
	}
}

void build_tree_command(dns::DirectoryNode& dir, std::vector<std::string>& args) {
	dns::Timer timer;	
	if(args.size() == 1) {
		build_tree(dir, "", false);
	}

	else if(is_prefix(args[1], "--human") or is_prefix(args[1], "-h")) {
		build_tree(dir, "", true);
	}

	else {
		print_help({"help", "tree"});
		return;
	}
}

void set_directory_command(dns::DirectoryNode& dir, std::vector<std::string>& args) {
	if(args.size() >= 2) {
		std::filesystem::path path = args[1];

		std::set<std::string> ignoreSet;
			if(args.size() > 2) {
				for(int i = 2; i < args.size(); i++) {
					ignoreSet.insert(args[i]);
				}
			}
	   dir = dns::DirectoryNode{path, ignoreSet};
	}
	else {
		print_help({"help", "setdir"});
	}
}
// search --largest --file 10; search -l --dir 5
// search --min-size 100MB
// search --extenstion .o 
void search_command(dns::DirectoryNode& dir, std::vector<std::string>& args) {
	std::vector<dns::FileInfo> files{dir.g_children()};	
	auto help = []() {
		print_help({"help", "search"});
	};

	auto print_files = [](const dns::FileInfo& file) {
		std::cout <<  ansi(file.g_name(), BLUE, 1) << ": " << ansi(humanize_size(file.g_size()), WHITE, 1) << std::endl;
	};

	if(args.size() == 3 or args.size() == 4) {
		if(is_prefix(args[1], "--largest") or is_prefix(args[1], "-l")) {
			std::uint8_t limit = 0;
			bool filterDir = false;
			bool filterFile = false;

			if(args.size() == 3) {
				limit = std::atoi(args[2].c_str());		
			}

			else {
				limit = std::atoi(args[3].c_str());
				
				if(is_prefix(args[2], "--dir")) {
					filterDir = true;
				}
				else if(is_prefix(args[2], "--file")) {
					filterFile = true;
				}
				else {
					help(); 
					return;
				}
			}
			// If the user enters a number more than the list's size, then just print out the maximum amount of contents	
			if(limit > files.size()) limit = files.size();

			std::sort(files.begin(), files.end(), [](const dns::FileInfo& a, const dns::FileInfo& b) {
						return a.g_size() > b.g_size();
					});

			if(filterDir) {
				std::stable_partition(files.begin(), files.end(), [](const dns::FileInfo& file) {
						return file.g_is_directory();
						});
			}

			else if(filterFile) {
				std::stable_partition(files.begin(), files.end(), [](const dns::FileInfo file) {
						return !file.g_is_directory();
						});
			}
			
			std::cout << "--------------------------------\n";
			for(int i = 0; i < limit; i++) {
				dns::FileInfo& file = files[i];

				if(filterDir and !file.g_is_directory())
					continue;
				if(filterFile and file.g_is_directory())
					continue;

				print_files(file);				
			}
			std::cout <<  "--------------------------------\n";		
		}
		if(is_prefix(args[1], "--min-size")) {
			if(args.size() > 3) {
				help(); 
				return;
			}

			std::string_view size = args[2];
			std::uintmax_t byteLimit = byte_size(size);
			
			std::cout << "--------------------------------\n";
			for(const dns::FileInfo& file : files) {
				if(file.g_size() >= byteLimit)
					print_files(file);
			}
			std::cout << "--------------------------------\n";
		}
		if(is_prefix(args[1], "--extension")) {
			if(args.size() > 3) {
				help(); 
				return;
			}

			std::string extension = args[2];

			std::cout << "--------------------------------\n";
			for(const dns::FileInfo& file : files) {
				int index = file.g_name().find(".");
				if(index == std::string::npos) continue;

				std::string substr = file.g_name().substr(index);
				if(substr.compare(extension) == 0) {
					print_files(file);
				}
			}
			std::cout << "--------------------------------\n";
		}
	}
	else {
		help();
		return;
	}	
}

void test_command(dns::DirectoryNode& dir) {
	std::vector<dns::FileInfo> files = dir.g_children();

	for(const auto& file : files) {
		std::cout << "Name: " + file.g_name() << " Size: " << file.g_size() << "Is Directory? " << file.g_is_directory() << std::endl;
	}
}

void dns::FileScanner::handle_command(dns::DirectoryNode& dir, std::string_view line) {
	auto args = split(line, ' ');
	auto command = args[0];

	if(is_prefix(command, "tree")) {
		build_tree_command(dir, args);
	}
	else if(is_prefix(command, "setdir")) {
		set_directory_command(dir, args);
	}
	else if(is_prefix(command, "search")) {
		search_command(dir, args);
	}
	else if(is_prefix(command, "test")) {
		test_command(dir);
	}
	else if(is_prefix(command, "help")) {
		print_help({args});
	}
}


#include "../include/libdns/FileScanner.hpp"
#include "../include/libdns/Error.hpp"
#include "../include/libdns/DirectoryNode.hpp"
#include <cstdint>
#include <filesystem>
#include <fmt/format.h>
#include <iterator>
#include <set>
#include <iostream>
#include <string_view>
#include <utility>

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
}

void print_help(const std::vector<std::string>& args) {
	if(args.size() == 1) {
		std::cerr << R"(Available Commands:
	tree - Commands for visualizing a directory tree
	setdir - Commands for changing the current operating directory 
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
	
	std::cout << "This directory has " << dir.g_num_directories() << " subdirectories, and " << dir.g_num_files() << " files.\n";	
}

void set_directory_command(dns::DirectoryNode& dir, std::vector<std::string>& args) {
	if(args.size() == 2) {
		std::filesystem::path path = args[1];

		dns::DirectoryNode tempDir{path};
	   dir.set_dir(std::move(tempDir));	
	}
	else {
		print_help({"help", "setdir"});
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
	else if(is_prefix(command, "help")) {
		print_help({args});
	}
}


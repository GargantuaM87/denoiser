#ifndef DIRECTORY_NODE_HPP
#define DIRECTORY_NODE_HPP

#include <cstdint>
#include <filesystem>
#include <string>
#include <sys/types.h>
#include <vector>
#include "./FileScanner.hpp"


namespace dns {
	class DirectoryNode {
		public:
			DirectoryNode(std::filesystem::path path);

			uintmax_t aggregate_directory();

		   std::string g_name() const { return name; }
		   std::filesystem::path g_path() const { return path; }	
			std::uintmax_t g_size() const { return size; }
			
			std::vector<DirectoryNode>& g_subdirectories() { return subdirectories; }
			const std::vector<DirectoryNode>& g_subdirectories() const { return subdirectories; }

			std::vector<FileInfo>& g_files() { return files; }
			const std::vector<FileInfo>& g_files() const { return files; }
		private:
			std::string name;
			std::filesystem::path path;
			std::uintmax_t size = 0;
			std::vector<DirectoryNode> subdirectories;
			std::vector<FileInfo> files;
	};

}

#endif

#ifndef DIRECTORY_NODE_HPP
#define DIRECTORY_NODE_HPP

#include <cstdint>
#include <filesystem>
#include <set>
#include <string>
#include <sys/types.h>
#include <vector>


namespace dns {
	class FileInfo {
		public:
			FileInfo(std::filesystem::path path);

			std::string g_name() const { return name; }
			std::filesystem::path g_path() const { return path; }
			std::uintmax_t g_size() const { return size; }
			bool g_is_directory() const { return isDirectory; }
		private:
			std::string name;
			std::filesystem::path path;
			std::uintmax_t size = 0;
			bool isDirectory = false;
	};

	class DirectoryNode {
		public:
			DirectoryNode(std::filesystem::path path);
			DirectoryNode(std::filesystem::path path, std::set<std::string> ignoreSet);	

			void set_dir(const DirectoryNode&& other);

		   std::string g_name() const { return name; }
		   std::filesystem::path g_path() const { return path; }	
			std::uintmax_t g_size() const { return size; }
			
			std::vector<DirectoryNode>& g_subdirectories() { return subdirectories; }
			const std::vector<DirectoryNode>& g_subdirectories() const { return subdirectories; }

			std::vector<FileInfo>& g_files() { return files; }
			const std::vector<FileInfo>& g_files() const { return files; }

			std::uint16_t g_num_directories() const { return num_directories; }
			std::uint16_t g_num_files() const { return num_files; }

			std::vector<FileInfo>& g_children() { return children; }
			const std::vector<FileInfo>& g_children() const { return children; }

		private:
			std::string name;
			std::filesystem::path path;
			std::uintmax_t size = 0;
			std::vector<DirectoryNode> subdirectories;
			std::vector<FileInfo> files;
			std::set<std::string> ignoreSet;

			std::vector<FileInfo> children;

			std::uint16_t num_directories = 0;
			std::uint16_t num_files = 0;

			uintmax_t aggregate_directory();
			void flatline_contents();

	};

}

#endif

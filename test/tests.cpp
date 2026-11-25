#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <gtest/gtest.h>
#include <sys/types.h>
#include "../include/libdns/DirectoryNode.hpp"
#include "../include/libdns/Error.hpp"

TEST(DirectoryNodeTest, EmptyDirectory) {
	if(mkdir("./testdir", 0777) == -1)
		std::cerr << "Error: " << strerror(errno) << std::endl;

	dns::DirectoryNode emptyDir{std::filesystem::path{"./testdir"}};
	EXPECT_EQ(emptyDir.g_size(), 0);
}

TEST(DirectoryNodeTest, NestedDirectorySize) {
	dns::DirectoryNode testNode{"/home/ayinde-abrams/Documents/Denoiser/include/"};

	std::uintmax_t totalSize = 0;

	for(const auto& file : testNode.g_files()) {
		totalSize += file.g_size();
	}

	for(const auto& file : testNode.g_subdirectories()) {
		totalSize += file.g_size();
	}

	EXPECT_EQ(totalSize, testNode.g_size());
}

TEST(DirectoryNodeTest, MissingDirectory) {
	EXPECT_THROW(dns::DirectoryNode{"thisIsFake/"}, dns::Error);
}

TEST(DirectoryNodeTest, UnreadableDirectory) {
	EXPECT_THROW(dns::DirectoryNode{"/root"}, dns::Error);
}

TEST(FileInfoTest, SizeMatches) {
	std::filesystem::path path{"/home/ayinde-abrams/Documents/Denoiser/CMakeLists.txt"};

	dns::FileInfo fInfo{path};

	EXPECT_EQ(std::filesystem::file_size(path), fInfo.g_size());
}

TEST(FileInfoTest, EmptyFile) {
	std::ofstream emptyFile{"./empty.txt"};
	std::filesystem::path path{"./empty.txt"};

	dns::FileInfo emptyFileInfo{path};

	EXPECT_EQ(emptyFileInfo.g_size(), 0);
}


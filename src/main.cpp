#include <cstdint>
#include <cstdio>
#include <iostream>
#include <filesystem>

void inspectDirectory(std::filesystem::path filePath) {
	if(!std::filesystem::exists(filePath) or !std::filesystem::is_directory(filePath)) return;

	std::uint32_t regular_files = 0;
	std::uint32_t directories = 0;
	for(const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator{filePath}) {
			if(entry.is_regular_file())
				regular_files++;
			else if(entry.is_directory())
				directories++;
			//std::cout << entry.path();
	}

	std::cout << "The Directory " << std::filesystem::absolute(filePath) << " has " << regular_files << " files and " << directories << " subdirectories.\n";
   std::cout << "There is a total of " << regular_files + directories << " entries.";
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
	inspectDirectory(path);
}



#include <iostream>
#include <filesystem>

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
}

#ifndef FILE_SCANNER_HPP
#define FILE_SCANNER_HPP

#include "DirectoryNode.hpp"
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace dns {	
	class FileScanner {
		public:
			FileScanner() = default;

			void handle_command(DirectoryNode& dir, std::string_view line);	
	};
}

#endif 

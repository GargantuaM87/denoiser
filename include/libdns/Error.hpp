#ifndef ERROR_HPP
#define ERROR_HPP

#include <cstring>
#include <stdexcept>
namespace dns {
	class Error : public std::runtime_error {
		public:
			[[noreturn]]
			static void send(const std::string& desc) { throw Error(desc); }

			static void send_errno(const std::string& prefix) {
				throw Error(prefix + ": " + std::strerror(errno));
			}
		private:
			Error(const std::string& desc) : std::runtime_error(desc) {}
	};
}

#endif

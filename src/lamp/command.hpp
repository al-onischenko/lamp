#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <iostream>
#include <iomanip>
#include <memory>
#include <vector>

namespace lamp {
	struct CommandHeader {
		unsigned char command;
		unsigned short length;

		void Print() {
			std::cout << "Header command = 0x"
				<< std::hex << std::setfill('0') << std::setw(2)
				<< static_cast<unsigned> (command) << std::dec
				<< ", length = " << length << std::endl;
		}
	};

	class Lamp;

	class Command {
	public:
		using Ptr = std::shared_ptr<Command>;
		using Func = void (Lamp::*)(std::vector<char>);

		explicit Command(unsigned short length, Func func): length_(length), func_(func) {
		}
		bool CheckLength(const unsigned short length) const { return length_ == length; }
		void Run(Lamp* lamp, const std::vector<char> buffer) const { ((*lamp).*func_)(buffer); }

	private:
		unsigned short length_;
		Func func_;
	};
}; // namespace lamp
#endif // COMMAND_HPP

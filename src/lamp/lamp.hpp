#ifndef LAMP_HPP
#define LAMP_HPP

#include "../channel.hpp"
#include "command.hpp"
#include <map>

namespace lamp {
	class Color {
		union {
			char channel[4];
			long value;
		} color_;

	public:
		Color() {
			color_.value = 0;
		}
		Color(char r, char g, char b) { Set(r, g, b); }

		void Set(char r, char g, char b) {
		       color_.channel[0] = r;	
		       color_.channel[1] = g;	
		       color_.channel[2] = b;	
		       color_.channel[3] = 0;
		}
		long Get() { return color_.value; }
	};

	class Lamp {
		bool state_; // On/Off
		Color color_;
		Channel::Ptr channel_;
		std::map<unsigned char, Command::Ptr> commands_;

	public:
		explicit Lamp(Channel::Ptr channel): channel_(channel) {
			commands_[0x12] = std::make_shared<Command>(Command(0, &Lamp::SetOn));
			commands_[0x13] = std::make_shared<Command>(Command(0, &Lamp::SetOff));
			commands_[0x20] = std::make_shared<Command>(Command(3, &Lamp::SetColor));
		}
		void PrintState() {
			std::cout << "Lamp " << (state_?"On":"Off") << ",\tcolor=0x"
				<< std::hex << std::setfill('0') << std::setw(6)
				<< color_.Get() << std::dec << std::endl;
		}
		void SetOn(std::vector<char>) { state_ = true; }
		void SetOff(std::vector<char>) { state_ = false; }
		void SetColor(std::vector<char> buf) { color_.Set(buf[0], buf[1], buf[2]); }

		void Run() {
			std::vector<char> buffer;
			while(true) {
				buffer.resize(3);
				size_t size = channel_->Receive(buffer);
				//std::cout << "Receive header size = " << size << std::endl;
				if (size != 3) { throw SocketError("Receive invalid command header"); }
				CommandHeader header;
				header.command = buffer[0];
				header.length = (buffer[1] << 8) + buffer[2]; // ntohs(header.length);
				//header.Print();

				if (header.length) {
					buffer.resize(header.length);
					size = channel_->Receive(buffer);
					//std::cout << "Receive data size = " << size << std::endl;
					if (size != header.length) { throw SocketError("Receive invalid data size"); }
				}
				if (auto cmd = commands_.find(header.command); cmd != commands_.end()) {
					//std::cout << "Command found" << std::endl;
					if (cmd->second->CheckLength(header.length)) {
						cmd->second->Run(this, buffer);
						PrintState();
					}
					else {
						//std::cout << "Command length invalid" << std::endl;
					}
				} 
				else {
					//std::cout << "Command not found" << std::endl;
				}
			 } // while(true)
		}
	};
}; //namespace lamp

#endif //LAMP_HPP

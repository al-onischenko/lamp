#ifndef LAMP_SERVER_HPP
#define LAMP_SERVER_HPP

#include "../channel.hpp"
#include <mutex>
#include <thread>
#include <iomanip>
#include <sstream>

namespace lamp {
	constexpr auto DELAY = 3000;

	struct Logger {
		static void Log(const std::vector<char>& buffer) {
			std::stringstream ss;
			ss << "id " << std::this_thread::get_id() << ", 0x" << std::hex << std::setfill('0');
			for(auto i : buffer) {
				ss << std::setw(2) << static_cast<short>(i) << " ";
			}
			ss << std::dec << std::endl;

			static std::mutex mux;
			std::lock_guard<std::mutex> lg(mux);
			std::cout << ss.str();
		}
	};

	class LampServer {
		Channel::Ptr channel_;
		std::vector<std::vector<char> > commands_;

	public:
		explicit LampServer(Channel::Ptr channel): channel_(channel) {
			commands_.emplace_back(std::vector<char>{0x12, 0, 0});
			commands_.emplace_back(std::vector<char>{0x20, 0, 3, static_cast<char>(0xFF), 0, 0});
			commands_.emplace_back(std::vector<char>{0x13, 0, 0});
			commands_.emplace_back(std::vector<char>{0x20, 0, 3, 0, static_cast<char>(0xFF), 0});
			commands_.emplace_back(std::vector<char>{0x12, 0, 0});
			commands_.emplace_back(std::vector<char>{0x20, 0, 3, 0, 0, static_cast<char>(0xFF)});

			commands_.emplace_back(std::vector<char>{0x13, 0, 1, 0});
			commands_.emplace_back(std::vector<char>{0x01, 0, 2, 1, 2});

			commands_.emplace_back(std::vector<char>{0x13, 0, 0});
		}
		void operator() () {
			try {
				while(true) {
					std::this_thread::sleep_for(std::chrono::milliseconds(DELAY));

					for (const auto& cmd : commands_) {
						Logger::Log(cmd);
						channel_->Send(cmd);
						std::this_thread::sleep_for(std::chrono::milliseconds(DELAY));
					}
				}
			}
			catch(lamp::SocketError& e) {
                                //std::cout << "Exception: " << e.what() << std::endl;
                        }

		}
	};
}; //namespace lamp

#endif //LAMP_SERVER_HPP

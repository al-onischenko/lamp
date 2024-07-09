// lamp connect to lamp_server (default host 127.0.0.1, port 9999)
// recieve TLV command:
//   type   - 1 byte,
//   length - 2 bytes,
//   value  - length bytes
// commands:
//   ON    - type = 0x12, length= 0
//   OFF   - type = 0x13, length= 0
//   COLOR - type = 0x20, length= 3, value = RGB

#include "lamp.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <boost/program_options.hpp>

namespace {
	constexpr auto DEFAULT_HOST = "127.0.0.1";
	constexpr auto DEFAULT_PORT = 9999;
	constexpr auto DELAY = 1000; // ms
};

namespace po = boost::program_options;

int main(int argc, char *argv[]) {
	try {
		po::options_description desc("Options");
		desc.add_options()
			("help", "help message")
			;
		po::options_description hidden("");
		hidden.add_options()
			("host", po::value<std::string>()->default_value(DEFAULT_HOST), "host address")
			("port", po::value<int>()->default_value(DEFAULT_PORT), "host port")
			;
		po::positional_options_description pd;
		pd.add("host", 1).add("port", 2);

		po::variables_map vm;
		std::string host;
		int port;
		try {
			po::store(po::command_line_parser(argc, argv).options(po::options_description(desc).add(hidden)).positional(pd).run(), vm);
			po::notify(vm);
			if(vm.count("help")) {
				std::cout << "Usage: " << basename(argv[0]) << " [Options] host port\n";
				std::cout << desc << std::endl;
				return 0;
			}
			host = vm["host"].as<std::string>();
			port = vm["port"].as<int>();
			if(port < 1) throw std::range_error("not positive port number");
		}
		catch(po::error& e) {
			std::cerr << "Error: " << e.what() << std::endl;
			std::cerr << desc << std::endl;
			return -1;
		}

		std::cout << "Start Lamp, connect to host: " << host << ", port: " << port << std::endl;
		while (true) {
			try {
				lamp::Lamp(lamp::TcpClient(host, port).Connect()).Run();
			}
			catch(lamp::SocketError& e) {
				//std::cout << "Exception: " << e.what() << std::endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(DELAY));
			}
		}
	}
	catch(std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
		return -1;
	}
	catch(...) {
		std::cerr << "Unknown error" << std::endl;
		return -1;
	}
	return 0;
}

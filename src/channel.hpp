#ifndef LAMP_CHANNEL_HPP
#define LAMP_CHANNEL_HPP

#include <stdexcept>
#include <string>
#include <memory>
#include <vector>
#include <iostream>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

namespace lamp {
	class SocketError: public std::logic_error {
	public:
		explicit SocketError(const std::string& error): std::logic_error(error) {
		}
	};

	class Descriptor {
		int descriptor_;
	public:
		using Ptr = std::shared_ptr<Descriptor>;
		explicit Descriptor(int descriptor): descriptor_(descriptor) {
			if (descriptor_ < 0) { throw std::logic_error("Can`t create socket"); }
		}
		~Descriptor() {
			::close(descriptor_);
			//std::cout << "Socket " << descriptor_ << " closed" << std::endl;
		}
		int Get() {
			return descriptor_;
		}
	};

	class Channel {
		Descriptor::Ptr descriptor_;
	public:
		using Ptr = std::shared_ptr<Channel>;
		explicit Channel(Descriptor::Ptr descriptor): descriptor_(descriptor) {
		}
		~Channel() {
			//int result = 
			::shutdown(descriptor_->Get(), 2);
			//std::cout << "Socket " << descriptor_->Get() << " shutdown result = " << result << std::endl;
		}

		size_t Receive(std::vector<char>& buffer) {
			CheckConnection();
			ssize_t size = ::read(descriptor_->Get(), &buffer[0], buffer.size());
			//std::cout << "Receive size = " << size << std::endl;
			if (size < 0) { throw SocketError("Read fail"); }
			return static_cast<size_t>(size);
		}
		bool Send(const std::vector<char>& buffer) {
			CheckConnection();
			ssize_t size = ::write(descriptor_->Get(), &buffer[0], buffer.size());
			if (size < 0) { throw SocketError("Write fail"); }
			if (static_cast<size_t>(size) != buffer.size()) { throw SocketError("Write wrong size"); }
			return true;
		}

	private:
		void CheckConnection() {
			int error_value;
			socklen_t len = sizeof(int);
			int result = ::getsockopt(descriptor_->Get(), SOL_SOCKET, SO_ERROR, &error_value, &len);
			if (result != 0) { throw SocketError("Get socket options fail"); }
			if (len != sizeof(int)) { throw SocketError("Socket options value size invalid"); }
			if (error_value != 0) { throw SocketError("Socket connection fail"); }
		}
	};

	class TcpClient {
		std::string host_;
		int port_;
		sockaddr sock_address_;
		Descriptor::Ptr descriptor_;
	public:
		TcpClient(const std::string host, const int port)
			: host_(host), port_(port), descriptor_(new Descriptor(::socket(PF_INET, SOCK_STREAM, 0))) {
			sockaddr_in sock_address;
			std::memset(&sock_address, '\0', sizeof (sock_address));
			sock_address.sin_family = AF_INET;
			sock_address.sin_port = htons(port_);
			int result = ::inet_pton(AF_INET, host_.c_str(), &sock_address.sin_addr);
			if (result != 1) { throw std::invalid_argument("Host address invalid"); }
			sock_address_ = *reinterpret_cast<sockaddr*>(&sock_address);
			//std::cout << "Socket " << descriptor_->Get() << " created" << std::endl;
		}
		Channel::Ptr Connect() {
			int result = ::connect(descriptor_->Get(), &sock_address_, sizeof(sockaddr));
			if (result != 0) { throw SocketError("Can`t connect host"); }
			return Channel::Ptr(new Channel(descriptor_));
		}
	};

	class TcpServer {
		int port_;
		sockaddr_in sockaddr_;
		Descriptor::Ptr descriptor_;
	public:
		TcpServer(const int port)
			: port_(port), descriptor_(new Descriptor(::socket(PF_INET, SOCK_STREAM, 0))) {
			sockaddr_in sock_address;
			std::memset(&sock_address, '\0', sizeof (sock_address));
			sock_address.sin_family = AF_INET;
			sock_address.sin_port = htons(port_);
			sock_address.sin_addr.s_addr = htonl(INADDR_ANY);
			//std::cout << "Socket " << descriptor_->Get() << " created" << std::endl;
			int param;
			int result;
			result = ::setsockopt(descriptor_->Get(), SOL_SOCKET, SO_REUSEADDR, &param, static_cast<socklen_t>(sizeof(param)));
			if (result != 0) { throw std::logic_error("setsockopt reuseaddr"); }
			result = ::bind(descriptor_->Get(), reinterpret_cast<sockaddr*>(&sock_address), sizeof(sock_address));
			if (result != 0) { throw std::logic_error("bind"); }
			result = ::listen(descriptor_->Get(), SOMAXCONN);
			if (result != 0) { throw std::logic_error("listen"); }
			//std::cout << "Socket " << descriptor_->Get() << " ready to accept" << std::endl;
		}
		Channel::Ptr Accept() {
			socklen_t addrlen = sizeof(sockaddr_);
			Channel::Ptr channel(new Channel(Descriptor::Ptr(new Descriptor(::accept(descriptor_->Get(), reinterpret_cast<sockaddr*>(&sockaddr_), &addrlen)))));
			std::vector<char> buffer(addrlen);
			const char* host = ::inet_ntop(AF_INET, &sockaddr_.sin_addr, &buffer[0], static_cast<socklen_t>(buffer.size()));
			if (host == nullptr) { throw SocketError("inet_ntop"); }
			std::cout << "Host " << host << " accepted" << std::endl;
			return channel;
		}
	};
}; // namespace lamp
#endif // LAMP_CHANNEL_HPP

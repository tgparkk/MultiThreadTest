#include <iostream>

#include<olc_net.h>

enum class CustomMsgTypes : uint32_t
{
	FireBullet,
	MovePlayer
};

class CustionClient : public olc::net::client_interface<CustomMsgTypes>
{
public:
	bool FireBullet(float x, float y)
	{
		olc::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::FireBullet;
		msg << x << y;
		Send(msg);
	}
};

int main()
{
	CustionClient c;
	c.Connect("community.onelonecoder.com", 60000);
	c.FireBullet(2.0f, 5.0f);

	return 0;
}

//#ifdef _WIN32
//#define _WIN32_WINNT 0x0A00
//#endif
//#define ASIO_STANDALONE
//
//#include <boost/asio.hpp>
//
//#include <boost/system/error_code.hpp>
//#include <boost/asio/ts/buffer.hpp>
//#include <boost/asio/ts/internet.hpp>
//
//std::vector<char> vBuffer(20 * 1024);
//
//void GrabSomeData(boost::asio::ip::tcp::socket& socket)
//{
//	socket.async_read_some(boost::asio::buffer(vBuffer.data(), vBuffer.size()), 
//		[&](std::error_code ec, std::size_t length)
//		{
//			if (!ec)
//			{
//				std::cout << "\n\nRead " << length << " bytes\n\n";
//
//				for (int i = 0; i < length; i++)
//					std::cout << vBuffer[i];
//
//				GrabSomeData(socket);
//			}
//		}
//	);
//}
//
//int main()
//{
//	std::cout << "123" << std::endl;
//
//	boost::system::error_code ec;
//
//	boost::asio::io_context context;
//
//	boost::asio::io_context::work idleWork(context);
//
//	std::thread thrContext = std::thread([&]() {context.run(); });
//
//	//boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address("https://tgparkk.github.io/", ec), 80);
//	boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address("51.38.81.49", ec), 80);
//	//boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address("93.184.216.34", ec), 80);
//	//boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address("127.0.0.1", ec), 80);
//
//	//tell socket to try and connect
//	boost::asio::ip::tcp::socket socket(context);
//
//	socket.connect(endpoint, ec);
//
//	if (!ec)
//	{
//		std::cout << "Connected" << std::endl;
//	}
//	else
//	{
//		std::cout << "Failed to Connected to address;\n" << ec.message() << std::endl;
//	}
//
//
//	if (socket.is_open())
//	{
//		GrabSomeData(socket);
//
//		std::string sRequest =
//			"GET /index.html HTTP/1.1\r\n"
//			"Host: example.com\r\n"
//			"Connection: close\r\n\r\n";
//
//		socket.write_some(boost::asio::buffer(sRequest.data(), sRequest.size()), ec);
//
//
//
//		using namespace std::chrono_literals;
//		std::this_thread::sleep_for(2000ms);
//
//		context.stop();
//		if (thrContext.joinable()) thrContext.join();
//
//	}
//
//	system("pause");
//	return 0;
//}
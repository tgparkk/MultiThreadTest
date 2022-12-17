#pragma once

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <string>
#include <array>
#include <iostream>
//#include <boost/thread.hpp>

// class Session
// {
// public:
// 	Session(boost::asio::io_service& io_service);
// 
// public:
// 	boost::asio::ip::tcp::socket& Socket();
// 	void PostReceive();
// 	void handle_write(const boost::system::error_code& /*error*/, size_t /*bytes_transferred*/);
// 	void handle_receive(const boost::system::error_code& error, size_t bytes_transferred);
// 
// private:
// 	boost::asio::ip::tcp::socket m_socket;
// 	std::string m_writeMessage;
// 	std::array<char, 128> m_receiveBuffer;
// 
// };

class AsyncServerTest
{
public:
	/*AsyncServerTest(boost::asio::io_service io_service);*/
	AsyncServerTest();
	~AsyncServerTest();

public:
	//void StartAccept();
	//void handle_accept(Session* pSession, const boost::system::error_code& error);

private:
	int m_nSeqNumber;
	//boost::asio::ip::tcp::acceptor m_acceptor;
	boost::asio::io_service m_io_service;
	
	//boost::asio::thread_pool m_thread_group;
	boost::asio::thread_pool m_thread_pool;

	//Session* m_pSession;

};


#pragma once

#include <boost/asio.hpp>
#include <boost/thread.hpp>


class Acceptor;

class Server
{
public:
	Server();
	~Server();

	void start_server(unsigned short port, unsigned int thread_pool_size, boost::system::error_code& ec);
	void stop_server();

private:
	boost::asio::io_service m_io_service;
	boost::asio::io_service::work m_work;
	Acceptor* m_accecptor;
	boost::thread_group m_thread_group;
};


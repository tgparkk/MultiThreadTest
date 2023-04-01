#pragma once

#include <boost/asio.hpp>
#include <boost/atomic.hpp>
#include <boost/bind/bind.hpp>

class Service;

class Acceptor
{
public:
	Acceptor(boost::asio::io_service& ios, unsigned short port);
	~Acceptor();
	void prepare(boost::system::error_code& error_code);
	void async_accept_start();
	void stop();

private:
	void on_accept(boost::shared_ptr<Service> service, const boost::system::error_code& error);

private:
	boost::asio::io_service& m_io_service;
	boost::asio::ip::tcp::endpoint m_endpoint;
	boost::asio::ip::tcp::acceptor m_accecptor;
	boost::atomic<bool> m_isStopped;

};


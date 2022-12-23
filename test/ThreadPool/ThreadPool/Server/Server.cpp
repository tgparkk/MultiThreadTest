#include "Server.h"

#include "Acceptor.h"

Server::Server()
	: m_io_service(),
	m_work(m_io_service),
	m_accecptor(NULL),
	m_thread_group()
{

}

Server::~Server()
{
	//stop();
	if (m_accecptor)
	{
		delete m_accecptor;
		m_accecptor = NULL;
	}
}

void Server::start_server(unsigned short port, unsigned int thread_pool_size, boost::system::error_code& ec)
{
	//ASSERT(thread_pool_size > 0);

	m_accecptor = new Acceptor(m_io_service, port);

	boost::system::error_code prepare_ec;
	m_accecptor->prepare(prepare_ec);
	if (prepare_ec) {
		ec = prepare_ec;
		return;
	}

	m_accecptor->async_accept_start();

	for (int i = 0; i < thread_pool_size; ++i)
	{
		m_thread_group.create_thread(boost::bind(&boost::asio::io_context::run, &m_io_service));
	}

	//AELOG("server started.");
}

void Server::stop_server()
{
	m_accecptor->stop();
	m_io_service.stop();

	m_thread_group.join_all();
	//AELOG("server ended.");
}

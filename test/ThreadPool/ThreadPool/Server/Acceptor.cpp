#include "Acceptor.h"
#include "Service.h"
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

Acceptor::Acceptor(boost::asio::io_service& ios, unsigned short port)
	: m_io_service(ios),
	m_endpoint(boost::asio::ip::address_v4::any(), port),
	m_accecptor(m_io_service),
	m_isStopped(false)
{

}

Acceptor::~Acceptor()
{
	boost::system::error_code ec_cancel;
	m_accecptor.cancel(ec_cancel); //nothrow

	boost::system::error_code ec_close;
	m_accecptor.close(ec_close); //nothrow
}

void Acceptor::prepare(boost::system::error_code& error_code)
{
	boost::system::error_code open_ec;
	m_accecptor.open(m_endpoint.protocol(), open_ec); //소켈을 특정프로토롤로 open
	if (open_ec)
	{
		//AELOG("acceptor open error.");
		error_code = open_ec;
		return;
	}

	boost::system::error_code bind_ec;
	m_accecptor.bind(m_endpoint, bind_ec); //소켓을 특정종료점과 묶는 과정을 binding 이라함.
	if (bind_ec)
	{
		//AELOG("acceptor socket bind error.");
		error_code = bind_ec;
		return;
	}

	//들어오는 요청이 있는지 듣기 시작
	boost::system::error_code listen_ec;
	m_accecptor.listen(boost::asio::socket_base::max_listen_connections, listen_ec);
	if (listen_ec)
	{
		//AELOG("acceptor linsten error.");
		error_code = listen_ec;
	}
}

void Acceptor::async_accept_start()
{
	//AELOG("service created and async_accept");
	boost::shared_ptr<Service> service = boost::make_shared<Service>(boost::ref(m_io_service));
	m_accecptor.async_accept(service->sock(),
		boost::bind(&Acceptor::on_accept, this, service, boost::placeholders::_1));
}

void Acceptor::stop()
{
	//m_isStopped = true;
}

void Acceptor::on_accept(boost::shared_ptr<Service> service, const boost::system::error_code& error)
{
	//작업실행(에러일때, 아닐때)
	if (error)
	{
		//AELOG("on_accept error:%s", error.message().c_str());
	}
	else
	{
		service->run_service();
	}

	//작업진행(진행/멈춤)
	if (m_isStopped) {
		//AELOG("stop 요청에 의한 accept 중단");
		boost::system::error_code ec;
		m_accecptor.close(ec); //nothrow
	}
	else
	{
		async_accept_start();
	}

	//delete service;

}

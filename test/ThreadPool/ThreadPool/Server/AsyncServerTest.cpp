#include "AsyncServerTest.h"

const unsigned short PORT_NUMBER = 31400;

AsyncServerTest::AsyncServerTest(boost::asio::io_service& io_service)
	:m_nSeqNumber(0)
	, m_acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), PORT_NUMBER))
	, m_io_service(io_service)
	, m_pSession(nullptr)
	
{
	StartAccept();
}

AsyncServerTest::~AsyncServerTest()
{
	if (m_pSession != nullptr)
		delete m_pSession;
}

void AsyncServerTest::StartAccept()
{
	std::cout << "클라이언트 접속 대기....." << std::endl;

	m_pSession = new Session(m_io_service);

	m_acceptor.async_accept(m_pSession->Socket()
		, boost::bind(&AsyncServerTest::handle_accept
			, this, m_pSession
			, boost::asio::placeholders::error));
}

void AsyncServerTest::handle_accept(Session* pSession, const boost::system::error_code& error)
{
	if (!error)
	{
		std::cout << "클라이언트 접속 성공" << std::endl;

		pSession->PostReceive();
	}
}

Session::Session(boost::asio::io_service& io_service)
	: m_socket(io_service)
	, m_writeMessage()
	, m_receiveBuffer()
{

}

boost::asio::ip::tcp::socket& Session::Socket()
{
	return m_socket;
}

void Session::PostReceive()
{
	memset(&m_receiveBuffer, '\0', sizeof(m_receiveBuffer));

	m_socket.async_read_some
	(boost::asio::buffer(m_receiveBuffer)
		, boost::bind(&Session::handle_receive
			, this, boost::asio::placeholders::error
			, boost::asio::placeholders::bytes_transferred));
}

void Session::handle_write(const boost::system::error_code& /*error*/, size_t /*bytes_transferred*/)
{

}

void Session::handle_receive(const boost::system::error_code& error, size_t bytes_transferred)
{
	if (error)
	{
		if (error == boost::asio::error::eof)
			std::cout << "클라이언트와 연결이 끊어졌습니다" << std::endl;
		else
			std::cout << "error No: " << error.value() << " error Message: " << error.message() << std::endl;
	}
	else
	{
		const std::string strRecvMessage = m_receiveBuffer.data();
		std::cout << "클라이언트에서 받은 메시지: " << strRecvMessage << ", 받은 크기: " << bytes_transferred << std::endl;

		char szMessage[128] = { 0, };
		sprintf_s(szMessage, 128 - 1, "Re:%s", strRecvMessage.c_str());
		m_writeMessage = szMessage;

		boost::asio::async_write(m_socket, boost::asio::buffer(m_writeMessage), boost::bind(&Session::handle_write, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));

		PostReceive();
	}
}

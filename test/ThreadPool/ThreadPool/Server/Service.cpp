#include "Service.h"


Service::Service(boost::asio::io_service& ios)
	:
	m_sock(ios),
	//m_request_header(),
	m_request_body(),
	//m_request(NULL),
	//m_worker(NULL),
	//m_response(NULL),
	m_response_buf()
{
}

Service::~Service()
{
// 	if (m_response) {
// 		delete m_response;
// 		m_response = NULL;
// 	}
// 
// 	if (m_worker) {
// 		delete m_worker;
// 		m_worker = NULL;
// 	}
// 
// 	if (m_request) {
// 		delete m_request;
// 		m_request = NULL;
// 	}
// 
// 	//m_sock.cancel();

	boost::system::error_code ec;
	m_sock.close(ec); //nothrow

}

void Service::run_service()
{
// 	boost::asio::async_read(m_sock,
// 		boost::asio::buffer((char*)&m_request_header, sizeof(m_request_header)),
// 		boost::bind(&Service::on_read_request_header, shared_from_this(), _1));
}

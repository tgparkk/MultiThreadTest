#pragma once

#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/enable_shared_from_this.hpp>


//소켓에서 하나의 Request를 읽어 처리하고 Response 보내는 기능을 한다. 내부 소켓을 가지고 
class Service : public boost::enable_shared_from_this<Service>
{
public:
	Service(boost::asio::io_service& ios);
	~Service();
	void run_service();
	boost::asio::ip::tcp::socket& sock() { return m_sock; }

private:
// 	void on_read_request_header(const boost::system::error_code& error);
// 	void on_read_request_body(const boost::system::error_code& error);

	//void send_response(const IMessage* response);

	//void on_write_response(const boost::system::error_code& error);

private:
	boost::asio::ip::tcp::socket m_sock;

	//요청
	//MsgHeader m_request_header;
	std::string m_request_body;
	//IMessage* m_request;

	//작업
	//IWorker* m_worker;

	//응답
	//IMessage* m_response;
	boost::asio::streambuf m_response_buf; //추후 stringstream 으로 변경하자.



};

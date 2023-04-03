#include "Service.h"
#include "worker_creator.h"
#include "request_creator.h"

Service::Service(boost::asio::io_service& ios)
	:
	m_sock(ios),
	m_request_header(),
	m_request_body(),
	m_request(NULL),
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
 	if (m_request) {
 		delete m_request;
 		m_request = NULL;
 	}
// 
// 	//m_sock.cancel();

	boost::system::error_code ec;
	m_sock.close(ec); //nothrow

}

void Service::run_service()
{
 	boost::asio::async_read(m_sock,
 		boost::asio::buffer((char*)&m_request_header, sizeof(m_request_header)),
 		std::bind(&Service::on_read_request_header, shared_from_this(), std::placeholders::_1));
}


void Service::run_service()
{
	boost::asio::async_read(m_sock, 
		boost::asio::buffer((char*)&m_request_header, sizeof(m_request_header)), 
		std::bind(&Service::on_read_request_header, shared_from_this(), std::placeholders::_1));
}

void Service::on_read_request_header(const boost::system::error_code& error)
{
	if (error)
	{
		//AELOG("on_read_request_header error:%s", error.message().c_str());
		return;
	}

	if (!m_request_header.IsValid())
	{
		send_response(&ResError_V1(L"메세지 헤더정보가 유효하지 않음."));
		return;
	}

	m_request_body.resize(m_request_header.body_size(), 0);
	boost::asio::async_read(m_sock, 
		boost::asio::buffer(&m_request_body[0], m_request_body.size()), 
		std::bind(&Service::on_read_request_body, shared_from_this(), std::placeholders::_1));
	
}

void Service::on_read_request_body(const boost::system::error_code& error)
{
	if (error)
	{
		//AELOG("on_read_request_body error:%s", error.message().c_str());
		return;
	}
	
	m_request = RequestCreator::create(m_request_header.msg_id());
	if (!m_request) 
	{
		_ASSERT(false);
		send_response(&ResError_V1(L"requset 생성 실패."));
		return;
	}

	m_request->load_from(m_request_body);

	const std::wstring& id_name = EnumStringW<eNetID::T>::From(m_request->msg_id());

	AELOG(L"received msg:%s, %s", id_name.c_str(), m_request->to_string().c_str());
	GetRunningInfo().m_req_received_count++; //atomic
	
	m_worker = createWorker(m_request);
	if (!m_worker) 
	{
		_ASSERT(false);
		wchar_t err_text[256] = {0,};
		StringCbPrintfW(err_text, sizeof(err_text), L"%s 요청에 대한 Worker 생성에 실패했습니다.", id_name.c_str());
		send_response(&ResError_V1(err_text));
		return;
	}
	
	m_response = m_worker->processing();
	if (!m_response) 
	{
		_ASSERT(false);
		wchar_t err_text[256] = {0,};
		StringCbPrintfW(err_text, sizeof(err_text), L"%s 요청에 대한 Response 생성에 실패했습니다.", id_name.c_str());
		send_response(&ResError_V1(err_text));
		return ;
	}

	send_response(m_response);

}

void Service::send_response(const IMessage* response)
{
	//응답을 stream으로 
	std::string body;

	ch::system_clock::time_point comp_start = ch::system_clock::now();
	//response->save_to(body);
	ch::duration<double> comp_delta = ch::system_clock::now() - comp_start;

	MsgHeader header(response->msg_id(), (int)body.size());
	
	std::ostream dummy(&m_response_buf);
	dummy.write((const char*)&header, sizeof(header));
	dummy.write((const char*)&body[0], body.size());

	boost::asio::async_write(m_sock, m_response_buf, boost::bind(&Service::on_write_response, shared_from_this(), _1));

	const std::wstring& id_name = EnumStringW<eNetID::T>::From(response->msg_id());
	AELOG(L"responsed msg:(%s,%s) comp_time:%f, size:%d", id_name.c_str(), response->to_string().c_str(), comp_delta, body.size());
	GetRunningInfo().m_res_sended_count++; //atomic

}

void Service::on_write_response(const boost::system::error_code& error)
{
	if (error)
	{
		//AELOG("on_write_response error:%s", error.message().c_str());
		return;
	}
	
	// 연결을 종료시킨다. 소켓이 연결되어 잇지 않다면 이 메서드는 실패한다. 이 함수가 실패하더라도 오류 코드에 신경 쓰지 않는다.
	//shutdown_send 로 보낼경우 상대방은 boost::asio::error::eof 에러코드를 받는다.
	boost::system::error_code ignored_ec;
	m_sock.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ignored_ec); 
}

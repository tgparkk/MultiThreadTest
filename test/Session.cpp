#include "stdafx.h"
#include "session.h"

#include "common/imessage.h"
#include <boost/function.hpp>
#include "message_creator.h"
#include "cotree_error_code.h"
#include "util_common.h"

Session::Session(
				 boost::asio::io_service& ios, 
				 const IMessage* request, 
				 const on_user_callback_type& callback, 
				 const on_session_end_type& end_session)
: m_sock(ios),
m_session_id(0),
m_request(request),
m_request_buf(),
m_response_header(),
m_response_body(),
m_response(NULL),
m_callback(callback),
m_on_session_end(end_session),
m_was_cancelled(false),
m_async_read_timer(ios),
m_read_timeout(false)
{
	static int s_id = 0;
	s_id++; 

	m_session_id = s_id;  
	
#ifdef _DEBUG
	//test_convert(m_request);
#endif
	
	
}

Session::~Session()
{
	if (m_request) {
		delete m_request;
		m_request = NULL;
	}

	if (m_response)
	{
		delete m_response;
		m_response = NULL;
	}

	boost::system::error_code ignored_ec;
	m_sock.close(ignored_ec); //nothrow
	
	// 	boost::system::error_code ignored_ec;
	// 	m_sock.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ignored_ec); 
}

void Session::start_session(const boost::asio::ip::tcp::resolver::results_type& endpoints)
{
	boost::asio::async_connect(m_sock, endpoints, 
		boost::bind(&Session::on_connect, shared_from_this(), _1));

}

void Session::do_process()
{
	if (m_was_cancelled) {
		onRequsetComplete(eCotreeError::UserCanceled);
		return;
	}

	AELOG(L"msg id:%d, %s", m_request->msg_id(), m_request->to_string().c_str());
	AELOG(typeid(*m_request).name());

	//객체를 스트림으로 바꾸기.
	std::string body;
	m_request->save_to(body);
	
	MsgHeader header(m_request->msg_id(), (int)body.size());
	
	std::ostream dummy(&m_request_buf);
	dummy.write((const char*)&header, sizeof(header));
	dummy.write((const char*)&body.at(0), body.size());

	boost::asio::async_write(m_sock, m_request_buf, boost::bind(&Session::on_write_request, shared_from_this(), _1));
}

void Session::on_write_request(const boost::system::error_code& error)
{
	if (m_was_cancelled) {
		onRequsetComplete(eCotreeError::UserCanceled);
		return;
	}

	if (error) {
		onRequsetComplete(error);
		return;
	}

	// 헤더/바디 따로 읽기 	
	boost::asio::async_read(m_sock, 
		boost::asio::buffer((char*)&m_response_header, sizeof(m_response_header)), 
		boost::bind(&Session::on_read_response_header, shared_from_this(), _1));

	//혹시나 coconut tree 에 접속되었을 경우 coconut tree는 read 요청을 보내도 반응을 하지 않기에 끊어줘야 함.
	m_async_read_timer.expires_from_now(boost::chrono::seconds(60 * 5)); //(서버처리 시간 ) 최대 2분
	m_async_read_timer.async_wait(boost::bind(&Session::on_read_timeout, shared_from_this(), _1));
}

void Session::on_read_response_header(const boost::system::error_code& error)
{
	m_async_read_timer.cancel();

	if (m_read_timeout) {
		onRequsetComplete(eCotreeError::ReadTimeOut);
		return;
	}

	if (m_was_cancelled) {
		onRequsetComplete(eCotreeError::UserCanceled);
		return;
	}

	if (error) {
		onRequsetComplete(error);
		return;
	}

	if (!m_response_header.IsValid()) {
		onRequsetComplete(eCotreeError::MsgHeaderInvalid);
		return;
	}

	m_response_body.resize(m_response_header.body_size(), 0);
	boost::asio::async_read(m_sock, 
		boost::asio::buffer(&m_response_body[0], m_response_body.size()), 
		boost::bind(&Session::on_read_response_body, shared_from_this(), _1));

	//m_async_read_timer.expires_from_now(boost::chrono::seconds(5)); //헤더받은 후 바이 받는작업은 서버 작업시간이 없기에 짧음.
	//m_async_read_timer.async_wait(boost::bind(&Session::on_read_timeout, shared_from_this(), _1));
}

void Session::on_read_response_body(const boost::system::error_code& error)
{
	//m_async_read_timer.cancel();

	if (m_read_timeout) {
		onRequsetComplete(eCotreeError::ReadTimeOut);
		return;
	}

	if (m_was_cancelled) {
		onRequsetComplete(eCotreeError::UserCanceled);
		return;
	}

	if (error) {
		onRequsetComplete(error);
		return;
	}

	IMessage* response = MsgCreator::create(m_response_header.msg_id());
	if (!response) {
		onRequsetComplete(eCotreeError::ResponseCreateFail); 
		return;
	}

	response->load_from(m_response_body);
	m_response = response;

	onRequsetComplete(error);

}

void Session::onRequsetComplete(const boost::system::error_code& ec)
{
	// 연결을 종료시킨다.
	// 소켓이 연결되어 잇지 않다면 이 메서드는 실패한다.
	// 이 함수가 실패하더라도 오류 코드에 신경 쓰지 않는다.
	boost::system::error_code ignored_ec;
	m_sock.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);



	// 사용자가 제공한 콜백을 호출한다.
	if (m_callback) {
		m_callback(m_response, ec);
	}
	
	//
	if (m_on_session_end) {
		m_on_session_end(m_session_id); 
	}


}

void Session::do_cancel()
{
	m_was_cancelled = true;

	boost::system::error_code ignored_ec;
	m_sock.cancel(ignored_ec);
}

// void Session::test_convert(const IMessage* request)
// {
// 	//요청을 stream 으로 
// 	std::string buf;
// 	request->save_to(buf);
// 
// 	//stream을 요청으로 
// 	IMessage* req_test = MsgCreator::create(request->msg_id());
// 
// 	req_test->load_from(buf);
// 
// 	delete req_test;
// }

void Session::on_read_timeout(const boost::system::error_code& error)
{
	//cancel 호출시 발생
	if (error == boost::asio::error::operation_aborted) {
		return;
	}

	if (error) {
		AELOG("Session::on_read_timeout error, %s", error.message().c_str());
		return;
	}

	m_read_timeout = true;
	boost::system::error_code ignored_ec;
	m_sock.cancel(ignored_ec);
}

void Session::on_connect(const boost::system::error_code& error)
{
	//m_async_read_timer.cancel();

	if (error)
	{
		onRequsetComplete(error);
		return;
	}

	do_process();
}


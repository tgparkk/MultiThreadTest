#pragma once

#include "common/imessage.h"
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/atomic.hpp>
#include <boost/thread.hpp>

class Session;

typedef boost::function2<void, const IMessage*, const boost::system::error_code&> on_user_callback_type; /*response*/
typedef boost::function1<void, int> on_session_end_type; 

class Session : public boost::enable_shared_from_this<Session>
{
public:
	Session(
		boost::asio::io_service& ios, 
		const IMessage* request, 
		const on_user_callback_type& callback, 
		const on_session_end_type& end_session);
	~Session();

	void start_session(const boost::asio::ip::tcp::resolver::results_type& endpoints);

	void do_process();
	void do_cancel();
	
	void onRequsetComplete(const boost::system::error_code& ec);

	boost::asio::ip::tcp::socket& sock() {return m_sock;}
	const IMessage* getRequest() const {return m_request;}
	int get_session_id() const {return m_session_id; }

private:
	void on_write_request(const boost::system::error_code& error);
	void on_read_response_header(const boost::system::error_code& error);
	void on_read_response_body(const boost::system::error_code& error);
	//void test_convert(const IMessage* request);
	void on_read_timeout(const boost::system::error_code& error);

	void on_connect(const boost::system::error_code& error);

private:
	boost::asio::ip::tcp::socket m_sock;
	int m_session_id; //세션 생성마다 unique 값을 가짐.

	const IMessage* m_request;
	boost::asio::streambuf m_request_buf;
	
	MsgHeader m_response_header;
	std::string m_response_body;
	IMessage* m_response;

	on_user_callback_type m_callback;
	on_session_end_type m_on_session_end;

	boost::atomic<bool> m_was_cancelled;

	boost::asio::steady_timer m_async_read_timer;
	boost::atomic<bool> m_read_timeout;

};


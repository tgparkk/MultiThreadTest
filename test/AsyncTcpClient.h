#pragma once

#include <boost/asio.hpp>
#include <boost/atomic.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>
#include "session.h"

class Session;

class AsyncTcpClient : public boost::noncopyable
{
public:
	AsyncTcpClient(unsigned char num_thread);
	~AsyncTcpClient();
	void SetEndpoints(const std::string& server_name, const std::string& port);

	void send_request(const IMessage* request, const on_user_callback_type& callback);
	void cancel_request(const IMessage* request);
	void join_all();

	void cancel_all();
private:
	
	void on_end_session(int session_id);

private:
	boost::asio::io_service m_ios;
	
	boost::asio::ip::tcp::resolver::results_type m_endpoints;

	std::map<int, boost::shared_ptr<Session>> m_active_sessions;
	boost::mutex m_active_sessions_guard;

	boost::asio::io_service::work* m_work;

	boost::thread_group m_threadgroup;
};




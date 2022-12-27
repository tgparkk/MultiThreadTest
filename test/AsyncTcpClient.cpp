#include "stdafx.h"
#include "asynctcpclient.h"
#include "common/imessage.h"
#include <boost/thread/mutex.hpp>
#include <boost/function.hpp>


AsyncTcpClient::AsyncTcpClient(unsigned char num_thread)
{
	m_work = new boost::asio::io_service::work(m_ios);

	for (int i = 0; i < num_thread; ++i) 
	{
		m_threadgroup.create_thread(boost::bind(&boost::asio::io_context::run, &m_ios));
	}
}

AsyncTcpClient::~AsyncTcpClient()
{
	join_all();
}

void AsyncTcpClient::SetEndpoints(const std::string& server_name, const std::string& port)
{
	boost::asio::ip::tcp::resolver resolver(m_ios);

	boost::system::error_code error;
	m_endpoints = resolver.resolve(server_name, port, error);
	if (error) {
		AELOG(error.message().c_str());
	}
}

void AsyncTcpClient::send_request(const IMessage* request, const on_user_callback_type& callback)
{
	boost::shared_ptr<Session> session = boost::make_shared<Session>(boost::ref(m_ios), request, callback, boost::bind(&AsyncTcpClient::on_end_session, this, _1));
	
	boost::unique_lock<boost::mutex> lock(m_active_sessions_guard);
	m_active_sessions[session->get_session_id()] = session;
	lock.unlock();

	session->start_session(m_endpoints);
}


void AsyncTcpClient::cancel_request(const IMessage* request)
{
	boost::unique_lock<boost::mutex> lock(m_active_sessions_guard);
	
	for (std::map<int, boost::shared_ptr<Session>>::iterator it = m_active_sessions.begin(); it != m_active_sessions.end(); it++)
	{
		boost::shared_ptr<Session> ses = it->second;
		if (ses->getRequest() == request)
		{
			ses->do_cancel();
		}
	}

	lock.unlock();
}

void AsyncTcpClient::join_all()
{
	// 작업 객체를 소멸시킨다.
	// 그러면 더 이상 남은 비동기 연산이 없을 때, I/O 쓰레드가 이벤트 루프에서 나올 수 있다.
	if (m_work)
	{
 		delete m_work;
 		m_work = NULL;
	}
 
 	m_threadgroup.join_all();
}

void AsyncTcpClient::cancel_all()
{
	boost::unique_lock<boost::mutex> lock(m_active_sessions_guard);

	for (std::map<int, boost::shared_ptr<Session>>::iterator it = m_active_sessions.begin(); it != m_active_sessions.end(); it++)
	{
		boost::shared_ptr<Session> ses = it->second;
		ses->do_cancel();
	}

	lock.unlock();
}

void AsyncTcpClient::on_end_session(int session_id)
{
	//능동 세션 목록에서 해당 세션을 지운다.
	boost::unique_lock<boost::mutex> lock(m_active_sessions_guard);
	std::map<int, boost::shared_ptr<Session>>::iterator it = m_active_sessions.find(session_id);
	if (it != m_active_sessions.end())
	{
		m_active_sessions.erase(it);
	}
	lock.unlock();

}


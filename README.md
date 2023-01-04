# MultiThreadTest

thread pool
multithread
cocurency


-client
1. AsyncTcpClient 클래스에서 생성자로 스레드그룹생성

```c++
// member variable
boost::asio::io_service m_ios;
boost::asio::ip::tcp::resolver::results_type m_endpoints;
std::map<int, boost::shared_ptr<Session>> m_active_sessions;
boost::mutex m_active_sessions_guard;
boost::asio::io_service::work* m_work;
boost::thread_group m_threadgroup;
AsyncTcpClient client(1);

AsyncTcpClient::AsyncTcpClient(unsigned char num_thread)
{
	m_work = new boost::asio::io_service::work(m_ios);

	for (int i = 0; i < num_thread; ++i) 
	{
		m_threadgroup.create_thread(boost::bind(&boost::asio::io_context::run, &m_ios));
	}
}
```

2. 다른 클래스에서 AsyncTcpClient 클래스를 이용한 서버로 콜백함수 send
```c++
client.SetEndpoints(address, cotree2_port);
/*
void AsyncTcpClient::SetEndpoints(const std::string& server_name, const std::string& port)
boost::asio::ip::tcp::resolver resolver(m_ios);
boost::system::error_code error;
m_endpoints = resolver.resolve(server_name, port, error);
*/

// 서버로 콜백함수 send
client.send_request(new ReqWelcomeMsg_V1(2022), 
		boost::bind(&UtilServer::OnRequestWelcome, this, _1, _2));
```

3. 세션 객체 생성후 비동기 호출 요청
```c++
void AsyncTcpClient::send_request(const IMessage* request, const on_user_callback_type& callback)
{
	boost::shared_ptr<Session> session = boost::make_shared<Session>(boost::ref(m_ios), request, callback, boost::bind(&AsyncTcpClient::on_end_session, this, _1));
	
	boost::unique_lock<boost::mutex> lock(m_active_sessions_guard);
	m_active_sessions[session->get_session_id()] = session;
	lock.unlock();

	session->start_session(m_endpoints);
}
```

4. Session 클래스에서 소켓정보로 세션 생성
```c++
typedef boost::function2<void, const IMessage*, const boost::system::error_code&> on_user_callback_type;

// member variable
boost::asio::ip::tcp::socket m_sock;
int m_session_id; //세션 생성마다 unique 값을 가짐.

Session(
		boost::asio::io_service& ios, 
		const IMessage* request, 
		const on_user_callback_type& callback, 
		const on_session_end_type& end_session);
```

4.1 만들어진 세션으로 비동기 연결 시작
```c++
void Session::start_session(const boost::asio::ip::tcp::resolver::results_type& endpoints)
{
	boost::asio::async_connect(m_sock, endpoints, 
		boost::bind(&Session::on_connect, shared_from_this(), _1));

}
```

```c++
void Session::on_connect(const boost::system::error_code& error)
{
	//m_async_read_timer.cancel();

	if (error)
	{
		onRequsetComplete(error);
		return;
	}

  // 서버로 작업요청
	do_process();
}
```

4.2 비동기로 요청사항 쓰기(write)
```c++
void Session::do_process()
{
	///
  ///

	boost::asio::async_write(m_sock, m_request_buf, boost::bind(&Session::on_write_request, shared_from_this(), _1));
}
```

4.3 비동기로 


# MultiThreadTest

## thread_group, thread
## io_context, io_service

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
request.h
typedef Req_Int<eNetID::Req_Welcome_V1>	ReqWelcomeMsg_V1;
```

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

4.2 비동기로 쓰기(write)
```c++
void Session::do_process()
{
///
///

	boost::asio::async_write(m_sock, m_request_buf, boost::bind(&Session::on_write_request, shared_from_this(), _1));
}
```

4.3 비동기로 읽기(read) (header)
```c++
// 헤더/바디 따로 읽기 	
/// 헤더에는 msgID, bodysize
	boost::asio::async_read(m_sock, 
		boost::asio::buffer((char*)&m_response_header, sizeof(m_response_header)), 
		boost::bind(&Session::on_read_response_header, shared_from_this(), _1));
```

4.4  body 읽기
```c++

/// std::string m_response_body; 은 Session 의 멤버변수

boost::asio::async_read(m_sock, 
		boost::asio::buffer(&m_response_body[0], m_response_body.size()), 
		boost::bind(&Session::on_read_response_body, shared_from_this(), _1));
```
데이터는 m_response_body 에 저장되어있어요.
```c++
IMessage* response = MsgCreator::create(m_response_header.msg_id());
response->load_from(m_response_body);
onRequsetComplete(error);
```
```c++
// 사용자가 제공한 콜백을 호출한다.
	if (m_callback) {
		m_callback(m_response, ec);
	}
```

5. clinet 에서 server 로 비동기 요청 예제
```c++
request.h
typedef Req_Ellipse_Loadtype_Area<eNetID::Req_LoadPolylines_Area_V1, int> ReqLoadPolylinesArea_V1;
```

```c++
void FirstDataLoader::load_polyline(AsyncTcpClient& client)
{
	client.send_request(new ReqLoadPolylinesArea_V1(m_ellipsoid, kairosman.GetLevel(), Area), 
		boost::bind(&FirstDataLoader::OnRequestSelectPolylineNew, this, _1, _2));
}
```

5.1 요청함수의 예외처리, 데이터 get
```c++
respone.h
typedef Res_vector<eNetID::Res_LoadPolylines_V1, out::Polyline>	ResLoadPolylines_V1;
```

```c++
void FirstDataLoader::OnRequestSelectPolylineNew(const IMessage* msg, const boost::system::error_code& ec)
{
	// 서버에서 받은 데이터 읽기
	const ResLoadPolylines_V1* response = dynamic_cast<const ResLoadPolylines_V1*>(msg);
	const std::vector<out::Polyline>& datas = response->getdatas();
}
```

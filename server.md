server
1. Server 클래스에서 boost::thread_group 으로 비동기 환경 구축
```c++
// member variable
boost::asio::io_service m_ios;
boost::asio::io_service::work m_work;
Acceptor* m_acc;
boost::thread_group m_threads;
```
```c++
void Server::start_server(unsigned short port, unsigned int thread_pool_size, boost::system::error_code& ec)
{
	m_acc = new Acceptor(m_ios, port);
  	boost::system::error_code prepare_ec;
  	// 소켓 설정
	m_acc->prepare(prepare_ec);
	m_acc->async_accept_start();
	for (int i = 0; i < thread_pool_size; ++i) 
	{
		m_threads.create_thread(boost::bind(&boost::asio::io_context::run, &m_ios));
	}
}
```

1.1 Acceptor 클래스에서 서버 구축, 소켓설정
```c++
// member variable
boost::asio::io_service& m_ios;
boost::asio::ip::tcp::endpoint m_endpoint;
boost::asio::ip::tcp::acceptor m_acceptor;
boost::atomic<bool> m_isStopped;
```
```c++
Acceptor(boost::asio::io_service& ios, unsigned short port) 
: m_ios(ios), 
m_endpoint(boost::asio::ip::address_v4::any(), port),
m_acceptor(m_ios), 
m_isStopped(false)
{
	
}
```
```c++
// 소켓할당
void Acceptor::prepare(boost::system::error_code& error_code)
{
	boost::system::error_code open_ec;
	m_acceptor.open(m_endpoint.protocol(), open_ec); //소켈을 특정프로토롤로 open
	if (open_ec)
	{
		AELOG("acceptor open error.");
		error_code = open_ec;
		return;
	}

	boost::system::error_code bind_ec;
	m_acceptor.bind(m_endpoint, bind_ec); //소켓을 특정종료점과 묶는 과정을 binding 이라함.
	if (bind_ec)
	{
		AELOG("acceptor socket bind error.");
		error_code = bind_ec;
		return;
	}

	boost::system::error_code listen_ec;
	m_acceptor.listen(boost::asio::socket_base::max_listen_connections, listen_ec);
	if (listen_ec)
	{
		AELOG("acceptor linsten error.");
		error_code = listen_ec;
	}
}
```

1.2 생성한 소켓으로 비동기 accepter 생성, 비동기 환경 구축
```c++
void Acceptor::async_accept_start()
{
	boost::shared_ptr<Service> service = boost::make_shared<Service>(boost::ref(m_ios));
	m_acceptor.async_accept(service->sock(),
		boost::bind(&Acceptor::on_accept, this, service, _1));
}
```
```c++
void Acceptor::on_accept(boost::shared_ptr<Service> service, const boost::system::error_code& error)
{
	if (error)
	{
		AELOG("on_accept error:%s", error.message().c_str());
	}
	else
	{
		// 작업을 수행하는 부분
		service->run_service();
	}

	if (m_isStopped) {
		AELOG("stop 요청에 의한 accept 중단");
		boost::system::error_code ec;
		m_acceptor.close(ec); //nothrow
	}
	else
	{
		async_accept_start();
	}
}
```

2. 비동기로 요청 읽기(헤더, 바디)
```c++
// class Service
// member variable
boost::asio::ip::tcp::socket m_sock;
//요청
MsgHeader m_request_header;
std::string m_request_body;
IMessage* m_request;
//작업
IWorker* m_worker;
//응답
IMessage* m_response;
boost::asio::streambuf m_response_buf; //추후 stringstream 으로 변경하자.
```
```c++
void Service::run_service()
{
	boost::asio::async_read(m_sock, 
		boost::asio::buffer((char*)&m_request_header, sizeof(m_request_header)), 
		boost::bind(&Service::on_read_request_header, shared_from_this(), _1));
}
```
```c++
void Service::on_read_request_header(const boost::system::error_code& error)
{
/// header가 문제 없다면 body를 읽자
	m_request_body.resize(m_request_header.body_size(), 0);
	boost::asio::async_read(m_sock, 
		boost::asio::buffer(&m_request_body[0], m_request_body.size()), 
		boost::bind(&Service::on_read_request_body, shared_from_this(), _1));
	
}
```
```c++
void Service::on_read_request_body(const boost::system::error_code& error)
{
///
	// IMessage* m_request;
	// 메시지를 관리하는 클래스에서 바디를 읽는 부분
	m_request->load_from(m_request_body);

	const std::wstring& id_name = EnumStringW<eNetID::T>::From(m_request->msg_id());

	GetRunningInfo().m_req_received_count++; //atomic
	
	// IWorker* m_worker;
	// interface로 client로 부터 받은 메시지를 통해 작업 생성
	m_worker = createWorker(m_request);
	if (!m_worker) 
	{
		_ASSERT(false);
		wchar_t err_text[256] = {0,};
		StringCbPrintfW(err_text, sizeof(err_text), L"%s 요청에 대한 Worker 생성에 실패했습니다.", id_name.c_str());
		send_response(&ResError_V1(err_text));
		return;
	}
	
	// virtual 함수로 목적에 맞게 캐스팅되어 실제 디비호출하는 부분
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
```

```c++
// client로 보내기 위해 비동기 쓰기(write)
```c++
void Service::send_response(const IMessage* response)
{
	//응답을 stream으로 
	std::string body;

	ch::system_clock::time_point comp_start = ch::system_clock::now();
	response->save_to(body);
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
```


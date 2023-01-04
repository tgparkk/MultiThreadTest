# MultiThreadTest

thread pool
multithread
cocurency


-client
AsyncTcpClient 클래스에서 생성자로 스레드그룹생성
-member variable-
boost::asio::io_service m_ios;
boost::asio::ip::tcp::resolver::results_type m_endpoints;
std::map<int, boost::shared_ptr<Session>> m_active_sessions;
boost::mutex m_active_sessions_guard;
boost::asio::io_service::work* m_work;
boost::thread_group m_threadgroup;
AsyncTcpClient client(1);

boost::asio::io_service m_ios;	
boost::asio::ip::tcp::resolver::results_type m_endpoints;

1. void AsyncTcpClient::SetEndpoints(const std::string& server_name, const std::string& port)
boost::asio::ip::tcp::resolver resolver(m_ios);
boost::system::error_code error;
m_endpoints = resolver.resolve(server_name, port, error);

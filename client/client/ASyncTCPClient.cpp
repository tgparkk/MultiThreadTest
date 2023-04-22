#include "pch.h"
#include "ASyncTCPClient.h"

ASyncTCPClient::ASyncTCPClient()
{
	m_work.reset(new boost::asio::io_service::work(m_ios));
	m_thread.reset(new std::thread([this]() {
		m_ios.run();
		}));
}


void ASyncTCPClient::emulateLongComputationOp(
	unsigned int duration_sec,
	const std::string& raw_ip_address,
	unsigned short port_num,
	Callback callback,
	unsigned int request_id)
{
	// Preparing the request string.
	std::string request = "EMULATE_LONG_CALC_OP tgparkk "
		+ std::to_string(duration_sec)
		+ "\n";
	std::shared_ptr<Session> session =
		std::shared_ptr<Session>(new Session(m_ios,
			raw_ip_address,
			port_num,
			request,
			request_id,
			callback));
	session->m_sock.open(session->m_ep.protocol());
	// Add new session to the list of active sessions so
	// that we can access it if the user decides to cancel
	// the corresponding request before it completes.
	// Because active sessions list can be accessed from 
	// multiple threads, we guard it with a mutex to avoid 
	// data corruption.
	std::unique_lock<std::mutex> lock(m_active_sessions_guard);

	m_active_sessions[request_id] = session;
	lock.unlock();
	session->m_sock.async_connect(session->m_ep,
		[this, session](const boost::system::error_code& ec)
		{
			if (ec.failed()) {

				std::cout << ec.what() << std::endl;

				std::string str = ec.what();

				session->m_ec = ec;
				onRequestComplete(session);
				return;
			}
	std::unique_lock<std::mutex>cancel_lock(session->m_cancel_guard);

	if (session->m_was_cancelled) {
		onRequestComplete(session);
		return;
	}
	boost::asio::async_write(session->m_sock,
		boost::asio::buffer(session->m_request),
		[this, session](const boost::system::error_code& ec,
			std::size_t bytes_transferred)
		{
			if (ec.failed()) {
				session->m_ec = ec;
				onRequestComplete(session);
				return;
			}
	std::unique_lock<std::mutex> cancel_lock(session->m_cancel_guard);

	if (session->m_was_cancelled) {
		onRequestComplete(session);
		return;
	}
	boost::asio::async_read_until(session->m_sock,
		session->m_response_buf,
		'\n',
		[this, session](const boost::system::error_code& ec,
			std::size_t bytes_transferred)

		{
			if (!ec.failed()) {
				session->m_ec = ec;
			}
			else {
				std::istream strm(&session->m_response_buf);
				std::getline(strm, session->m_response);
			}
	onRequestComplete(session);
		}); }); });
}

void ASyncTCPClient::cancelRequest(unsigned int request_id)
{
	std::unique_lock<std::mutex> lock(m_active_sessions_guard);
	auto it = m_active_sessions.find(request_id);
	if (it != m_active_sessions.end()) 
	{
		std::unique_lock<std::mutex>
			cancel_lock(it->second->m_cancel_guard);
		it->second->m_was_cancelled = true;
		it->second->m_sock.cancel();
	}
}

void ASyncTCPClient::close()
{
	// Destroy work object. This allows the I/O thread to
	// exits the event loop when there are no more pending
	// asynchronous operations. 
	m_work.reset(NULL);
	// Wait for the I/O thread to exit.
	m_thread->join();
}

void ASyncTCPClient::onRequestComplete(std::shared_ptr<Session> session) {
	// Shutting down the connection. This method may
	// fail in case socket is not connected. We don¡¯t care 
	// about the error code if this function fails.
	boost::system::error_code ignored_ec;

	session->m_sock.shutdown(
		boost::asio::ip::tcp::socket::shutdown_both,
		ignored_ec);
	// Remove session form the map of active sessions.
	std::unique_lock<std::mutex> lock(m_active_sessions_guard);

	auto it = m_active_sessions.find(session->m_id);
	if (it != m_active_sessions.end())
		m_active_sessions.erase(it);
	lock.unlock();
	boost::system::error_code ec;
	if (session->m_ec && session->m_was_cancelled)
		ec = boost::asio::error::operation_aborted;
	else
		ec = session->m_ec;

	// Call the callback provided by the user.
	session->m_callback(session->m_id, session->m_response, ec);
};


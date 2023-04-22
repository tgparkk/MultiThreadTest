#pragma once


#include <boost/predef.h> // Tools to identify the OS.
// We need this to enable cancelling of I/O operations on
// Windows XP, Windows Server 2003 and earlier.
// Refer to "http://www.boost.org/doc/libs/1_58_0/
// doc/html/boost_asio/reference/basic_stream_socket/
// cancel/overload1.html" for details.
#ifdef BOOST_OS_WINDOWS
#define _WIN32_WINNT 0x0501
#if _WIN32_WINNT <= 0x0502 // Windows Server 2003 or earlier.
#define BOOST_ASIO_DISABLE_IOCP
#define BOOST_ASIO_ENABLE_CANCELIO 
#endif
#endif

#include <boost/asio.hpp>
#include <thread>
#include <mutex>
#include <memory>
#include <iostream>
#include <boost/noncopyable.hpp>

#include <map>

// Function pointer type that points to the callback
// function which is called when a request is complete.
typedef void(*Callback) (unsigned int request_id,
	const std::string& response,
	const boost::system::error_code& ec);

struct Session {
	Session(boost::asio::io_service& ios,
		const std::string& raw_ip_address,
		unsigned short port_num,
		const std::string& request,
		unsigned int id,
		Callback callback) :
		m_sock(ios),
		m_ep(boost::asio::ip::address::from_string(raw_ip_address),
			port_num),
		m_request(request),
		m_id(id),
		m_callback(callback),
		m_was_cancelled(false) {}
	boost::asio::ip::tcp::socket m_sock; // Socket used for communication
	boost::asio::ip::tcp::endpoint m_ep; // Remote endpoint.
	std::string m_request; // Request string.
	// streambuf where the response will be stored.
	boost::asio::streambuf m_response_buf;
	std::string m_response; // Response represented as a string.
	// Contains the description of an error if one occurs during
	// the request life cycle.
	boost::system::error_code m_ec;
	unsigned int m_id; // Unique ID assigned to the request.
	// Pointer to the function to be called when the request
	// completes.
	Callback m_callback;
	bool m_was_cancelled;
	std::mutex m_cancel_guard;
};

class ASyncTCPClient : public boost::noncopyable
{
public:
	ASyncTCPClient();
	

	void emulateLongComputationOp(
		unsigned int duration_sec,
		const std::string& raw_ip_address,
		unsigned short port_num,
		Callback callback,
		unsigned int request_id);

	// Cancels the request. 
	void cancelRequest(unsigned int request_id);

	void close();

private:
	void onRequestComplete(std::shared_ptr<Session> session);

private:
	boost::asio::io_service m_ios;
	std::map<int, std::shared_ptr<Session>> m_active_sessions;
	std::mutex m_active_sessions_guard;
	std::unique_ptr<boost::asio::io_service::work> m_work;
	std::unique_ptr<std::thread> m_thread;
};




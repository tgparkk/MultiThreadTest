#pragma once

#include<memory>
#include<thread>
#include<mutex>
#include<deque>
#include<optional>
#include<vector>
#include<iostream>
#include<vector>
#include<algorithm>
#include<chrono>
#include<cstdint>

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#define ASIO_STANDALONE


#include <boost/asio.hpp>

#include <boost/system/error_code.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>

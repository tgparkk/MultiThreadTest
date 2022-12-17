#include <iostream>
#include "ThreadPool/ThreadPool.h"
#include "Task/Task1.h"
#include "Server/AsyncServerTest.h"

int main()
{
// 	std::cout << "test" << std::endl;
// 
// 	ThreadPool pool(3);
// 
// 	for (int i = 0; i < 10; i++)
// 	{
// 		pool.EnqueueJob([i]()
// 			{
// 				Task1 t1;
// 		t1.work(i % 3 + 1, i);
// 		t1.work1(i);
// 			});
// 	}

	boost::asio::io_service io_service;

	AsyncServerTest server(io_service);

	io_service.run();

	return 0;
}

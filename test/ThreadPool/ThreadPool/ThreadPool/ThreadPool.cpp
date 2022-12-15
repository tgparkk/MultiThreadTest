#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t num_threads)
	: num_threads_(num_threads), stop_all(false) 
{
	worker_threads_.reserve(num_threads_);
	for (size_t i = 0; i < num_threads_; ++i) 
	{
		worker_threads_.emplace_back([this]() { this->WorkerThread(); });
	}
}

ThreadPool::~ThreadPool() 
{
	stop_all = true;
	cv_job_q_.notify_all();

	for (auto& t : worker_threads_) 
	{
		t.join();
	}
}

void ThreadPool::WorkerThread() 
{
	while (true) 
	{
		// std::lock_guard 는 lock과 unlock 사이에 lock,unlock 사용 불가
		std::unique_lock<std::mutex> lock(m_job_q_);

		cv_job_q_.wait(lock, [this]() { return !jobs_.empty() || stop_all; });
		// 1) Lock을 잡은 상태
		// 2) 조건 확인
		//	- 만족 O => 빠져 나와서 이어서 코드를 진행(락을 획득하고 탈출)
		//	- 만족 X => Lock을 원자적으로 풀고 블락됨(wait(loci))
		//		a) notify_xxx()가 호출되면 스레드가 깨어난다
		//		b) lock을 다시 얻고(wait(lock), 2)부터 반복


		// 주석하면 서버 상태?
		if (stop_all && jobs_.empty())
		{
			return;
		}

		if (!jobs_.empty())
		{
			// 맨 앞의 job 을 뺀다.
			std::function<void()> job = std::move(jobs_.front());
			jobs_.pop();
			lock.unlock();

			// 해당 job 을 수행한다 :)
			job();
		}

	}
}

void ThreadPool::WorkerThread_Promise()
{
	while (true)
	{
		std::promise<void*> p;
		std::future<void*> data = p.get_future();
	}
}

void ThreadPool::EnqueueJob(std::function<void()> job) 
{
	if (stop_all) 
	{
		int temp = 1;
		//throw std::runtime_error("ThreadPool 사용 중지됨");
	}
	
	{
		std::lock_guard<std::mutex> lock(m_job_q_);
		jobs_.push(std::move(job));
	}
	
	// 대기하고 있는 하나의 thread 깨우기
	cv_job_q_.notify_one();
}

void ThreadPool::EnqueueJob_Promise(std::function<void()> job)
{
	if (stop_all)
	{
		int temp = 1;
		//throw std::runtime_error("ThreadPool 사용 중지됨");
	}

}

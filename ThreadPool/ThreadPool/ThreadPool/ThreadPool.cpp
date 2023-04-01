#include "ThreadPool.h"

ThreadPool::ThreadPool(std::size_t num_threads)
	: m_num_threads(num_threads), m_stop_all(false) 
{

	

	m_worker_threads(m_num_threads);
	for (std::size_t i = 0; i < m_num_threads; ++i)
	{
		m_worker_threads.emplace_back([this]() { this->WorkerThread(); });
	}
}

ThreadPool::~ThreadPool() 
{
	m_stop_all = true;
	m_cv_job_q.notify_all();

	for (auto& t : m_worker_threads) 
	{
		t.join();
	}
}

void ThreadPool::WorkerThread() 
{
	while (true) 
	{
		// std::lock_guard 는 lock과 unlock 사이에 lock,unlock 사용 불가
		std::unique_lock<std::mutex> lock(m_job_q);

		m_cv_job_q.wait(lock, [this]() { return !m_jobs.empty() || m_stop_all; });
		// 1) Lock을 잡은 상태
		// 2) 조건 확인
		//	- 만족 O => 빠져 나와서 이어서 코드를 진행(락을 획득하고 탈출)
		//	- 만족 X => Lock을 원자적으로 풀고 블락됨(wait(loci))
		//		a) notify_xxx()가 호출되면 스레드가 깨어난다
		//		b) lock을 다시 얻고(wait(lock), 2)부터 반복


		// 주석하면 서버 상태?
		if (m_stop_all && m_jobs.empty())
		{
			std::cout << "no job" << std::endl;
		}

		if (!m_jobs.empty())
		{
			// 맨 앞의 job 을 뺀다.
			std::function<void()> job = std::move(m_jobs.front());
			m_jobs.pop();
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
	if (m_stop_all) 
	{
		int temp = 1;
		//throw std::runtime_error("ThreadPool 사용 중지됨");
	}
	
	{
		std::lock_guard<std::mutex> lock(m_job_q);
		m_jobs.push(std::move(job));
	}
	
	// 대기하고 있는 하나의 thread 깨우기
	m_cv_job_q.notify_one();
}

void ThreadPool::EnqueueJob_Promise(std::function<void()> job)
{
	if (m_stop_all)
	{
		int temp = 1;
		//throw std::runtime_error("ThreadPool 사용 중지됨");
	}

}

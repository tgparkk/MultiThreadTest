#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <future>

class ThreadPool
{
public:
	ThreadPool(size_t num_threads);
	~ThreadPool();

public:
	void WorkerThread();
	//void WorkerThread_Promise();
	void EnqueueJob(std::function<void()> job);
	//void EnqueueJob_Promise(std::function<void()> job);

private:
	// 총 Worker 쓰레드의 개수.
	size_t num_threads_;
	// Worker 쓰레드를 보관하는 벡터.
	std::vector<std::thread> worker_threads_;

	// 할일들을 보관하는 job 큐.
	std::queue<std::function<void()>> jobs_;

	// deadlock 방지를 위한
	std::condition_variable cv_job_q_;
	std::mutex m_job_q_;

	// 모든 쓰레드 종료
	bool stop_all;
};
#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <future>
#include <iostream>

class ThreadPool
{
public:
	ThreadPool(size_t num_threads);
	~ThreadPool();

public:
	void WorkerThread();
	void WorkerThread_Promise();
	void EnqueueJob(std::function<void()> job);
	void EnqueueJob_Promise(std::function<void()> job);

private:
	// 총 Worker 쓰레드의 개수.
	size_t m_num_threads;
	// Worker 쓰레드를 보관하는 벡터.
	std::vector<std::thread> m_worker_threads;

	// 할일들을 보관하는 job 큐.
	std::queue<std::function<void()>> m_jobs;

	std::condition_variable m_cv_job_q;
	std::mutex m_job_q;

	// 모든 쓰레드 종료
	bool m_stop_all;
};


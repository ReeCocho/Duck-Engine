/**
 * @file thread_pool.cpp
 * @brief Thread pool source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "threading.hpp"

namespace dk
{
	SimulationThread::SimulationThread() : m_running(false), m_stopping(false)
	{
		// Spawn thread
		m_thread = std::thread
		(
			[this]()
			{
				// While we aren't stopping...
				while (true)
				{
					// Wait until we need to run the function
					{
						std::unique_lock<std::mutex> lock(m_running_mutex);
						m_running_condition.wait(lock, [this] { return m_running || m_stopping; });

						if (m_stopping)
							break;
					}

					// Run it.
					m_function();

					{
						std::unique_lock<std::mutex> lock(m_running_mutex);
						m_running = false;
						m_running_condition.notify_one();
					}
				}
			}
		);
	}

	SimulationThread::SimulationThread(std::function<void()> func) : SimulationThread()
	{
		m_function = std::move(func);
	}

	SimulationThread::~SimulationThread()
	{
		{
			std::unique_lock<std::mutex> lock(m_running_mutex);
			m_stopping = true;
			m_running_condition.notify_one();
		}

		m_thread.join();
	}

	void SimulationThread::start()
	{
		wait();

		std::unique_lock<std::mutex> lock(m_running_mutex);
		m_running = true;
		m_running_condition.notify_one();
	}

	void SimulationThread::wait()
	{
		std::unique_lock<std::mutex> lock(m_running_mutex);
		m_running_condition.wait(lock, [this]() { return !m_running; });
	}



	WorkerThread::WorkerThread()
	{
		m_thread = std::thread(&WorkerThread::work, this);
	}

	WorkerThread::~WorkerThread()
	{
		if (m_thread.joinable())
		{
			wait();

			{
				std::lock_guard<std::mutex> lock(m_jobs_mutex);
				m_destroying = true;
				m_condition.notify_one();
			}

			m_thread.join();
		}
	}

	void WorkerThread::add_job(std::function<void(void)> job)
	{
		std::lock_guard<std::mutex> lock(m_jobs_mutex);
		m_jobs.push(std::move(job));
		m_condition.notify_one();
	}

	void WorkerThread::add_jobs(const std::vector<std::function<void(void)>>& jobs)
	{
		std::lock_guard<std::mutex> lock(m_jobs_mutex);
		
		for(auto& job : jobs)
			m_jobs.push(std::move(job));

		m_condition.notify_one();
	}

	void WorkerThread::wait()
	{
		std::unique_lock<std::mutex> lock(m_jobs_mutex);
		m_condition.wait(lock, [this]() { return m_jobs.empty(); });
	}

	void WorkerThread::work()
	{
		while (true)
		{
			std::function<void(void)> job;
			{
				std::unique_lock<std::mutex> lock(m_jobs_mutex);
				m_condition.wait(lock, [this] { return !m_jobs.empty() || m_destroying; });

				if (m_destroying)
					break;

				job = m_jobs.front();
			}

			job();

			{
				std::lock_guard<std::mutex> lock(m_jobs_mutex);
				m_jobs.pop();
				m_condition.notify_one();
			}
		}
	}



	ThreadPool::ThreadPool()
	{
		
	}

	ThreadPool::ThreadPool(size_t thread_count)
	{
		workers.resize(thread_count);
		for (size_t i = 0; i < thread_count; ++i)
			workers[i] = new WorkerThread();
	}

	ThreadPool::~ThreadPool()
	{
		wait();

		for (auto worker : workers)
			delete worker;
	}

	void ThreadPool::wait()
	{
		for (auto worker : workers)
			worker->wait();
	}
}
#pragma once

/**
 * @file thread_pool.hpp
 * @brief Thread pool header file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <mutex>
#include <thread>
#include <condition_variable>
#include <vector>
#include <atomic>
#include <functional>
#include <queue>
#include <memory>

namespace dk
{
	/**
	 * A thread that repeats the same function when told to.
	 */
	class SimulationThread
	{
	public:

		/**
		 * Constructor.
		 */
		SimulationThread();

		/**
		 * Constructor.
		 * @param Function to run.
		 */
		SimulationThread(std::function<void()> func);

		/**
		 * Destructor.
		 */
		~SimulationThread();

		/**
		 * Start simulation.
		 */
		void start();

		/**
		 * Wait for the simulation to finish.
		 */
		void wait();

	private:

		/** Thread. */
		std::thread m_thread;

		/** Function to run. */
		std::function<void()> m_function;

		/** Is the thread running? */
		bool m_running;
		std::mutex m_running_mutex;
		std::condition_variable m_running_condition;

		/** Is the thread stopping? */
		bool m_stopping;
	};

	/**
	 * Performs work for a thread pool.
	 * @see ThreadPool
	 */
	class WorkerThread
	{
	public:

		/**
		 * Constructor.
		 */
		WorkerThread();

		/**
		 * Destructor.
		 */
		~WorkerThread();

		/**
		 * Add a job to the worker thread.
		 * @param Job
		 */
		void add_job(std::function<void(void)> job);

		/**
		 * Add a list of jobs.
		 * @param List of jobs.
		 */
		void add_jobs(const std::vector<std::function<void(void)>>& jobs);

		/**
		 * Wait for the worker to finish working.
		 */
		void wait();

	private:

		/** Is the worker thread being destroyed? */
		bool m_destroying = false;

		/** Thread object. */
		std::thread m_thread;

		/** Job queue. */
		std::queue<std::function<void(void)>> m_jobs;

		/** Job queue mutex. */
		std::mutex m_jobs_mutex;

		/** Condition to wait for. */
		std::condition_variable m_condition;

		/**
		 * @brief Work method.
		 */
		void work();
	};

	/**
	 * Manages worker threads.
	 * @see WorkerThread
	 */
	class ThreadPool
	{
	public:

		/**
		 * Constructor.
		 */
		ThreadPool();

		/**
		 * Constructor.
		 * @param Number of worker threads.
		 */
		ThreadPool(size_t thread_count);

		/**
		 * Destructor.
		 */
		~ThreadPool();



		/**
		 * Wait for the thread pool to finish working.
		 */
		void wait();

		/** Worker threads. */
		std::vector<WorkerThread*> workers = {};
	};
}
#include <future>
#include <chrono>

template<typename Fn>
struct one_at_a_time {
	typedef typename std::chrono::steady_clock clock;
	typedef typename std::chrono::duration<clock> timeout_t;
	typedef typename Fn&& async_process_t;

	struct input_track {
		A a;
		std::chrono::time_point<clock> request_stamp;
	};

	std::forward_list<struct input_track> outstanding;
	std::future<bool> timeout_future;
	timeout_t timeout;
	async_process_t begin_processing;

	void process_next(void)
	{
		A& front = this->outstanding.front();
		this->begin_processing(front);
		std::async(std::launch::async, [this]() {
			std::this_thread::sleep_until(front.request_stamp);
			/* expire all the old ones, then sleep again */
		})
	}

	void push(A a)
	{
		this->outstanding.emplace_back(a, clock::now());

		/* XXX: do this atomically WRT the timeout completing */
		if (!this->timeout.valid())
			this->process_next();
	}

	void is_complete(B b)
	{
		/* map to outstanding... */
	}

	void on_complete(A a, B b)
	{
		
	}

	one_at_a_time(async_process_t _begin_processing, timeout_t _timeout)
		: outstanding(),
		, timeout_future()
		, timeout(_timeout)
		, begin_processing(_begin_processing)
	{}
};

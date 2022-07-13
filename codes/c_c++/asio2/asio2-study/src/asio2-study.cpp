#include "asio2-study.h"

int main(void)
{
	// size check
	{
		static_assert(1 == sizeof(int8_t));
		static_assert(1 == sizeof(uint8_t));
		static_assert(2 == sizeof(int16_t));
		static_assert(2 == sizeof(uint16_t));
		static_assert(4 == sizeof(int32_t));
		static_assert(4 == sizeof(uint32_t));
		static_assert(8 == sizeof(int64_t));
		static_assert(8 == sizeof(uint64_t));

		static_assert(4 == sizeof(float));
		static_assert(8 == sizeof(double));

		static_assert(8 == sizeof(time_t));
		static_assert(8 == sizeof(size_t));
		static_assert(8 == sizeof(ptrdiff_t));
		static_assert(8 == sizeof(intptr_t));
	}

	time_t systemSeconds = get_time_system<std::chrono::seconds>();

	struct tm curGMTM {};
	struct tm curLocalTM {};

	time_t startTime = get_time_steady<std::chrono::microseconds>();
	safe_localtime(&systemSeconds, &curGMTM);
	time_t endTime = get_time_steady<std::chrono::microseconds>();
	time_t diffTime = endTime - startTime;
	fmt::print("safe_localtime() cost {} microseconds!\n", diffTime);

	startTime = get_time_steady<std::chrono::microseconds>();
	safe_gmtime(&systemSeconds, &curLocalTM);
	endTime = get_time_steady<std::chrono::microseconds>();
	diffTime = endTime - startTime;
	fmt::print("safe_gmtime() cost {} microseconds!\n", diffTime);

	return 0;
}
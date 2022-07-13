#include "base/ev_atomic.h"

#if defined(__EV_WINDOWS__)
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #undef WIN32_LEAN_AND_MEAN
#endif // defined(__EV_WINDOWS__)

namespace evpp {

int32 ev_atomic_increment_and_fetch(ev_atomic_int32* val)
{
#if defined(__EV_WINDOWS__)
	return static_cast<int32>(InterlockedIncrement((volatile LONG*)val));
#elif defined(__EV_LINUX__)
	return static_cast<int32>(__sync_add_and_fetch(val, 1));
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)
}

int64 ev_atomic_increment_and_fetch(ev_atomic_int64* val)
{
#if defined(__EV_WINDOWS__)
	return static_cast<int64>(InterlockedIncrement64((volatile LONGLONG*)val));
#elif defined(__EV_LINUX__)
	return static_cast<int64>(__sync_add_and_fetch(val, 1));
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)
}

int32 ev_atomic_fetch_and_increment(ev_atomic_int32* val)
{
#if defined(__EV_WINDOWS__)
	return static_cast<int32>(InterlockedExchangeAdd((volatile LONG*)val, 1));
#elif defined(__EV_LINUX__)
	return static_cast<int32>(__sync_fetch_and_add(val, 1));
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)
}

int64 ev_atomic_fetch_and_increment(ev_atomic_int64* val)
{
#if defined(__EV_WINDOWS__)
	return static_cast<int64>(InterlockedExchangeAdd64((volatile LONGLONG*)val, 1));
#elif defined(__EV_LINUX__)
	return static_cast<int64>(__sync_fetch_and_add(val, 1));
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)
}

int32 ev_atomic_decrement_and_fetch(ev_atomic_int32* val)
{
#if defined(__EV_WINDOWS__)
	return static_cast<int32>(InterlockedDecrement((volatile LONG*)val));
#elif defined(__EV_LINUX__)
	return static_cast<int32>(__sync_sub_and_fetch(val, 1));
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)
}

int64 ev_atomic_decrement_and_fetch(ev_atomic_int64* val)
{
#if defined(__EV_WINDOWS__)
	return static_cast<int64>(InterlockedDecrement64((volatile LONGLONG*)val));
#elif defined(__EV_LINUX__)
	return static_cast<int64>(__sync_sub_and_fetch(val, 1));
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)
}

int32 ev_atomic_fetch_and_decrement(ev_atomic_int32* val)
{
#if defined(__EV_WINDOWS__)
	return static_cast<int32>(InterlockedExchangeAdd((volatile LONG*)val, -1));
#elif defined(__EV_LINUX__)
	return static_cast<int32>(__sync_fetch_and_sub(val, 1));
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)
}

int64 ev_atomic_fetch_and_decrement(ev_atomic_int64* val)
{
#if defined(__EV_WINDOWS__)
	return static_cast<int64>(InterlockedExchangeAdd64((volatile LONGLONG*)val, -1));
#elif defined(__EV_LINUX__)
	return static_cast<int64>(__sync_fetch_and_sub(val, 1));
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)
}

int32 ev_atomic_fetch_and_add(ev_atomic_int32* target, int32 val)
{
#if defined(__EV_WINDOWS__)
	return static_cast<int32>(InterlockedExchangeAdd((volatile LONG*)target, val));
#elif defined(__EV_LINUX__)
	return static_cast<int32>(__sync_fetch_and_add(target, val));
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)
}

int64 ev_atomic_fetch_and_add(ev_atomic_int64* target, int64 val)
{
#if defined(__EV_WINDOWS__)
	return static_cast<int64>(InterlockedExchangeAdd64((volatile LONGLONG*)target, val));
#elif defined(__EV_LINUX__)
	return static_cast<int64>(__sync_fetch_and_add(target, val));
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)
}

int32 ev_atomic_compare_and_swap(ev_atomic_int32* target, int32 old_value, int32 new_value)
{
#if defined(__EV_WINDOWS__)
	return static_cast<int32>(InterlockedCompareExchange((volatile LONG*)target, new_value, old_value));
#elif defined(__EV_LINUX__)
	return static_cast<int32>(__sync_val_compare_and_swap(target, old_value, new_value));
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)
}

int64 ev_atomic_compare_and_swap(ev_atomic_int64* target, int64 old_value, int64 new_value)
{
#if defined(__EV_WINDOWS__)
	return static_cast<int64>(InterlockedCompareExchange64((volatile LONGLONG*)target, new_value, old_value));
#elif defined(__EV_LINUX__)
	return static_cast<int64>(__sync_val_compare_and_swap(target, old_value, new_value));
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)
}

bool ev_atomic_test_nonzero_and_increment(ev_atomic_int32* target)
{
	while (true)
	{
		int32 temp = ev_atomic_get_value(target);
		if (0 == temp)
		{
			break;
		}

		if (temp == ev_atomic_compare_and_swap(target, temp, temp + 1))
		{
			return true;
		}
	}

	return false;
}

bool ev_atomic_test_nonzero_and_increment(ev_atomic_int64* target)
{
	while (true)
	{
		int64 temp = ev_atomic_get_value(target);
		if (0 == temp)
		{
			break;
		}

		if (temp == ev_atomic_compare_and_swap(target, temp, temp + 1))
		{
			return true;
		}
	}

	return false;
}

bool ev_atomic_test_zero_and_increment(ev_atomic_int32* target)
{
	while (true)
	{
		int32 temp = ev_atomic_get_value(target);
		if (0 != temp)
		{
			break;
		}

		if (temp == ev_atomic_compare_and_swap(target, temp, 1))
		{
			return true;
		}
	}

	return false;
}

bool ev_atomic_test_zero_and_increment(ev_atomic_int64* target)
{
	while (true)
	{
		int64 temp = ev_atomic_get_value(target);
		if (0 != temp)
		{
			break;
		}

		if (temp == ev_atomic_compare_and_swap(target, temp, 1))
		{
			return true;
		}
	}

	return false;
}

} // namespace evpp
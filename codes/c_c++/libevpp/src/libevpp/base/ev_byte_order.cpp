#include "base/ev_byte_order.h"

namespace evpp    {
namespace details {

/**
 * @brief 获取当前计算机的字节序
 * @param
 * @return
 */
static inline ev_byte_order::byte_order ev_get_native_byte_order_internal(void)
{
#pragma pack(1)
	union
	{
		uint16 s;
		uint8  c[2];
	} un;
#pragma pack()

	un.s = 0x0201;
	if (2 == un.c[0] && 1 == un.c[1])
	{
		return ev_byte_order::BO_BIG;
	}
	else if (1 == un.c[0] && 2 == un.c[1])
	{
		return ev_byte_order::BO_LITTLE;
	}
	else
	{
		return ev_byte_order::BO_UNKNOWN;
	}
}

/**
 * @brief 反转字节
 * @param
 * @return
 */
static inline uint16 ev_flip_bytes_internal(uint16 val)
{
	return (
		((val >> 8) & 0x00FF) |
		((val << 8) & 0xFF00) );
}

static inline uint32 ev_flip_bytes_internal(uint32 val)
{
	return (
		((val >> 24) & 0x000000FFU) |
		((val >>  8) & 0x0000FF00U) |
		((val <<  8) & 0x00FF0000U) |
		((val << 24) & 0xFF000000U) );
}

static inline uint64 ev_flip_bytes_internal(uint64 val)
{
#if defined(__EV_WINDOWS__)
	return (
		((val >> 56) & 0x00000000000000FFULL) |
		((val >> 40) & 0x000000000000FF00ULL) |
		((val >> 24) & 0x0000000000FF0000ULL) |
		((val >>  8) & 0x00000000FF000000ULL) |
		((val <<  8) & 0x000000FF00000000ULL) |
		((val << 24) & 0x0000FF0000000000ULL) |
		((val << 40) & 0x00FF000000000000ULL) |
		((val << 56) & 0xFF00000000000000ULL) );
#else
	#if defined(__EV_WORDSIZE32__)
		return (
			((val >> 56) & 0x00000000000000FFULL) |
			((val >> 40) & 0x000000000000FF00ULL) |
			((val >> 24) & 0x0000000000FF0000ULL) |
			((val >>  8) & 0x00000000FF000000ULL) |
			((val <<  8) & 0x000000FF00000000ULL) |
			((val << 24) & 0x0000FF0000000000ULL) |
			((val << 40) & 0x00FF000000000000ULL) |
			((val << 56) & 0xFF00000000000000ULL) );
	#else
		return (
			((val >> 56) & 0x00000000000000FFUL) |
			((val >> 40) & 0x000000000000FF00UL) |
			((val >> 24) & 0x0000000000FF0000UL) |
			((val >>  8) & 0x00000000FF000000UL) |
			((val <<  8) & 0x000000FF00000000UL) |
			((val << 24) & 0x0000FF0000000000UL) |
			((val << 40) & 0x00FF000000000000UL) |
			((val << 56) & 0xFF00000000000000UL) );
	#endif // defined(__EV_WORDSIZE32__)
#endif // defined(__EV_WINDOWS__)
}

/**
 * @brief 字节序转换
 * @param
 * @return 成功返回对应转换的值，失败返回-1
 */
template<typename T>
static inline T ev_ntoh_internal(T val)
{
	ev_byte_order::byte_order order = ev_byte_order::ev_get_byte_order();
	if (ev_byte_order::BO_LITTLE == order)
	{
		return ev_flip_bytes_internal(val);
	}
	else if (ev_byte_order::BO_BIG == order)
	{
		return val;
	}
	else
	{
		return static_cast<T>(-1);
	}
}

/**
 * @brief 字节序转换
 * @param
 * @return
 */
template<typename T>
static inline T ev_hton_internal(T val)
{
	return ev_ntoh_internal<T>(val);
}

} // namespace details

//////////////////////////////////////////////////////////////////////////
// ev_byte_order
//////////////////////////////////////////////////////////////////////////
ev_byte_order::byte_order ev_byte_order::ev_get_byte_order(void)
{
	static byte_order native_byte_order = details::ev_get_native_byte_order_internal();
	return native_byte_order;
}

int16 ev_byte_order::ntoh16(int16 val)
{
	return static_cast<int16>(details::ev_ntoh_internal<uint16>(static_cast<uint16>(val)));
}

uint16 ev_byte_order::ntoh16(uint16 val)
{
	return details::ev_ntoh_internal<uint16>(val);
}

int32 ev_byte_order::ntoh32(int32 val)
{
	return static_cast<int32>(details::ev_ntoh_internal<uint32>(static_cast<uint32>(val)));
}

uint32 ev_byte_order::ntoh32(uint32 val)
{
	return details::ev_ntoh_internal<uint32>(val);
}

int64 ev_byte_order::ntoh64(int64 val)
{
	return static_cast<int64>(details::ev_ntoh_internal<uint64>(static_cast<uint64>(val)));
}

uint64 ev_byte_order::ntoh64(uint64 val)
{
	return details::ev_ntoh_internal<uint64>(val);
}

int16 ev_byte_order::hton16(int16 val)
{
	return static_cast<int16>(details::ev_hton_internal<uint16>(static_cast<uint16>(val)));
}

uint16 ev_byte_order::hton16(uint16 val)
{
	return details::ev_hton_internal<uint16>(val);
}

int32 ev_byte_order::hton32(int32 val)
{
	return static_cast<int32>(details::ev_hton_internal<uint32>(static_cast<uint32>(val)));
}

uint32 ev_byte_order::hton32(uint32 val)
{
	return details::ev_hton_internal<uint32>(val);
}

int64 ev_byte_order::hton64(int64 val)
{
	return static_cast<int64>(details::ev_hton_internal<uint64>(static_cast<uint64>(val)));
}

uint64 ev_byte_order::hton64(uint64 val)
{
	return details::ev_hton_internal<uint64>(val);
}

} // namespace evpp
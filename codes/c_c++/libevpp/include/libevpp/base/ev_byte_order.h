//////////////////////////////////////////////////////////////////////////
// 字节序
//////////////////////////////////////////////////////////////////////////
#ifndef __EV_BYTE_ORDER_H__
#define __EV_BYTE_ORDER_H__

#include "base/ev_types.h"

namespace evpp {

//////////////////////////////////////////////////////////////////////////
// ev_byte_order
//////////////////////////////////////////////////////////////////////////
class ev_byte_order
{
public:
	/* 字节序类型 */
	typedef enum
	{
		BO_UNKNOWN = 0,  // 未知类型
		BO_BIG     = 1,  // 大端序
		BO_LITTLE  = 2,  // 小端序
	}byte_order;

public:
	/**
	 * @brief 获取当前计算机的字节序
	 * @param
	 * @return
	 */
	static byte_order ev_get_byte_order(void);

	/**
	 * @brief 将网络字节序转换为主机字节序
	 * @param
	 * @return 成功返回对应转换的值，失败返回-1
	 */
	static int16  ntoh16(int16  val);
	static uint16 ntoh16(uint16 val);

	static int32  ntoh32(int32  val);
	static uint32 ntoh32(uint32 val);

	static int64  ntoh64(int64  val);
	static uint64 ntoh64(uint64 val);

	/**
	 * @brief 将主机字节序转换为网络字节序
	 * @param
	 * @return 成功返回对应转换的值，失败返回-1
	 */
	static int16  hton16(int16  val);
	static uint16 hton16(uint16 val);

	static int32  hton32(int32  val);
	static uint32 hton32(uint32 val);

	static int64  hton64(int64  val);
	static uint64 hton64(uint64 val);
};

} // namespace evpp

#endif // __EV_BYTE_ORDER_H__
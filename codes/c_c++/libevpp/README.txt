【说明】
1. 更新至libevent-2.1.12稳定版

【源码修改记录】
1. 整理event-config.h头文件，将不同平台的配置头文件分开，目前已添加平台配置头文件：
    event-config-windows.h
    event-config-linux.h

2. 修改_WIN32_WINNT为0x0600(Windows Vista及以上，包含Windows Server 2008及以上)
    evutil.c
    listener.c
    event_iocp.c
    evthread_win32.c

3. 添加#pragma warning(disable : 4244)以忽略转型警告
    buffer.c
    evutil_time.c

4. 修改evutil_time.c中函数evutil_gettimeofday中的两个变量(GetSystemTimePreciseAsFileTime_fn和check_precise)定义位置
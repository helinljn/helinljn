【说明】
	1. 更新至libevent-2.1.12稳定版

【源码修改记录】
1. 整理include/event2/event-config.h头文件，将不同平台的配置头文件分开，目前已添加平台配置头文件：
	event-config-windows.h
	event-config-linux.h

2. 整理src/evconfig-private.h头文件，将不同平台的配置头文件分开，目前已添加平台配置头文件：
	evconfig-private-windows.h
	evconfig-private-linux.h
#############################################################################################
#Makefile的常规编写流程
#注意：在不改变路径的情况下(不使用cd命令)，当前的路径永远是Makefile所在的路径

#############################################################################################
#0. 预定义编译版本，默认编译为Release版本
#   该步骤无需添加任何内容，保留当前即可
build_version = release

#############################################################################################
#1. 预定义变量用来存储所有的*.c, *.cpp, *.o, *.d
#   该步骤无需添加任何内容，保留当前即可
c_sources := 
c_objects := 
c_depends := 

cpp_sources := 
cpp_objects := 
cpp_depends := 

#############################################################################################
#2. 预定义各种调用的shell命令
#   该步骤无需添加任何内容，保留当前即可
rm_command := rm -rf

#############################################################################################
#3. 定义g++的编译选项
#   该步骤只需修改对应的模式即可，不能同时打开，二者选一
#
#   g++ -g(默认为-g2)选项：
#     -g1 : 只可以进行回溯跟踪和堆栈转储，不包含局部变量和行号相关的信息
#     -g2 : 包含级别1中所有的调试信息，同时包括符号表、行号、局部变量和外部变量信息
#     -g3 : 包含级别2中所有的调试信息，同时包括源代码中定义的宏
ifeq ($(build_version), debug)
# Debug版
build_flags := -O0 -g -Wall -Wextra -c -fmessage-length=0 -std=c++03
else ifeq ($(build_version), release)
# Release版
build_flags := -O2 -g -Wall -Wextra -c -fmessage-length=0 -std=c++03 -DNDEBUG
else
$(error Please make sure build_version flag is one of debug and release, build_version=$(build_version))
endif

#############################################################################################
#4. 定义编译所依赖的头文件路径
#例如：
#includes := -I./ -I../aaa/bbb -I../ccc/ddd
includes := -I./libevent -I../include/libevent -I./libevpp -I../include/libevpp

#############################################################################################
#5. 添加编译所依赖的静态库的路径和需要链接的静态库
#注意：编库的时候，一般不需要依赖其它库，所以下面两个变量基本为空
#例如：
#libs_path := 
#libs      := 

#############################################################################################
#6. 定义本次编译的库的输出地址
#例如：
#output_bin := ./output_bin
output_bin := ../lib

#############################################################################################
#7. 定义本次编译的库的名字
ifeq ($(build_version), debug)
# Debug版
build_target := libevppd.a
else ifeq ($(build_version), release)
# Release版
build_target := libevpp.a
else
$(error Please make sure build_version flag is one of debug and release, build_version=$(build_version))
endif

#############################################################################################
#8. 开始编译前的准备，主要用于执行一些shell命令
#例如：
#$(shell mkdir -p $(output_bin))
$(shell mkdir -p $(output_bin))

#############################################################################################
#9. 将一个目录下所有的源文件编译为对应的目标文件
#注意：如果当前路径下有多个子目录，并且每个子目录都有*.c | *.cpp源文件
#      那么就要分别对每个子目录采用这种方式全部编译一遍
#例如：有如下目录层级
#        example_dir1(目录)          example_dir2(目录)          Makefile(Makefile文件)
#             |                           |
#      test1.c test2.c              test3.cpp test4.cpp
#
# 编译example_dir1目录下的*.c源文件
#example_dir1 := $(wildcard example_dir1/*.c)
#c_sources    += $(patsubst %.c, %.c,   $(example_dir1))
#c_objects    += $(patsubst %.c, %.o,   $(example_dir1))
#c_depends    += $(patsubst %.c, %.d,   $(example_dir1))
#
#example_dir1/%.o: example_dir1/%.c
#	@echo ''
#	@echo 'Building file: $<'
#	@echo 'Invoking: Cross GCC Compiler'
#	gcc $(includes) $(build_flags) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
#	@echo 'Finished building: $<'
#
# 编译example_dir2目录下的*.cpp源文件
#example_dir2 := $(wildcard example_dir2/*.cpp)
#cpp_sources    += $(patsubst %.cpp, %.cpp, $(example_dir2))
#cpp_objects    += $(patsubst %.cpp, %.o,   $(example_dir2))
#cpp_depends    += $(patsubst %.cpp, %.d,   $(example_dir2))
#
#example_dir2/%.o: example_dir2/%.cpp
#	@echo ''
#	@echo 'Building file: $<'
#	@echo 'Invoking: Cross G++ Compiler'
#	g++ $(includes) $(build_flags) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
#	@echo 'Finished building: $<'

# 编译./libevent目录下的*.c源文件
src_libevent_c := ./libevent/buffer.c \
./libevent/bufferevent.c \
./libevent/bufferevent_filter.c \
./libevent/bufferevent_pair.c \
./libevent/bufferevent_ratelim.c \
./libevent/bufferevent_sock.c \
./libevent/epoll.c \
./libevent/evdns.c \
./libevent/event.c \
./libevent/event_tagging.c \
./libevent/evmap.c \
./libevent/evrpc.c \
./libevent/evthread.c \
./libevent/evthread_pthread.c \
./libevent/evutil.c \
./libevent/evutil_rand.c \
./libevent/evutil_time.c \
./libevent/http.c \
./libevent/listener.c \
./libevent/log.c \
./libevent/poll.c \
./libevent/select.c \
./libevent/signal.c \
./libevent/strlcpy.c 

c_sources += $(patsubst %.c, %.c, $(src_libevent_c))
c_objects += $(patsubst %.c, %.o, $(src_libevent_c))
c_depends += $(patsubst %.c, %.d, $(src_libevent_c))

./libevent/%.o: ./libevent/%.c
	@echo ''
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc $(includes) $(build_flags) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'

# 编译./libevpp/base目录下的*.cpp源文件
src_libevpp_base_cpp := $(wildcard ./libevpp/base/*.cpp)
cpp_sources          += $(patsubst %.cpp, %.cpp, $(src_libevpp_base_cpp))
cpp_objects          += $(patsubst %.cpp, %.o,   $(src_libevpp_base_cpp))
cpp_depends          += $(patsubst %.cpp, %.d,   $(src_libevpp_base_cpp))

./libevpp/base/%.o: ./libevpp/base/%.cpp
	@echo ''
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ $(includes) $(build_flags) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'

# 编译./libevpp/net目录下的*.cpp源文件
src_libevpp_net_cpp := $(wildcard ./libevpp/net/*.cpp)
cpp_sources         += $(patsubst %.cpp, %.cpp, $(src_libevpp_net_cpp))
cpp_objects         += $(patsubst %.cpp, %.o,   $(src_libevpp_net_cpp))
cpp_depends         += $(patsubst %.cpp, %.d,   $(src_libevpp_net_cpp))

./libevpp/net/%.o: ./libevpp/net/%.cpp
	@echo ''
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ $(includes) $(build_flags) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'

#############################################################################################
#10. 生成静态库
#   该步骤无需添加任何内容，保留当前即可
all: $(output_bin)/$(build_target)

$(output_bin)/$(build_target): $(c_objects) $(cpp_objects)
	@echo ''
	@echo 'Building target: $@'
	@echo 'Invoking: Cross GCC Archiver'
	ar -r $(output_bin)/$(build_target) $(c_objects) $(cpp_objects)
	@echo 'Finished building target: $@'
	@echo ''

#############################################################################################
#11. 清理所有编译输出
#   该步骤无需添加任何内容，保留当前即可
clean:
	-$(rm_command) $(output_bin)/$(build_target) $(c_objects) $(c_depends) $(cpp_objects) $(cpp_depends)

.PHONY: all clean dependents
.SECONDARY:

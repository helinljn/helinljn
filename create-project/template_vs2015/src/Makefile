#############################################################################################
#Makefile的常规编写流程
#注意：在不改变路径的情况下(不使用cd命令)，当前的路径永远是Makefile所在的路径

#############################################################################################
#0. 预定义编译版本，默认编译为Release版本
#   该步骤无需添加任何内容，保留当前即可
build_version = release

#############################################################################################
#1. 预定义三个变量用来存储所有的*.cpp, *.o, *.d
#   该步骤无需添加任何内容，保留当前即可
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
build_flags := -O0 -g -Wall -Wextra -c -fmessage-length=0 -std=c++11
else ifeq ($(build_version), release)
# Release版
build_flags := -O2 -g -Wall -Wextra -c -fmessage-length=0 -std=c++11 -DNDEBUG
else
$(error Please make sure build_version flag is one of debug and release, build_version=$(build_version))
endif

#############################################################################################
#4. 定义编译所依赖的头文件路径
#例如：
#cpp_includes := -I./ -I../aaa/bbb -I../ccc/ddd
cpp_includes := -I../include

#############################################################################################
#5. 添加编译所依赖的静态库的路径和需要链接的静态库
#注意：使用gcc/g++编译的时候，因为编译器的库依赖是从右往左的
#      所依赖的库越基础越往右边放，此处甚为关键，有坑，需注意
ifeq ($(build_version), debug)
# Debug版
libs_path :=
libs      := -pthread
else ifeq ($(build_version), release)
# Release版
libs_path :=
libs      := -pthread
else
$(error Please make sure build_version flag is one of debug and release, build_version=$(build_version))
endif

#############################################################################################
#6. 定义本次编译的可执行二进制文件的输出地址
#例如：
#output_bin := ./output_bin
output_bin := ../output_bin

#############################################################################################
#7. 定义本次编译的可执行二进制文件的名字
ifeq ($(build_version), debug)
# Debug版
build_target := templated
else ifeq ($(build_version), release)
# Release版
build_target := template
else
$(error Please make sure build_version flag is one of debug and release, build_version=$(build_version))
endif

#############################################################################################
#8. 开始编译前的准备，主要用于执行一些shell命令
#例如：
#$(shell mkdir -p $(output_bin))
$(shell mkdir -p $(output_bin))

#############################################################################################
#9. 将一个目录下所有的*.cpp文件编译为对应的*.o文件
#注意：如果当前路径下有多个子目录，并且每个子目录都有*.cpp源文件
#      那么就要分别对每个子目录采用这种方式全部编译一遍
#例如：有如下目录层级
#        example_dir1(目录)          example_dir2(目录)          Makefile(Makefile文件)
#             |                           |
#    test1.cpp test2.cpp         test3.cpp test4.cpp
#
# 编译example_dir1目录下的*.cpp源文件
#example_dir1 := $(wildcard example_dir1/*.cpp)
#cpp_sources  += $(patsubst %.cpp, %.cpp, $(example_dir1))
#cpp_objects  += $(patsubst %.cpp, %.o,   $(example_dir1))
#cpp_depends  += $(patsubst %.cpp, %.d,   $(example_dir1))
#
#example_dir1/%.o: example_dir1/%.cpp
#	@echo ''
#	@echo 'Building file: $<'
#	@echo 'Invoking: Cross G++ Compiler'
#	g++ $(cpp_includes) $(build_flags) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
#	@echo 'Finished building: $<'
#
# 编译example_dir2目录下的*.cpp源文件
#example_dir2 := $(wildcard example_dir2/*.cpp)
#cpp_sources  += $(patsubst %.cpp, %.cpp, $(example_dir2))
#cpp_objects  += $(patsubst %.cpp, %.o,   $(example_dir2))
#cpp_depends  += $(patsubst %.cpp, %.d,   $(example_dir2))
#
#example_dir2/%.o: example_dir2/%.cpp
#	@echo ''
#	@echo 'Building file: $<'
#	@echo 'Invoking: Cross G++ Compiler'
#	g++ $(cpp_includes) $(build_flags) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
#	@echo 'Finished building: $<'

# 编译./目录下的*.cpp源文件
src_dir     := $(wildcard ./*.cpp)
cpp_sources += $(patsubst %.cpp, %.cpp, $(src_dir))
cpp_objects += $(patsubst %.cpp, %.o,   $(src_dir))
cpp_depends += $(patsubst %.cpp, %.d,   $(src_dir))

./%.o: ./%.cpp
	@echo ''
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ $(cpp_includes) $(build_flags) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'

#############################################################################################
#10. 生成可执行文件
#   该步骤无需添加任何内容，保留当前即可
all: $(output_bin)/$(build_target)

$(output_bin)/$(build_target): $(cpp_objects)
	@echo ''
	@echo 'Building target: $@'
	@echo 'Invoking: Cross G++ Linker'
	g++ -o $(output_bin)/$(build_target) $(cpp_objects) $(libs_path) $(libs)
	@echo 'Finished building target: $@'
	@echo ''

#############################################################################################
#11. 清理所有编译输出
#   该步骤无需添加任何内容，保留当前即可
clean:
	-$(rm_command) $(output_bin)/$(build_target) $(cpp_objects) $(cpp_depends)

.PHONY: all clean dependents
.SECONDARY:

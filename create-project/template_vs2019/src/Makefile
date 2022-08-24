#############################################################################################
#Makefile的常规编写流程
#注意：在不改变路径的情况下(不使用cd命令)，当前的路径永远是Makefile所在的路径

#############################################################################################
#0. 预定义编译类型，默认编译为Release版本
build_type = release

#############################################################################################
#1. 预定义六个变量用来存储所有的*.c, *.cpp, *.o, *.d
c_sources   :=
c_objects   :=
c_depends   :=

cpp_sources := 
cpp_objects := 
cpp_depends := 

#############################################################################################
#2. 预定义各种调用的shell命令
rm_command := rm -rf

#############################################################################################
#3. 预定义gcc & g++的编译选项
#
#   gcc & g++ -g(默认为-g2)选项：
#     -g1 : 只可以进行回溯跟踪和堆栈转储，不包含局部变量和行号相关的信息
#     -g2 : 包含级别1中所有的调试信息，同时包括符号表、行号、局部变量和外部变量信息
#     -g3 : 包含级别2中所有的调试信息，同时包括源代码中定义的宏
ifeq ($(build_type), debug)
	# Debug
	c_build_flags   := -O0 -g -c -std=c99 -Wall -Wextra -Wpedantic
	cpp_build_flags := -O0 -g -c -std=c++17 -Wall -Wextra -Wpedantic
else ifeq ($(build_type), release)
	# Release
	c_build_flags   := -O3 -g -c -std=c99 -Wall -Wextra -Wpedantic -DNDEBUG
	cpp_build_flags := -O3 -g -c -std=c++17 -Wall -Wextra -Wpedantic -DNDEBUG
else
	$(error Please make sure build_type flag is one of debug and release, build_type=$(build_type))
endif

#############################################################################################
#4. 添加编译所依赖的头文件路径
#例如：
#	common_includes_path := -I./ -I../aaa/bbb -I../ccc/ddd
ifeq ($(build_type), debug)
	# Debug
	common_includes_path := -I../include
else
	# Release
	common_includes_path := -I../include
endif

#############################################################################################
#5. 添加编译所依赖的静态库的路径和需要链接的静态库
#注意：使用gcc/g++编译的时候，因为编译器的库依赖是从右往左的
#      所依赖的库越基础越往右边放，此处甚为关键，有坑，需注意
#例如：
#	common_libs_path := -L../libaaa_dir -L../libbbb_dir
#	common_libs      := -laaa -lbbb -pthread
ifeq ($(build_type), debug)
	# Debug
	common_libs_path :=
	common_libs      := -pthread
else
	# Release
	common_libs_path :=
	common_libs      := -pthread
endif

#############################################################################################
#6. 定义本次编译的可执行文件的名字
ifeq ($(build_type), debug)
	# Debug
	build_target := template_d
else
	# Release
	build_target := template_r
endif

#############################################################################################
#7. 定义本次编译的可执行文件的输出目录
#例如：
#	output_dir := ./output
output_dir := ../output

#############################################################################################
#8. 开始编译前的准备，主要用于执行一些shell命令
#例如：
#	$(shell mkdir -p $(output_dir))
$(shell mkdir -p $(output_dir))

#############################################################################################
#9. 将一个目录下所有的*.c & *.cpp文件编译为对应的*.o文件
#注意：如果当前路径下有多个子目录，并且每个子目录都有*.c & *.cpp源文件
#      那么就要分别对每个子目录采用这种方式全部编译一遍
#例如：有如下目录层级
#        example_dir1(目录)          example_dir2(目录)          Makefile(Makefile文件)
#             |                           |
#      test1.c test2.cpp           test3.c test4.cpp
#
# 编译example_dir1目录下的*.c源文件
#example_dir1_c := $(wildcard example_dir1/*.c)
#c_sources      += $(patsubst %.c, %.c, $(example_dir1_c))
#c_objects      += $(patsubst %.c, %.o, $(example_dir1_c))
#c_depends      += $(patsubst %.c, %.d, $(example_dir1_c))
#
#example_dir1/%.o: example_dir1/%.c
#	@echo ''
#	@echo 'Building file: $<'
#	@echo 'Invoking: Cross GCC Compiler'
#	gcc $(common_includes_path) $(c_build_flags) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
#	@echo 'Finished building: $<'
#
# 编译example_dir1目录下的*.cpp源文件
#example_dir1_cpp := $(wildcard example_dir1/*.cpp)
#cpp_sources      += $(patsubst %.cpp, %.cpp, $(example_dir1_cpp))
#cpp_objects      += $(patsubst %.cpp, %.o,   $(example_dir1_cpp))
#cpp_depends      += $(patsubst %.cpp, %.d,   $(example_dir1_cpp))
#
#example_dir1/%.o: example_dir1/%.cpp
#	@echo ''
#	@echo 'Building file: $<'
#	@echo 'Invoking: Cross G++ Compiler'
#	g++ $(common_includes_path) $(cpp_build_flags) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
#	@echo 'Finished building: $<'
#
# 编译example_dir2目录下的*.c源文件
#example_dir2_c := $(wildcard example_dir2/*.c)
#c_sources      += $(patsubst %.c, %.c, $(example_dir2_c))
#c_objects      += $(patsubst %.c, %.o, $(example_dir2_c))
#c_depends      += $(patsubst %.c, %.d, $(example_dir2_c))
#
#example_dir2/%.o: example_dir2/%.c
#	@echo ''
#	@echo 'Building file: $<'
#	@echo 'Invoking: Cross GCC Compiler'
#	gcc $(common_includes_path) $(c_build_flags) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
#	@echo 'Finished building: $<'
#
# 编译example_dir2目录下的*.cpp源文件
#example_dir2_cpp := $(wildcard example_dir2/*.cpp)
#cpp_sources      += $(patsubst %.cpp, %.cpp, $(example_dir2_cpp))
#cpp_objects      += $(patsubst %.cpp, %.o,   $(example_dir2_cpp))
#cpp_depends      += $(patsubst %.cpp, %.d,   $(example_dir2_cpp))
#
#example_dir2/%.o: example_dir2/%.cpp
#	@echo ''
#	@echo 'Building file: $<'
#	@echo 'Invoking: Cross G++ Compiler'
#	g++ $(common_includes_path) $(cpp_build_flags) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
#	@echo 'Finished building: $<'

# 编译./目录下的*.c源文件
current_dir_c := $(wildcard ./*.c)
c_sources     += $(patsubst %.c, %.c, $(current_dir_c))
c_objects     += $(patsubst %.c, %.o, $(current_dir_c))
c_depends     += $(patsubst %.c, %.d, $(current_dir_c))

./%.o: ./%.c
	@echo ''
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc $(common_includes_path) $(c_build_flags) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'

# 编译./目录下的*.cpp源文件
current_dir_cpp := $(wildcard ./*.cpp)
cpp_sources     += $(patsubst %.cpp, %.cpp, $(current_dir_cpp))
cpp_objects     += $(patsubst %.cpp, %.o,   $(current_dir_cpp))
cpp_depends     += $(patsubst %.cpp, %.d,   $(current_dir_cpp))

./%.o: ./%.cpp
	@echo ''
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ $(common_includes_path) $(cpp_build_flags) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'

#############################################################################################
#10. 生成可执行文件
all: $(output_dir)/$(build_target)

$(output_dir)/$(build_target): $(c_objects) $(cpp_objects)
	@echo ''
	@echo 'Building target: $@'
	@echo 'Invoking: Cross G++ Linker'
	g++ -o $(output_dir)/$(build_target) $(c_objects) $(cpp_objects) $(common_libs_path) $(common_libs)
	@echo 'Finished building target: $@'
	@echo ''

#############################################################################################
#11. 清理所有编译输出
clean:
	-$(rm_command) $(output_dir)/$(build_target) $(c_objects) $(c_depends) $(cpp_objects) $(cpp_depends)

.PHONY: all clean dependents
.SECONDARY:

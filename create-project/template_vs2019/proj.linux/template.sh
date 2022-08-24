#!/bin/bash

##########################################################################
#确定当前编译是debug模式，还是release模式
##########################################################################
xx_build_mode=""

if [ $# -ne 1 ]
then
    echo "Usage:./template.sh [debug | release | all]"
    exit
fi

if   [ "$1" = "debug" ]; then
	xx_build_mode="debug"
elif [ "$1" = "release" ]; then
	xx_build_mode="release"
elif [ "$1" = "all" ]; then
	xx_build_mode="all"
else
    echo "Usage:./template.sh [debug | release | all]"
	exit
fi

##########################################################################
#编译template
##########################################################################
cd ../src

if   [ "$xx_build_mode" = "debug" ]; then
	make -f Makefile build_type=debug clean
	make -f Makefile build_type=debug
elif [ "$xx_build_mode" = "release" ]; then
	make -f Makefile clean
	make -f Makefile
else
	make -f Makefile build_type=debug clean
	make -f Makefile build_type=debug
	
	make -f Makefile clean
	make -f Makefile
fi

#清理中间文件
find . -name "*.o" | xargs rm -rf;
find . -name "*.d" | xargs rm -rf;

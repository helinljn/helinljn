# 删除指定目录下的所有指定扩展名的文件
# find ${TARGET_DIR} -name "*.${FILE_EXTENSION}" | xargs rm -rf
MACRO(REMOVE_FILES TARGET_DIR FILE_EXTENSION)
    # 获取匹配指定模式的文件列表
    FILE(GLOB_RECURSE FILES "${TARGET_DIR}/*.${FILE_EXTENSION}")

    # 删除文件
    FOREACH(FILE ${FILES})
        EXECUTE_PROCESS(
            COMMAND ${CMAKE_COMMAND} -E remove ${FILE}
        )
    ENDFOREACH()
ENDMACRO()

# 外部调用方法：
# ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -DARG1=XXX_DIR -DARG2=exp -P ${CMAKE_PROJECT_ROOT_DIR}/cmake/remove_files.cmake
# )
REMOVE_FILES(${ARG1} ${ARG2})
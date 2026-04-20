# 项目名字
SET(CURRENT_TARGET_NAME opencc_data)

# 查找Python解释器
FIND_PACKAGE(Python3 REQUIRED COMPONENTS Interpreter)

# 目录
SET(OPENCC_DATA_SOURCE_DIR        ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/data)
SET(OPENCC_DATA_CONFIG_SOURCE_DIR ${OPENCC_DATA_SOURCE_DIR}/config)
SET(OPENCC_DATA_DICT_SOURCE_DIR   ${OPENCC_DATA_SOURCE_DIR}/dictionary)
SET(OPENCC_DATA_SCRIPT_SOURCE_DIR ${OPENCC_DATA_SOURCE_DIR}/scripts)

SET(OPENCC_DATA_ROOT_DIR          ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}/data)
SET(OPENCC_DATA_CONFIG_OUTPUT_DIR ${OPENCC_DATA_ROOT_DIR}/config)
SET(OPENCC_DATA_DICT_OUTPUT_DIR   ${OPENCC_DATA_ROOT_DIR}/dictionary)
SET(OPENCC_DATA_GENERATED_DIR     ${OPENCC_DATA_ROOT_DIR}/opencc_data)

# 可执行文件
SET(OPENCC_DICT_BIN $<TARGET_FILE:opencc_dict>)

# 原始词典
SET(DICTS_RAW
    STCharacters
    STPhrases
    TSCharacters
    TSPhrases
    TWPhrases
    TWPhrasesRev
    TWVariants
    TWVariantsRevPhrases
    HKVariants
    HKVariantsRevPhrases
    JPVariants
    JPShinjitaiCharacters
    JPShinjitaiPhrases
)

# 自动生成词典
SET(DICTS_GENERATED
    TWVariantsRev
    HKVariantsRev
    JPVariantsRev
)

# 全部词典
SET(DICTS ${DICTS_RAW} ${DICTS_GENERATED})

# config文件
SET(CONFIG_FILES
    hk2s.json
    hk2t.json
    jp2t.json
    s2hk.json
    s2t.json
    s2tw.json
    s2twp.json
    t2hk.json
    t2jp.json
    t2s.json
    t2tw.json
    tw2s.json
    tw2sp.json
    tw2t.json
)

# 自动生成词典的源文件
SET(DICT_TWVariantsRev_GENERATING_INPUT ${OPENCC_DATA_DICT_SOURCE_DIR}/TWVariants.txt)
SET(DICT_HKVariantsRev_GENERATING_INPUT ${OPENCC_DATA_DICT_SOURCE_DIR}/HKVariants.txt)
SET(DICT_JPVariantsRev_GENERATING_INPUT ${OPENCC_DATA_DICT_SOURCE_DIR}/JPVariants.txt)

# 原始词典输入文件
FOREACH(DICT ${DICTS_RAW})
    SET(DICT_${DICT}_INPUT ${OPENCC_DATA_DICT_SOURCE_DIR}/${DICT}.txt)
ENDFOREACH()

# 自动生成词典输入文件
FOREACH(DICT ${DICTS_GENERATED})
    SET(DICT_${DICT}_INPUT ${OPENCC_DATA_GENERATED_DIR}/${DICT}.txt)
ENDFOREACH()

# 生成自动反向词典txt
FOREACH(DICT ${DICTS_GENERATED})
    ADD_CUSTOM_COMMAND(
        OUTPUT
            ${DICT_${DICT}_INPUT}
        COMMENT
            "Generating ${DICT}.txt"
        COMMAND
            ${CMAKE_COMMAND} -E make_directory ${OPENCC_DATA_GENERATED_DIR}
        COMMAND
            ${Python3_EXECUTABLE} ${OPENCC_DATA_SCRIPT_SOURCE_DIR}/reverse.py ${DICT_${DICT}_GENERATING_INPUT} ${DICT_${DICT}_INPUT}
        DEPENDS
            ${DICT_${DICT}_GENERATING_INPUT}
            ${OPENCC_DATA_SCRIPT_SOURCE_DIR}/reverse.py
            ${OPENCC_DATA_SCRIPT_SOURCE_DIR}/common.py
        VERBATIM
    )
ENDFOREACH()

# 生成词典二进制文件
FOREACH(DICT ${DICTS})
    SET(DICT_${DICT}_OUTPUT ${OPENCC_DATA_DICT_OUTPUT_DIR}/${DICT}.ocd2)

    ADD_CUSTOM_COMMAND(
        OUTPUT
            ${DICT_${DICT}_OUTPUT}
        COMMENT
            "Building ${DICT}.ocd2"
        COMMAND
            ${CMAKE_COMMAND} -E make_directory ${OPENCC_DATA_DICT_OUTPUT_DIR}
        COMMAND
            ${OPENCC_DICT_BIN}
                --input ${DICT_${DICT}_INPUT}
                --output ${DICT_${DICT}_OUTPUT}
                --from text
                --to ocd2
        DEPENDS
            opencc_dict
            ${DICT_${DICT}_INPUT}
        VERBATIM
    )

    SET(DICT_TARGETS ${DICT_TARGETS} ${DICT_${DICT}_OUTPUT})
ENDFOREACH()

# 复制config文件
FOREACH(CONFIG_FILE ${CONFIG_FILES})
    SET(CONFIG_INPUT  ${OPENCC_DATA_CONFIG_SOURCE_DIR}/${CONFIG_FILE})
    SET(CONFIG_OUTPUT ${OPENCC_DATA_CONFIG_OUTPUT_DIR}/${CONFIG_FILE})

    ADD_CUSTOM_COMMAND(
        OUTPUT
            ${CONFIG_OUTPUT}
        COMMENT
            "Copying ${CONFIG_FILE}"
        COMMAND
            ${CMAKE_COMMAND} -E make_directory ${OPENCC_DATA_CONFIG_OUTPUT_DIR}
        COMMAND
            ${CMAKE_COMMAND} -E copy_if_different ${CONFIG_INPUT} ${CONFIG_OUTPUT}
        DEPENDS
            ${CONFIG_INPUT}
        VERBATIM
    )

    SET(CONFIG_TARGETS ${CONFIG_TARGETS} ${CONFIG_OUTPUT})
ENDFOREACH()

# 一键构建OpenCC数据
ADD_CUSTOM_TARGET(
    ${CURRENT_TARGET_NAME}
    ALL
    DEPENDS
        ${DICT_TARGETS}
        ${CONFIG_TARGETS}
)

# 其它设置
IF(WIN32)
    # VS工程设置
    SET_PROPERTY(TARGET ${CURRENT_TARGET_NAME} PROPERTY FOLDER "cmake")
ENDIF()
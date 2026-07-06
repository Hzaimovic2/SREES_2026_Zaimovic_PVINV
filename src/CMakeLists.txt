cmake_minimum_required(VERSION 3.20)

project(srees_solar_inverter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(SREES_USE_NATID "Build with natID matrix/plugin integration" OFF)
option(SREES_BUILD_PLUGIN "Build dTwin plugin DLL" OFF)
set(NATID_SDK_ROOT "" CACHE PATH "Path to natID.SDK folder")
set(SREES_BUILD_ROOT ${CMAKE_CURRENT_BINARY_DIR})

set(SREES_COMMON_SOURCES
    srees/converter.cpp
    srees/dtwin_model_writer.cpp
    srees/power_case.cpp
    srees/solar_inverter.cpp
    srees/xml_config.cpp
)

if(SREES_USE_NATID)
    if(NOT NATID_SDK_ROOT)
        message(FATAL_ERROR "Set NATID_SDK_ROOT to the natID.SDK folder when SREES_USE_NATID=ON")
    endif()

    include(${NATID_SDK_ROOT}/DevEnv/Common.cmake)
    include(${NATID_SDK_ROOT}/DevEnv/MatrixLib.cmake)
    if(SREES_BUILD_PLUGIN)
        include(${NATID_SDK_ROOT}/DevEnv/natGUI.cmake)
    endif()
    list(APPEND SREES_COMMON_SOURCES srees/matrix_adapter_natid.cpp)
else()
    if(SREES_BUILD_PLUGIN)
        message(FATAL_ERROR "SREES_BUILD_PLUGIN requires SREES_USE_NATID=ON and NATID_SDK_ROOT")
    endif()
    list(APPEND SREES_COMMON_SOURCES srees/matrix_adapter.cpp)
endif()

add_executable(srees_solar_inverter
    main.cpp
    ${SREES_COMMON_SOURCES}
)

target_include_directories(srees_solar_inverter PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/natid_compat
)

if(SREES_USE_NATID)
    target_compile_definitions(srees_solar_inverter PRIVATE SREES_USE_NATID=1)
    target_link_libraries(srees_solar_inverter
        debug ${MU_LIB_DEBUG} optimized ${MU_LIB_RELEASE}
        debug ${MATRIX_LIB_DEBUG} optimized ${MATRIX_LIB_RELEASE}
    )
    if(WIN32)
        set_target_properties(srees_solar_inverter PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_DEBUG "${SREES_BUILD_ROOT}/Debug"
            RUNTIME_OUTPUT_DIRECTORY_RELEASE "${SREES_BUILD_ROOT}/Release"
            LIBRARY_OUTPUT_DIRECTORY_DEBUG "${SREES_BUILD_ROOT}/Debug"
            LIBRARY_OUTPUT_DIRECTORY_RELEASE "${SREES_BUILD_ROOT}/Release"
            ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${SREES_BUILD_ROOT}/Debug"
            ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${SREES_BUILD_ROOT}/Release"
            VS_DEBUGGER_ENVIRONMENT "PATH=${NATID_SDK_BIN};%PATH%;"
        )
    endif()
endif()

if(SREES_BUILD_PLUGIN)
    add_library(srees_pvinv_plugin SHARED
        plugin/solar_inverter_plugin.cpp
        ${SREES_COMMON_SOURCES}
    )

    target_include_directories(srees_pvinv_plugin PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/natid_compat
    )

    target_compile_definitions(srees_pvinv_plugin PRIVATE PLUGIN_EXPORTS)
    target_link_libraries(srees_pvinv_plugin
        debug ${MU_LIB_DEBUG} optimized ${MU_LIB_RELEASE}
        debug ${MATRIX_LIB_DEBUG} optimized ${MATRIX_LIB_RELEASE}
        debug ${NATGUI_LIB_DEBUG} optimized ${NATGUI_LIB_RELEASE}
    )

    set_target_properties(srees_pvinv_plugin PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${SREES_BUILD_ROOT}/Debug"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${SREES_BUILD_ROOT}/Release"
        LIBRARY_OUTPUT_DIRECTORY_DEBUG "${SREES_BUILD_ROOT}/Debug"
        LIBRARY_OUTPUT_DIRECTORY_RELEASE "${SREES_BUILD_ROOT}/Release"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${SREES_BUILD_ROOT}/Debug"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${SREES_BUILD_ROOT}/Release"
        VS_DEBUGGER_ENVIRONMENT "PATH=${NATID_SDK_BIN};%PATH%;"
    )
endif()

if(MSVC)
    target_compile_options(srees_solar_inverter PRIVATE /W4)
else()
    target_compile_options(srees_solar_inverter PRIVATE -Wall -Wextra -Wpedantic)
endif()

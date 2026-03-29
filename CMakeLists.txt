cmake_minimum_required(VERSION 3.10)
project(wcdb-demo CXX)

set(CMAKE_CXX_STANDARD 14)

# 指向 WCDB 编译产物目录（即 wcdb/src/build）
if(NOT DEFINED WCDB_BUILD_DIR)
  set(WCDB_BUILD_DIR "/Users/stephen/Lab/github/wcdb/src/build")
endif()
get_filename_component(WCDB_BUILD_DIR "${WCDB_BUILD_DIR}" ABSOLUTE)

# WCDB.framework 路径
set(WCDB_FRAMEWORK "${WCDB_BUILD_DIR}/WCDB.framework")

if(NOT EXISTS "${WCDB_FRAMEWORK}")
  message(FATAL_ERROR
    "WCDB.framework not found at ${WCDB_FRAMEWORK}. "
    "Build WCDB first (see README.md), then run cmake with -DWCDB_BUILD_DIR=/path/to/wcdb/src/build")
endif()

add_executable(wcdb-demo
  main.cpp
  Sample.cpp
  PinyinSample.cpp
  ChineseFtsSample.cpp
)

target_include_directories(wcdb-demo PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${WCDB_FRAMEWORK}/Headers
  ${WCDB_FRAMEWORK}/Versions/Current/Headers
)

# 链接 WCDB.framework，macOS 上建议同时链接 Foundation（WCDB 底层可能依赖）
target_link_libraries(wcdb-demo
  "-framework WCDB"
  "-framework Foundation"
  "-F ${WCDB_BUILD_DIR}"
)

target_compile_options(wcdb-demo PRIVATE
  -F "${WCDB_BUILD_DIR}"
)

# 让可执行文件在运行时找到 WCDB.framework（开发时可用 RPATH）
set_target_properties(wcdb-demo PROPERTIES
  BUILD_RPATH "${WCDB_BUILD_DIR}"
  INSTALL_RPATH "@executable_path"
)

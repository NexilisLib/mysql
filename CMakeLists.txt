cmake_minimum_required(VERSION 3.30)
project(mysql_cpp_wrapper)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 20)

# clang
#set(CMAKE_C_COMPILER "clang")
#set(CMAKE_CXX_COMPILER "clang++")

# MySQL C API
find_library(MYSQL_CLIENT_LIBRARY mysqlclient)

if(MYSQL_CLIENT_LIBRARY)
    add_definitions(-DHAS_MYSQL_CLIENT_LIBRARY)
    message("MySQL client library found")
else()
    message("MySQL client library not found")
endif()

add_compile_options(-Wall -Wextra -pedantic)

add_executable(mysql_cpp main.cc database.cc)
target_link_libraries(mysql_cpp PRIVATE ${MYSQL_CLIENT_LIBRARY})

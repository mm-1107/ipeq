cmake_minimum_required(VERSION 3.18)
project(FEDB
       	LANGUAGES C)

add_library(util util.c)
add_library(tpch tpch.c)

add_executable(main main.c)
target_link_libraries(main tpch util gmp cifer m)

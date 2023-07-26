cmake_minimum_required(VERSION 3.18)
project(FEDB
       	LANGUAGES C CXX)

add_library(logistic logistic.cpp)
# add_library(adult adult.c)

add_executable(main main.c)
target_link_libraries(main logistic gmp cifer m)

cmake_minimum_required(VERSION 3.26)
project(Vector)          
set(CMAKE_CXX_STANDARD 20)    


add_executable(Vector main.cpp vector.h iterator.h reverse_iterator.h 
               back_insert_iterator.h allocator.h)

cmake_minimum_required(VERSION 3.15)
project(OpenMPTest)

set(CMAKE_CXX_COMPILER g++)

find_package(OpenMP REQUIRED)

if(OpenMP_FOUND)
    message(STATUS "OpenMP found: ${OpenMP_CXX_FLAGS}")  # C++ bayrakları gösterir

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}") # C++ için
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

add_executable(OpenMPTest main.cpp) # hedef adı ve kaynak dosyası

target_link_libraries(OpenMPTest OpenMP::OpenMP_CXX) # C++ için doğru kütüphane
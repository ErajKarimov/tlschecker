cmake_minimum_required(VERSION 3.23)
project(Tls_Checker)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the path to your static library and include directory
set(LIBRARY_DIR /home/kali/Desktop/tls/chilkat-9.5.0-x86_64-linux-gcc/lib)
set(INCLUDE_DIR chilkat-9.5.0-x86_64-linux-gcc/include/)  # Adjust to your actual include directory

# Add the directory containing the library and include files
include_directories(${INCLUDE_DIR})

# Define the executable target
add_executable(my_program main.cpp)

# Link the static library to the executable
target_link_libraries(my_program ${LIBRARY_DIR}/libchilkat.a)

cmake_minimum_required(VERSION 3.10)
project(network_protocols C)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -O2")

# Find RDMA / ibverbs
find_library(IBVERBS_LIB ibverbs REQUIRED)

# Define source directory
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Collect all source files (except main.c)
file(GLOB TCP_SOURCES "${SRC_DIR}/tcp_*.c")
file(GLOB RDMA_SOURCES "${SRC_DIR}/rdma_*.c")
file(GLOB AMD_SOURCES "${SRC_DIR}/amd_*.c")

# Create a library for RDMA and TCP functions
add_library(protocols STATIC ${TCP_SOURCES} ${RDMA_SOURCES})

# Link the RDMA library
target_link_libraries(protocols ${IBVERBS_LIB})

# AMD GPU Library
add_library(amd_gpu STATIC ${AMD_SOURCES})

# Main executable (calls all protocols)
add_executable(main "${SRC_DIR}/main.c")
target_link_libraries(main protocols amd_gpu ${IBVERBS_LIB})

# Debugging output
message(STATUS "Collected TCP sources: ${TCP_SOURCES}")
message(STATUS "Collected RDMA sources: ${RDMA_SOURCES}")
message(STATUS "Collected AMD sources: ${AMD_SOURCES}")

cmake_minimum_required(VERSION 3.10)

project(nerdminer-rpi VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include header directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Set executable output path
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# Find required packages
find_package(Boost REQUIRED COMPONENTS system)
find_package(OpenSSL REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)

# Check for and add tests subdirectory
if(EXISTS ${PROJECT_SOURCE_DIR}/tests)
    enable_testing()
    add_subdirectory(tests)
else()
    message(WARNING "Tests directory not found.")
endif()

# Add main executable with all source files
add_executable(nerdminer
    src/main.cpp
    src/miner_job.cpp
    src/miner_session.cpp
    src/nerdminer_block.cpp
    src/stratum/stratum_client.cpp
)

# Raspberry Pi 4 (aarch64) specific flags
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
    message(STATUS "Configuring for Raspberry Pi 4 (arm64)")
    target_compile_options(nerdminer PRIVATE -O3 -march=armv8-a)
endif()

# Compile definitions
target_compile_definitions(nerdminer PRIVATE PROJECT_NAME=\"nerdminer-rpi\")

# Link libraries
target_link_libraries(nerdminer PRIVATE
    Boost::system
    OpenSSL::Crypto
    nlohmann_json::nlohmann_json
    pthread
)

# Add test
add_test(NAME TestNerdMiner COMMAND nerdminer)
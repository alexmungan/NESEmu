cmake_minimum_required(VERSION 3.5)
project(NESEmu)

set(CMAKE_CXX_STANDARD 20)

# Option to control whether we download Google Test (to prevent re-downloading every time)
set(GTEST_DOWNLOAD OFF CACHE BOOL "Download Google Test" FORCE)

# Source files for the emulator executable (EXCLUDES main.cpp)
set(SOURCE_FILES
        NES.cpp
        NES.hpp
        CPU.cpp
        CPU.hpp
        SystemBus.cpp
        SystemBus.hpp
        Cartridge.cpp
        Cartridge.hpp
        Constants.hpp
)

# Source files for test executable
set(TEST_FILES
        ${SOURCE_FILES}
        test.cpp
)

# Add an option to control whether to enable the tests
option(ENABLE_TESTS "Enable tests" ON)

# Download Google Test via FetchContent only if not already present
if (NOT GTEST_DOWNLOAD)
    include(FetchContent)
    FetchContent_Declare(
            gtest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG release-1.11.0  # Specify a version
    )

    # Make Google Test available (will be downloaded only once)
    FetchContent_MakeAvailable(gtest)
endif()

# Set up the test executable
if (ENABLE_TESTS)
    add_executable(test_NESEmu ${TEST_FILES})
    target_link_libraries(test_NESEmu gtest gtest_main pthread)
endif()

# Set up the normal emulator executable
if(!ENABLE_TESTS)
    add_executable(NESEmu main.cpp ${SOURCE_FILES})
endif()

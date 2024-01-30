cmake_minimum_required(VERSION 3.27)
project(horcrux)

set(CMAKE_CXX_STANDARD 20)

enable_testing()

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

FetchContent_MakeAvailable(googletest)

include_directories(asio/include logic networking)

set(COMMON
        logic/Horcrux.hpp
        logic/Horcrux.cpp
        logic/Utils.cpp
        logic/Utils.hpp
        networking/Requests.hpp
)

add_executable(horcrux client.cpp ${COMMON}
        test/UtilsTests.cpp)
add_executable(server server.cpp ${COMMON})

add_executable(
        tests
        ${COMMON}
        test/CoreLogicTests.cpp
        test/NetworkingHelpersTests.cpp
        test/UtilsTests.cpp
)

target_link_libraries(
        tests
        GTest::gtest_main
)


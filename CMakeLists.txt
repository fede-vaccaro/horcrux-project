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

add_executable(horcrux main.cpp ${COMMON})
add_executable(server server.cpp ${COMMON})

add_executable(
        hello_test
        test/CoreLogicTests.cc
        ${COMMON}
)

target_link_libraries(
        hello_test
        GTest::gtest_main
)


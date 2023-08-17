cmake_minimum_required(VERSION 3.26)
project(libinputTest C)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_C_STANDARD 23)
find_package(Libudev)
find_package(Libinput)
add_executable(libinputTest main.c)
target_link_libraries(libinputTest PUBLIC
        Libinput::Libinput
        ${Libudev_LIBRARIES}
)

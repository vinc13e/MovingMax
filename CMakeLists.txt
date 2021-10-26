cmake_minimum_required(VERSION 3.20)
project(MovingMax)

set(CMAKE_CXX_STANDARD 20)


########################################################################
# RabbitMQ

include(ExternalProject)
set(AMQPCPP_ROOT "${CMAKE_CURRENT_BINARY_DIR}/amqpcpp")
ExternalProject_Add(libamqpcpp
        URL "https://github.com/hoxnox/AMQP-CPP/archive/v2.5.1-nx2.tar.gz"
        URL_MD5 "fcfbd25c03eecde4e4b0dfa58598a426"
        CMAKE_ARGS -Wno-dev "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>"
        INSTALL_DIR ${AMQPCPP_ROOT}
        )
set(AMQPCPP_INCLUDE_DIR ${AMQPCPP_ROOT}/include)
set(AMQPCPP_LIBRARIES ${AMQPCPP_ROOT}/lib/libamqp-cpp.a)
message(STATUS "Found external dependency AMQP-CPP: " ${AMQPCPP_INCLUDE_DIR})
include_directories(${AMQPCPP_INCLUDE_DIR})
list(APPEND LIBRARIES ${AMQPCPP_LIBRARIES})
list(APPEND DEPENDENCIES libamqpcpp)

########################################################################
# libevent

if (WITH_SYSTEM_LIBEVENT)
    find_package(Event REQUIRED)
    add_custom_target(libevent DEPENDS ${Event_INCLUDE_DIR}/event2/event.h)
else()
    set(Event_ROOT "${CMAKE_CURRENT_BINARY_DIR}/libevent")
    ExternalProject_Add(libevent
            URL "https://github.com/libevent/libevent/archive/release-2.0.22-stable.tar.gz"
            URL_MD5 "8913ef56ec329f2c046007bd634c7201"
            PATCH_COMMAND <SOURCE_DIR>/autogen.sh
            CONFIGURE_COMMAND <SOURCE_DIR>/configure --disable-shared --enable-static
            --disable-openssl --disable-libevent-regress --prefix <INSTALL_DIR>
            INSTALL_DIR ${Event_ROOT}
            )
    set(Event_INCLUDE_DIR ${Event_ROOT}/include)
    set(Event_LIBRARIES ${Event_ROOT}/lib/libevent_core.a)
    message(STATUS "Found external dependency Event: " ${Event_INCLUDE_DIR})
endif()
include_directories(${Event_INCLUDE_DIR})
list(APPEND LIBRARIES ${Event_LIBRARIES})
list(APPEND DEPENDENCIES libevent)

########################################################################
# threads

find_package(Threads)
list(APPEND LIBRARIES ${CMAKE_THREAD_LIBS_INIT})
########################################################################


########################################################################
# Json and amqt.cpp
include_directories(include)
########################################################################


add_executable(MovingMax src/main.cpp src/MovingMax.cpp src/MovingMax.h)
target_link_libraries(MovingMax ${LIBRARIES})
add_dependencies(MovingMax ${DEPENDENCIES})
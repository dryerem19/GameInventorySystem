cmake_minimum_required(VERSION 3.5)

project(inventory_system LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(inventory_system main.cpp
    item.h
    storage.h storage.cpp )

include(GNUInstallDirs)
install(TARGETS inventory_system
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

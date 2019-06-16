cmake_minimum_required(VERSION 3.14)
project(CppND_System_Monitor)

set(CMAKE_CXX_STANDARD 17)


include_directories(.)

add_executable(CppND_System_Monitor
        main.cpp
        util.cpp
        ProcessParser.cpp
        SysInfo.cpp
        Process.cpp
        ProcessContainer.cpp
)
set(CMAKE_CXX_FLAGS "-lncurses")
target_link_libraries(${PROJECT_NAME} ncurses)
cmake_minimum_required(VERSION 3.10)
project(dag_executor)

set(CMAKE_CXX_STANDARD 17)

find_package(yaml-cpp REQUIRED)

add_executable(dag_executor
    main.cpp

    config/parser.cpp

    core/job.cpp
    core/semaphoreManager.cpp
    core/validator.cpp
    core/scheduler.cpp
)

target_include_directories(dag_executor PRIVATE .)
target_link_libraries(dag_executor PRIVATE yaml-cpp)

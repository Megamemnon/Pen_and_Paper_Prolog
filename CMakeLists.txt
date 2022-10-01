cmake_minimum_required(VERSION 3.0.0)
project(ppp VERSION 0.2.0)

include(CTest)
enable_testing()

add_executable(ppp main.c ppp.c utils.c)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

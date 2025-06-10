cmake_minimum_required(VERSION 3.30)
project(ics2024 C)

set(CMAKE_C_STANDARD 11)

add_executable(ics2024 main.c)

add_executable(nemu-main nemu/src/nemu-main.c)
add_executable(expr nemu/src/monitor/sdb/expr.c)
add_executable(sdb nemu/src/monitor/sdb/sdb.c)
add_executable(gen-expr nemu/tools/gen-expr/gen-expr.c)
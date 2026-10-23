cmake_minimum_required(VERSION 4.3)
project(XARV)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(SOURCES
	main.c
	shell.c
	commands.c
)

add_executable(XARV ${SOURCES})
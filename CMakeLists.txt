cmake_minimum_required(VERSION 4.3)
project(XARV)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(SOURCES
	sqlite3.c
	main.c
	shell.c
	log.c
	app.c
)

add_executable(XARV ${SOURCES})
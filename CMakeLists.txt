cmake_minimum_required(VERSION 3.4.1)

set(SRC_MAIN_DIR src/main/cpp)

include_directories(${SRC_MAIN_DIR}/sqlite-amalgamation-3140200/ ${SRC_MAIN_DIR}/)

set(SOURCE_FILES
        ${SRC_MAIN_DIR}/ContentValues.cpp
        ${SRC_MAIN_DIR}/Cursor.cpp
        ${SRC_MAIN_DIR}/Database.cpp
        ${SRC_MAIN_DIR}/Util.cpp
        ${SRC_MAIN_DIR}/sqlite-amalgamation-3140200/sqlite3.c)

add_definitions(-DSQLITE_ENABLE_RTREE)
#add_definitions(-DSQLITE_ENABLE_FTS4)
add_definitions(-DSQLITE_ENABLE_FTS5)
#add_definitions(-DSQLITE_ENABLE_JSON1)

if (ANDROID)
    set(SOURCE_FILES
            ${SRC_MAIN_DIR}/MadSqlite.cpp
            ${SOURCE_FILES})
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
    set(SOURCE_FILES
            ${SRC_MAIN_DIR}/Main.cpp
            ${SOURCE_FILES})
endif ()

if (ANDROID)
    add_library(madsqlite-lib SHARED ${SOURCE_FILES})
    find_library(log-lib log)
    target_link_libraries(madsqlite-lib ${log-lib})
else ()
    add_executable(madsqlite-lib ${SOURCE_FILES})
    target_link_libraries(madsqlite-lib)
endif ()
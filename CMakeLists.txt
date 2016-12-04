cmake_minimum_required(VERSION 3.4.1)

set(SRC_MAIN_DIR src/main/cpp)

include_directories(${SRC_MAIN_DIR}/sqlite-amalgamation/ ${SRC_MAIN_DIR}/)

set(SOURCE_FILES
        ${SRC_MAIN_DIR}/MadConstants.hpp
        ${SRC_MAIN_DIR}/MadContentValues.cpp
        ${SRC_MAIN_DIR}/MadQuery.cpp
        ${SRC_MAIN_DIR}/MadDatabase.cpp
        ${SRC_MAIN_DIR}/MadUtil.hpp
        ${SRC_MAIN_DIR}/sqlite-amalgamation/sqlite3.c)

add_definitions(-DSQLITE_ENABLE_RTREE)
#add_definitions(-DSQLITE_ENABLE_FTS4)
add_definitions(-DSQLITE_ENABLE_FTS5)
#add_definitions(-DSQLITE_ENABLE_JSON1)

if (ANDROID)
    set(SOURCE_FILES
            ${SRC_MAIN_DIR}/MadSqliteJni.cpp
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

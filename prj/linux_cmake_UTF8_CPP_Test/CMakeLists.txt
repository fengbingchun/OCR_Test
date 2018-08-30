PROJECT(UTF8_CPP_Test)
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

# support C++11
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# support C++14, when gcc version > 5.1, use -std=c++14 instead of c++1y
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -O2")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -O2")

MESSAGE(STATUS "project source dir: ${PROJECT_SOURCE_DIR}")
SET(PATH_TEST_FILES ${PROJECT_SOURCE_DIR}/./../../demo/UTF8_CPP_Test)
SET(PATH_SRC_FILES ${PROJECT_SOURCE_DIR}/./../../src/UTF8_CPP)
MESSAGE(STATUS "path src files: ${PATH_SRC_FILES}")

# head file search path
INCLUDE_DIRECTORIES(
	${PATH_TEST_FILES}
	${PATH_SRC_FILES}
)

# recursive query match files :*.cpp, *c
FILE(GLOB_RECURSE TEST_CPP_LIST ${PATH_TEST_FILES}/*.cpp)

# build executable program
ADD_EXECUTABLE(UTF8_CPP_Test ${TEST_CPP_LIST})
# add dependent library: static and dynamic
TARGET_LINK_LIBRARIES(UTF8_CPP_Test freetype pthread)


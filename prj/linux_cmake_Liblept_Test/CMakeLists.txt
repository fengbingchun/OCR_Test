PROJECT(Liblept_Test)
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

# support C++11
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# support C++14, when gcc version > 5.1, use -std=c++14 instead of c++1y
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -O2")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -g -Wall -O2")

MESSAGE(STATUS "project source dir: ${PROJECT_SOURCE_DIR}")
SET(PATH_TEST_FILES ${PROJECT_SOURCE_DIR}/./../../demo/Liblept_Test)
SET(PATH_SRC_ZLIB_FILES ${PROJECT_SOURCE_DIR}/./../../src/zlib/zlib-1.2.8)
SET(PATH_SRC_LIBPNG_FILES ${PROJECT_SOURCE_DIR}/./../../src/libpng/lpng1616)
SET(PATH_SRC_LIBJPEG_FILES ${PROJECT_SOURCE_DIR}/./../../src/libjpeg/jpeg-9a)
SET(PATH_SRC_LIBTIFF_FILES ${PROJECT_SOURCE_DIR}/./../../src/libtiff/tiff-4.0.3)
SET(PATH_SRC_GIFLIB_FILES ${PROJECT_SOURCE_DIR}/./../../src/giflib/giflib-5.1.1)
SET(PATH_SRC_LIBLEPT_FILES ${PROJECT_SOURCE_DIR}/./../../src/leptonica-1.71)
MESSAGE(STATUS "path zlib src files: ${PATH_SRC_ZLIB_FILES}")

# head file search path
INCLUDE_DIRECTORIES(
	${PATH_TEST_FILES}
	${PATH_SRC_ZLIB_FILES}
	${PATH_SRC_LIBPNG_FILES}
	${PATH_SRC_LIBJPEG_FILES}
	${PATH_SRC_LIBTIFF_FILES}/libtiff
	${PATH_SRC_GIFLIB_FILES}/lib
	${PATH_SRC_LIBLEPT_FILES}/src
)

# recursive query match files :*.cpp
FILE(GLOB_RECURSE TEST_CPP_LIST ${PATH_TEST_FILES}/*.cpp)

# find library
FIND_LIBRARY(libz NAMES z PATHS ${PROJECT_SOURCE_DIR}/build NO_DEFAULT_PATH)
FIND_LIBRARY(libjpeg NAMES jpeg PATHS ${PROJECT_SOURCE_DIR}/build NO_DEFAULT_PATH)
FIND_LIBRARY(libtiff NAMES tiff PATHS ${PROJECT_SOURCE_DIR}/build NO_DEFAULT_PATH)
FIND_LIBRARY(libgif NAMES gif PATHS ${PROJECT_SOURCE_DIR}/build NO_DEFAULT_PATH)
FIND_LIBRARY(libpng NAMES png PATHS ${PROJECT_SOURCE_DIR}/build NO_DEFAULT_PATH)
FIND_LIBRARY(liblept NAMES lept PATHS ${PROJECT_SOURCE_DIR}/build NO_DEFAULT_PATH)
MESSAGE(STATUS "image libraries: ${libz} ${libjpeg} ${libtiff} ${libgif} ${libpng} ${liblept}")

# build executable program
ADD_EXECUTABLE(Liblept_Test ${TEST_CPP_LIST})

# add dependent library: static and dynamic
# Wrong JPEG library version: library is 90, caller expects 80, so use system jpeg library
# libpng warning: Application built with libpng-1.2.50 but running with 1.6.16, so use system png library
#TARGET_LINK_LIBRARIES(Liblept_Test ${liblept} ${libtiff} ${libgif} ${libpng} ${libgif} ${libjpeg} ${libz})
TARGET_LINK_LIBRARIES(Liblept_Test ${liblept} ${libtiff} ${libgif} png jpeg ${libz})


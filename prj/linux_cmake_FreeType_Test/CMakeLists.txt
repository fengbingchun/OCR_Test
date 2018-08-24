PROJECT(FreeType_Test)
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

# support C++11
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# support C++14, when gcc version > 5.1, use -std=c++14 instead of c++1y
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -O2")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -O2")

MESSAGE(STATUS "project source dir: ${PROJECT_SOURCE_DIR}")
SET(PATH_TEST_FILES ${PROJECT_SOURCE_DIR}/./../../demo/FreeType_Test)
SET(PATH_SRC_CVXTEXT_FILES ${PROJECT_SOURCE_DIR}/./../../src/CvxText)
SET(PATH_SRC_FREETYPE_FILES ${PROJECT_SOURCE_DIR}/./../../src/freetype-2.5.5)
MESSAGE(STATUS "path src files: ${PATH_SRC_FREETYPE_FILES}")

# head file search path
INCLUDE_DIRECTORIES(
	${PATH_TEST_FILES}
	${PATH_SRC_CVXTEXT_FILES}
	${PATH_SRC_FREETYPE_FILES}/include
	/opt/opencv3.4.2/include
)

# find opencv 3.4.2 dynamic library
FIND_LIBRARY(opencv_core NAMES opencv_core PATHS /opt/opencv3.4.2/lib NO_DEFAULT_PATH)
FIND_LIBRARY(opencv_imgproc NAMES opencv_imgproc PATHS /opt/opencv3.4.2/lib NO_DEFAULT_PATH)
FIND_LIBRARY(opencv_highgui NAMES opencv_highgui PATHS /opt/opencv3.4.2/lib NO_DEFAULT_PATH)
FIND_LIBRARY(opencv_imgcodecs NAMES opencv_imgcodecs PATHS /opt/opencv3.4.2/lib NO_DEFAULT_PATH)
FIND_LIBRARY(opencv_video NAMES opencv_video PATHS /opt/opencv3.4.2/lib NO_DEFAULT_PATH)
FIND_LIBRARY(opencv_videoio NAMES opencv_videoio PATHS /opt/opencv3.4.2/lib NO_DEFAULT_PATH)
MESSAGE(STATUS "opencv libraries: ${opencv_core} ${opencv_imgproc} ${opencv_highgui} ${opencv_imgcodecs} ${opencv_video}" ${opencv_videoio})

# recursive query match files :*.cpp, *c
FILE(GLOB_RECURSE TEST_CPP_LIST ${PATH_TEST_FILES}/*.cpp)
FILE(GLOB_RECURSE TEST_C_LIST ${PATH_TEST_FILES}/*.c)
#MESSAGE(STATUS "cpp list: ${TEST_CPP_LIST} ${TEST_C_LIST}")

FILE(GLOB_RECURSE SRC_CVXTEXT_CPP_LIST ${PATH_SRC_CVXTEXT_FILES}/*.cpp)
#MESSAGE(STATUS "src cpp list: ${SRC_CVXTEXT_CPP_LIST}")

# generate static library for freetype
ADD_SUBDIRECTORY(${PATH_SRC_FREETYPE_FILES} build)

# build executable program
ADD_EXECUTABLE(FreeType_Test ${TEST_CPP_LIST} ${TEST_C_LIST} ${SRC_CVXTEXT_CPP_LIST})
# add dependent library: static and dynamic
TARGET_LINK_LIBRARIES(FreeType_Test freetype ${opencv_core} ${opencv_imgproc} ${opencv_highgui} ${opencv_imgcodecs} ${opencv_video} ${opencv_videoio} pthread)


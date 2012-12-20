cmake_minimum_required(VERSION 2.6)
project (Ballspeed)

include_directories ("${PROJECT_BINARY_DIR}")

SET(SOURCES
    "main.c"
    "hough.c"
    "window.c"
    "stb_image.c"
)

option (USE_OPENCV2 "Use OpenCV-implementation" ON)

find_library(GL REQUIRED)


if (USE_OPENCV2)
    add_definitions(-DOPENCV_BACKEND)
    SET(executable "BallspeedOCV")
else (USE_OPENCV2)
    add_executable(Ballspeed ${SOURCES})
    SET(executable "Ballspeed")
endif (USE_OPENCV2)

add_executable(${executable} ${SOURCES})
target_link_libraries (${executable} GL glut m)

cmake_minimum_required(VERSION 2.6)
project (Ballspeed)

#include_directories ("${PROJECT_BINARY_DIR}")

SET(SOURCES
    "main.c"
    "hough.c"
    "window.c"
    "edge.c"
    "stb_image.c"
)

SET(LIBS
    GL
    glut
    m
)

option (USE_OPENCV2 "Use OpenCV-implementation" ON)

if (USE_OPENCV2)
    add_definitions(-DOPENCV_BACKEND)
    find_package (OpenCV REQUIRED)

    SET(LIBS
        ${LIBS}
        ${OpenCV_LIBS}
    )

    SET(executable "BallspeedOCV")

else (USE_OPENCV2)
    SET(executable "Ballspeed")
endif (USE_OPENCV2)

message(STATUS ${SOURCES} ${LIBS})
add_executable(${executable} ${SOURCES})
target_link_libraries (${executable} ${LIBS})

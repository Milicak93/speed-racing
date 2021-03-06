cmake_minimum_required(VERSION 3.8)
project(SpeedRacing)

set(CMAKE_CXX_STANDARD 11 -framework OpenGL -framework GLUT)

find_package (OpenGL REQUIRED)
find_package (GLUT REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})

configure_file(car.model car.model COPYONLY)
configure_file(car.bmp car.bmp COPYONLY)
configure_file(wheels.bmp wheels.bmp COPYONLY)
configure_file(asphalt.bmp asphalt.bmp COPYONLY)
configure_file(grass.bmp grass.bmp COPYONLY)
configure_file(start.bmp start.bmp COPYONLY)

set(SOURCE_FILES main.c model.c model.h texture.c texture.h)
add_executable(SpeedRacing ${SOURCE_FILES})
target_link_libraries (SpeedRacing ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
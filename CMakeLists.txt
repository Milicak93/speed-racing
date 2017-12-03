cmake_minimum_required(VERSION 3.8)
project(SpeedRacing)

set(CMAKE_CXX_STANDARD 11 -framework OpenGL -framework GLUT)

find_package (OpenGL REQUIRED)
find_package (GLUT REQUIRED)

configure_file(car.obj car.obj COPYONLY)
configure_file(car.bmp car.bmp COPYONLY)

set(SOURCE_FILES main.c model.c model.h texture.c texture.h)
add_executable(SpeedRacing ${SOURCE_FILES})
target_link_libraries (SpeedRacing ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
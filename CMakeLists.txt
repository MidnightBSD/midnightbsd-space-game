cmake_minimum_required(VERSION 3.10)
project(midnightbsd_space_game)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    Allenroids.cpp
    Allenroids.h
    Asteroid.cpp
    Asteroid.h
    BadGuy.cpp
    BadGuy.h
    ConfigureScreen.cpp
    ConfigureScreen.h
    Crystal.cpp
    Crystal.h
    GameObject.cpp
    GameObject.h
    Iterator.h
    Kerblooie.cpp
    Kerblooie.h
    LinkedList.h
    Node.h
    Player.cpp
    Player.h
    Shot.cpp
    Shot.h
    sound.cpp
    sound.h
    Spaceship.cpp
    Spaceship.h
    StandardElements.cpp
    StandardElements.h
    Starfield.cpp
    Starfield.h
    TitleScreen.cpp
    TitleScreen.h)

find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(GLUT REQUIRED)
find_package(SDL REQUIRED)

include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} ${SDL_INCLUDE_DIRS} )

add_executable(midnightbsd_space_game ${SOURCE_FILES})

target_link_libraries(midnightbsd_space_game SDL ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${SDL_LIBRARIES} )
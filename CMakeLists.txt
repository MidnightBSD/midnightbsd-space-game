cmake_minimum_required(VERSION 3.10)
project(midnightbsd_space_game)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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
    Platform.cpp
    Platform.h
    Renderer.cpp
    Renderer.h
    UiText.cpp
    UiText.h
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
find_package(SDL2 REQUIRED CONFIG)
find_package(SDL2_ttf REQUIRED CONFIG)

include_directories(${OPENGL_INCLUDE_DIRS})

add_executable(midnightbsd_space_game ${SOURCE_FILES})

target_link_libraries(midnightbsd_space_game OpenGL::OpenGL SDL2::SDL2 SDL2_ttf::SDL2_ttf)

include(CTest)

if(BUILD_TESTING)
    add_executable(linked_list_tests tests/LinkedListTests.cpp)
    target_include_directories(linked_list_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    add_test(NAME linked_list_tests COMMAND linked_list_tests)

    add_executable(game_random_tests tests/GameRandomTests.cpp)
    target_include_directories(game_random_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    add_test(NAME game_random_tests COMMAND game_random_tests)

    add_executable(game_object_tests tests/GameObjectTests.cpp GameObject.cpp Spaceship.cpp Renderer.cpp)
    target_include_directories(game_object_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(game_object_tests OpenGL::OpenGL SDL2::SDL2)
    add_test(NAME game_object_tests COMMAND game_object_tests)

    add_executable(standard_elements_tests tests/StandardElementsTests.cpp StandardElements.cpp)
    target_include_directories(standard_elements_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    add_test(NAME standard_elements_tests COMMAND standard_elements_tests)

    add_executable(ui_text_tests tests/UiTextTests.cpp)
    target_include_directories(ui_text_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    add_test(NAME ui_text_tests COMMAND ui_text_tests)

    add_executable(sound_tests tests/SoundTests.cpp sound.cpp)
    target_include_directories(sound_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(sound_tests SDL2::SDL2)
    add_test(NAME sound_tests COMMAND sound_tests)

    add_executable(platform_tests tests/PlatformTests.cpp Platform.cpp)
    target_include_directories(platform_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(platform_tests SDL2::SDL2)
    add_test(NAME platform_tests COMMAND platform_tests)

    add_executable(core_context_smoke EXCLUDE_FROM_ALL
        tests/CoreContextSmoke.cpp Platform.cpp Renderer.cpp Starfield.cpp UiText.cpp
        GameObject.cpp Asteroid.cpp Shot.cpp Kerblooie.cpp Spaceship.cpp Crystal.cpp)
    target_include_directories(core_context_smoke PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(core_context_smoke OpenGL::OpenGL SDL2::SDL2 SDL2_ttf::SDL2_ttf)
endif()

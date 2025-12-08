cmake_minimum_required(VERSION 3.29)
project(saiboVolleyball)

set(CMAKE_CXX_STANDARD 20)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-finput-charset=utf-8 -fexec-charset=utf-8)
    add_definitions(-DUNICODE -D_UNICODE)
endif()



add_executable(main
        main.cpp
        player.cpp
        game.cpp
        serve.cpp
        receiveServe.cpp
        setBall.cpp
        spike.cpp
        block.cpp
        defense.cpp
        supportCal.cpp
        mentalCalculation.cpp
        player.h
        game.h
        serve.h
        receiveServe.h
        setBall.h
        spike.h
        block.h
        defense.h
        supportCal.h
        mentalCalculation.h
        config.h)
cmake_minimum_required(VERSION 3.31.0)

# Options
set(DOTCMAKE_CONFIGURE_IDE ON)
set(DOTCMAKE_EMSDK_AUTOUPDATE ON)

project(SDLTemplate)

EXE(${PROJECT_NAME})

add_subdirectory(Source)

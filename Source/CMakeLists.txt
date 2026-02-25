include(C++Utils)

target_sources(${PROJECT_NAME}
PUBLIC
    FILE_SET module_files
    TYPE CXX_MODULES
    FILES
        App.ixx
        App.State.c++
        Logger.c++
        Events.c++      Events.impl.c++
        Variants.c++
        Windows.c++
            Windows/Window.c++
            Windows/Renderer.c++
PRIVATE
    main.cpp
)

target_link_libraries(${PROJECT_NAME}
    .cmake::Utils
    SDL3::SDL3
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    PROJECT_NAME="${DOTCMAKE_PROJECT_NAME}"
    PROJECT_VERSION="${DOTCMAKE_PROJECT_VERSION}"
    PROJECT_ID="${DOTCMAKE_PROJECT_ID}"
)

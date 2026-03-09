include(C++Utils)

target_sources(${PROJECT_NAME}
PUBLIC
    FILE_SET module_files
    TYPE CXX_MODULES
    FILES
        App/App.ixx
        App/App.State.cxx
        App/Events.cxx      App/Events.impl.cxx
        Utils/Logger.cxx
        Variants.cxx
        Windows.cxx
            Windows/Window.cxx
            Windows/Renderer.cxx
PRIVATE
    App/main.cxx
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

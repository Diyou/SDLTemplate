include(C++Utils)

target_sources(${PROJECT_NAME}
PUBLIC
    FILE_SET module_files
    TYPE CXX_MODULES
    FILES
        App/App.ixx
        App/App.Main.cxx
        App/Events.cxx
        Utils/Logger.cxx
        Variants.cxx
        Windows.cxx
            Windows/Window.cxx
            Windows/Renderer.cxx
            Windows/WGPUWindow.cxx
                Windows/webgpu/Backend.ixx
PRIVATE
        Windows/webgpu/Surface.cxx
        Windows/webgpu/Instance.cxx
)

target_link_libraries(${PROJECT_NAME}
    .cmake::Utils
    SDL3::SDL3
    $<IF:$<BOOL:${EMSCRIPTEN}>,
        emdawnwebgpu_cpp,
        dawn::webgpu_dawn
        dawn::dawncpp_headers
    >
)
target_link_options(${PROJECT_NAME} PRIVATE
    $<$<BOOL:${EMSCRIPTEN}>:
        -sASYNCIFY # or -sJSPI
    >
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    PROJECT_NAME="${DOTCMAKE_PROJECT_NAME}"
    PROJECT_VERSION="${DOTCMAKE_PROJECT_VERSION}"
    PROJECT_ID="${DOTCMAKE_PROJECT_ID}"
)

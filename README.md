[![Linux](https://github.com/Diyou/SDLTemplate/actions/workflows/linux.yml/badge.svg)](https://github.com/Diyou/SDLTemplate/actions/workflows/linux.yml)
[![macOS](https://github.com/Diyou/SDLTemplate/actions/workflows/macos.yml/badge.svg)](https://github.com/Diyou/SDLTemplate/actions/workflows/macos.yml)
[![Windows](https://github.com/Diyou/SDLTemplate/actions/workflows/windows.yml/badge.svg)](https://github.com/Diyou/SDLTemplate/actions/workflows/windows.yml)<br>
[![Android](https://github.com/Diyou/SDLTemplate/actions/workflows/android.yml/badge.svg)](https://github.com/Diyou/SDLTemplate/actions/workflows/android.yml)
[![Web](https://github.com/Diyou/SDLTemplate/actions/workflows/emscripten.yml/badge.svg)](https://github.com/Diyou/SDLTemplate/actions/workflows/emscripten.yml)

# SDLTemplate

A easy extendable template to start a c++ module based multiplatform project with SDL3.<br>
Toolchain abstractions via [dotcmake](https://github.com/Diyou/.cmake)

### Contents:

- [Platform agnostic main](Source/App/App.Main.cxx) (via SDL_MAIN_USE_CALLBACKS)
- [Global App-State instance](Source/App/App.ixx)
- [Event-Mapping](Source/App/Events.cxx) (via static dispatch)
- [Basic Window class with default Renderer](Source/Windows/Renderer.cxx)

### How-to:

 - Just edit [Renderer.cxx](Source/Windows/Renderer.cxx)
 - Or
    1. Create a new partiton in Source/Windows with a new struct     inheriting from [Window](Source/Windows/Window.cxx)
    2. Modify [Variants](Source/Variants.cxx) to include the new Window variant
    3. Add the partition in [Windows.cxx](Source/Windows.cxx)

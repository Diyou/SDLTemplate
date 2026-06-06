module;
#include <cstdlib>
#ifndef CMAKE_IMPORT_STD
#  include <expected>
#  include <format>
#  include <iostream>
#  include <unordered_map>
#  include <variant>
#endif
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>

module App:WGPUWindow;

#ifdef CMAKE_IMPORT_STD
import std;
#endif

import dotcmake;
import :Window;
import webgpu;

import Scene;

namespace Windows {

using namespace std;
using namespace dotcmake;

struct WGPUWindow final : Window
{
  using Window::Window;

  [[nodiscard]]
  SDL_AppResult
  Iterate() const override
  {
    scene.Draw();
    Backend.Iterate();
    return Window::Iterate();
  }

  void
  Destroy()
  {
    Backend.Destroy();
    Window::Destroy();
  }

private:
  wgpu::Backend Backend{handle};
  Scene         scene{Backend};
};

}

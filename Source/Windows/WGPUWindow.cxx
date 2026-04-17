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
import wgpu;

namespace Windows {

using namespace std;
using namespace dotcmake;

struct WGPUWindow final
: Window
, private wgpu::Backend
{
  enum class Errors : uint8_t
  {
    Unsupported_Surface
  };

private:
  expected< void, Errors >
  InitWGPU();

public:
  WGPUWindow(
    string const   &title,
    SDL_WindowFlags flags  = 0,
    int             width  = DEFAULT_WIDTH,
    int             height = DEFAULT_HEIGHT)
  : Window{title, flags, width, height}
  , wgpu::Backend(handle)
  {}

  [[nodiscard]]
  SDL_AppResult
  Iterate() const override
  {
    return Window::Iterate();
  }

  void
  Destroy() const
  {
    Window::Destroy();
  }
};

}

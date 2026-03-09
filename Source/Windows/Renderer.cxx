module;
#ifndef CMAKE_IMPORT_STD
#  include <format>
#  include <unordered_map>
#  include <variant>
#endif
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <cassert>
module App:Renderer;

#ifdef CMAKE_IMPORT_STD
import std;
#endif

import dotcmake;

import :Window;
import :App;
import Logger;

namespace Windows {

using namespace std;

struct Renderer final : Window
{
  static inline int DEFAULT_WIDTH  = 600;
  static inline int DEFAULT_HEIGHT = 800;

  SDL_Renderer     *renderer;

  Renderer(
    string const   &title,
    SDL_WindowFlags flags  = 0,
    int             width  = DEFAULT_WIDTH,
    int             height = DEFAULT_HEIGHT)
  : Window{title, flags, width, height}
  , renderer{SDL_CreateRenderer(handle, nullptr)}
  {
    assert(renderer != nullptr);
    SDL_SetRenderVSync(
      renderer, dotcmake::Platform::Web ? 1 : SDL_RENDERER_VSYNC_ADAPTIVE);
  }

  // Override Window Event
  void
  OnResized(SDL_WindowEvent &event) override
  {
    auto const &width  = event.data1;
    auto const &height = event.data2;
  }

  [[nodiscard]]
  SDL_AppResult
  Iterate() const override
  {
    SDL_SetRenderDrawColor(renderer, 0x00, 0xd0, 0xd0, 0xff);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    return Window::Iterate();
  }

  void
  Destroy() const
  {
    SDL_DestroyRenderer(renderer);
    Window::Destroy();
  }
};

}

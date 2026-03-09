module;
#ifndef CMAKE_IMPORT_STD
#  include <memory>
#  include <string>
#  include <unordered_map>
#endif
#include <SDL3/SDL_init.h>
#include <cassert>
module App:Window;

#ifdef CMAKE_IMPORT_STD
import std;
#endif

import :Events;

namespace Windows {

using namespace std;

struct Window : Events
{
  SDL_Window       *handle          = nullptr;
  bool              minimizeOnClose = false;

  static inline int DEFAULT_WIDTH   = 720;
  static inline int DEFAULT_HEIGHT  = 480;

  Window(
    string const   &title,
    SDL_WindowFlags flags  = 0,
    int             width  = DEFAULT_WIDTH,
    int             height = DEFAULT_HEIGHT)
  {
    handle = SDL_CreateWindow(title.c_str(), width, height, flags);
    assert(handle != nullptr);
  }

  [[nodiscard]]
  virtual SDL_AppResult
  Iterate() const
  {
    return SDL_APP_CONTINUE;
  }

  [[nodiscard]]
  SDL_WindowID
  GetID() const
  {
    return SDL_GetWindowID(handle);
  }

  void
  Maximize() const
  {
    SDL_MaximizeWindow(handle);
  }

  void
  Minimize() const
  {
    SDL_MinimizeWindow(handle);
  }

  void
  Hide() const
  {
    SDL_HideWindow(handle);
  }

  void
  Destroy() const
  {
    SDL_DestroyWindow(handle);
  }

  template< typename Variant, typename... Args >
    requires derived_from< Variant, Window > && is_final_v< Variant >
  static pair< Container::iterator, bool >
  Create(Args &&...args)
  {
    return Container::Emplace< Variant >(std::forward< Args >(args)...);
  }
};

}

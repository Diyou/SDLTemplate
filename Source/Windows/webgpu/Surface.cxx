module;
#include <webgpu/webgpu_cpp.h>
#ifndef CMAKE_IMPORT_STD
#  include <expected>
#endif
#include <SDL3/SDL_metal.h>
#include <SDL3/SDL_video.h>
module wgpu;

#ifdef CMAKE_IMPORT_STD
import std;
#endif

import dotcmake;

using namespace std;
using namespace dotcmake;

namespace wgpu {

using Errors = Backend::Errors;

expected< Surface, Errors >
Backend::GetSurface(SDL_Window *handle)
{
  Surface           surface;
  SurfaceDescriptor descriptor{};

  SDL_PropertiesID  props = SDL_GetWindowProperties(handle);

#ifndef __EMSCRIPTEN__
  if constexpr (Platform::Linux) {
    if (
      void *display_p = SDL_GetPointerProperty(
        props, SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, nullptr);
      display_p != nullptr)
    {
      // wayland
      void *surface_p = SDL_GetPointerProperty(
        props, SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, nullptr);
      if (surface_p == nullptr) [[unlikely]] {
        return unexpected(Errors::Unsupported_Surface);
      }

      SurfaceSourceWaylandSurface source{};
      source.display         = display_p;
      source.surface         = surface_p;

      descriptor.label       = "Surface(wayland)";
      descriptor.nextInChain = &source;
      surface                = instance.CreateSurface(&descriptor);
    }
    else if (
      void *display_p = SDL_GetPointerProperty(
        props, SDL_PROP_WINDOW_X11_DISPLAY_POINTER, nullptr);
      display_p != nullptr)
    {
      // x11
      uint64_t window_n =
        SDL_GetNumberProperty(props, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
      if (window_n == 0) [[unlikely]] {
        return unexpected(Errors::Unsupported_Surface);
      }
      SurfaceSourceXlibWindow source{};
      source.display         = display_p;
      source.window          = window_n;

      descriptor.label       = "Surface(x11)";
      descriptor.nextInChain = &source;
      surface                = instance.CreateSurface(&descriptor);
    }
    else {
      // Neither wayland nor x11
      return unexpected(Errors::Unsupported_Surface);
    }
  }
  else if constexpr (Platform::Windows) {
    void *hwnd_p = SDL_GetPointerProperty(
      props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);

    if (hwnd_p == nullptr) [[unlikely]] {
      return unexpected(Errors::Unsupported_Surface);
    }

    SurfaceSourceWindowsHWND source{};
    source.hwnd            = hwnd_p;

    descriptor.label       = "Surface(win32)";
    descriptor.nextInChain = &source;
    surface                = instance.CreateSurface(&descriptor);
  }
  else if constexpr (Platform::Android) {
    void *window_p = SDL_GetPointerProperty(
      props, SDL_PROP_WINDOW_ANDROID_WINDOW_POINTER, nullptr);

    if (window_p == nullptr) [[unlikely]] {
      return unexpected(Errors::Unsupported_Surface);
    }

    SurfaceSourceAndroidNativeWindow source{};
    source.window          = window_p;

    descriptor.label       = "Surface(android)";
    descriptor.nextInChain = &source;
    surface                = instance.CreateSurface(&descriptor);
  }
  else if constexpr (Platform::macOS || Platform::IOS) {
    void *window_p = SDL_GetPointerProperty(
      props, SDL_PROP_WINDOW_COCOA_METAL_VIEW_TAG_NUMBER, nullptr);

    if (window_p == nullptr) [[unlikely]] {
      return unexpected(Errors::Unsupported_Surface);
    }

    SurfaceSourceMetalLayer source{};
    SDL_MetalView           view = SDL_Metal_CreateView(handle);
    source.layer                 = SDL_Metal_GetLayer(view);

    descriptor.label             = "Surface(Metal)";
    descriptor.nextInChain       = &source;
    surface                      = instance.CreateSurface(&descriptor);
  }
  else {
    return unexpected(Errors::Unsupported_Surface);
  }
#else
  if constexpr (Platform::Web) {
    char const *canvas_id = SDL_GetStringProperty(
      props, SDL_PROP_WINDOW_EMSCRIPTEN_CANVAS_ID_STRING, nullptr);

    if (canvas_id == nullptr) [[unlikely]] {
      return unexpected(Errors::Unsupported_Surface);
    }

    EmscriptenSurfaceSourceCanvasHTMLSelector source{};
    source.selector        = canvas_id;

    descriptor.label       = "Surface(html5)";
    descriptor.nextInChain = &source;
    surface                = instance.CreateSurface(&descriptor);
  }
  else {
    return unexpected(Errors::Unsupported_Surface);
  }
#endif
  return surface;
}

}

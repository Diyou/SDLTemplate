module;
#ifndef CMAKE_IMPORT_STD
#  include <memory>
#  include <unordered_map>
#  include <variant>
#endif
#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>

module App;
import :Events.implementation;

using namespace std;

struct App::Main
{
  using Window = Windows::Default;

  alignas(App) static inline array< byte, sizeof(App) > app;
  static inline tuple< Windows::Container::iterator > runtime;

  static SDL_AppResult
  Init(void **appstate, int argc, char **argv)
  {
    auto &[window] = runtime;
    App::instance  = new (app.data()) App{argc, argv};

    // This is required to trigger the last SDL_EVENT_WINDOW_DESTROYED
    // and to not quit when window is configured to minimize on close
    SDL_SetHint(SDL_HINT_QUIT_ON_LAST_WINDOW_CLOSE, "0");

    if (!SDL_Init(App::INIT_FLAGS)) [[unlikely]] {
      SDL_LogError(
        SDL_LOG_CATEGORY_ERROR, "SDL_Init failed: %s", SDL_GetError());
      return SDL_AppResult::SDL_APP_FAILURE;
    }

    SDL_Rect bounds{0, 0, Window::DEFAULT_WIDTH, Window::DEFAULT_HEIGHT};

    if constexpr (dotcmake::Platform::MOBILE) {
      auto const display = SDL_GetPrimaryDisplay();
      if (display == 0 || !SDL_GetDisplayBounds(display, &bounds)) [[unlikely]]
      {
        SDL_LogError(
          SDL_LOG_CATEGORY_ERROR,
          "SDL_GetDisplayBounds failed: %s",
          SDL_GetError());
        return SDL_AppResult::SDL_APP_FAILURE;
      }
    }

    auto const &[handle, created] = Window::Create< Window >(
      App::Executable(),
      dotcmake::Platform::MOBILE ? SDL_WINDOW_FULLSCREEN
      : dotcmake::Platform::Web  ? SDL_WINDOW_BORDERLESS
                                : SDL_WINDOW_RESIZABLE | SDL_WINDOW_TRANSPARENT,
      bounds.w,
      bounds.h);

    if (!created) [[unlikely]] {
      SDL_LogError(
        SDL_LOG_CATEGORY_ERROR, "Window::Create() failed: %s", SDL_GetError());
      return SDL_AppResult::SDL_APP_FAILURE;
    }

    window    = handle;
    *appstate = &runtime;

    return SDL_AppResult::SDL_APP_CONTINUE;
  }

  using Pointer = decltype(runtime) *;

  static SDL_AppResult
  Event(void *appstate, SDL_Event *event)
  {
    auto &[window] = *Pointer(appstate);
    switch (event->type) {
      using TYPE = SDL_EventType;
      case TYPE::SDL_EVENT_QUIT:
        return SDL_AppResult::SDL_APP_SUCCESS;
      default:
        if (
          event->type >= TYPE::SDL_EVENT_WINDOW_FIRST
          && event->type <= TYPE::SDL_EVENT_WINDOW_LAST)
        {
          return visit(
            [&event](auto &window) { return window.Event(event->window); },
            *Windows::Container::Get(event->window.windowID));
        }
        break;
    }
    return SDL_AppResult::SDL_APP_CONTINUE;
  }

  static SDL_AppResult
  Iterate(void *appstate)
  {
    auto &[window] = *Pointer(appstate);
    return visit(
      [](auto &window) { return window.Iterate(); }, *window->second);
  }

  static void
  Quit(void *appstate, SDL_AppResult result)
  {
    auto &[window] = *Pointer(appstate);
    SDL_Quit();
    App::State().~App();
  }
};

extern "C" {
SDLMAIN_DECLSPEC SDL_AppResult SDLCALL
SDL_AppInit(void **appstate, int argc, char *argv[])
{
  return App::Main::Init(appstate, argc, argv);
}

SDLMAIN_DECLSPEC SDL_AppResult SDLCALL
SDL_AppEvent(void *appstate, SDL_Event *event)
{
  return App::Main::Event(appstate, event);
}

SDLMAIN_DECLSPEC SDL_AppResult SDLCALL
SDL_AppIterate(void *appstate)
{
  return App::Main::Iterate(appstate);
}

SDLMAIN_DECLSPEC void SDLCALL
SDL_AppQuit(void *appstate, SDL_AppResult result)
{
  App::Main::Quit(appstate, result);
}
}

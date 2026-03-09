module;
#ifndef CMAKE_IMPORT_STD
#  include <filesystem>
#  include <span>
#  include <unordered_set>
#  include <vector>
#endif
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
module App:App;

#ifdef CMAKE_IMPORT_STD
import std;
#endif

import dotcmake;
using namespace std;
using path                                 = filesystem::path;

constexpr string_view VIDEO_DRIVER_WAYLAND = "wayland";

class App
{
  path const                         arg0;
  vector< string_view > const        args;
  unordered_set< string_view > const availableVideoDrivers;

  constexpr static SDL_InitFlags     INIT_FLAGS =
    SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD;

public:
  constexpr static string_view NAME       = PROJECT_NAME;
  constexpr static string_view VERSION    = PROJECT_VERSION;
  constexpr static string_view IDENTIFIER = PROJECT_ID;

  static path const &
  Path()
  {
    return State().arg0;
  }

  static string
  Executable()
  {
    return Path().filename().string();
  }

  static string
  Directory()
  {
    return Path().parent_path().string();
  }

  static decltype(args) &
  Arguments()
  {
    return State().args;
  }

  static App const &
  State()
  {
    return *instance;
  }

private:
  App(span< char * > const &argv)
  : arg0{argv.front()}
  , args{ArgVector(argv.subspan(1))}
  , availableVideoDrivers{GetVideoDrivers()}
  {
    SDL_SetAppMetadata(NAME.data(), VERSION.data(), IDENTIFIER.data());

    if constexpr (dotcmake::Compiler::DEBUG) {
      SDL_SetLogPriorities(SDL_LOG_PRIORITY_VERBOSE);
    }

    // Define hints
    if constexpr (dotcmake::Platform::Linux) {
      if (availableVideoDrivers.contains(VIDEO_DRIVER_WAYLAND)) {
        SDL_SetHint(SDL_HINT_VIDEO_DRIVER, VIDEO_DRIVER_WAYLAND.data());
      }
    }
  }

  App(int argc, char **argv)
  : App{span< char * >{argv, argv + argc}}
  {}

  static decltype(args)
  ArgVector(span< char * > const &list)
  {
    remove_const_t< decltype(args) > views;
    views.reserve(list.size());

    for (auto const &arg : list) {
      views.emplace_back(arg);
    }
    return views;
  }

  static decltype(availableVideoDrivers)
  GetVideoDrivers()
  {
    size_t const count = SDL_GetNumVideoDrivers();
    remove_const_t< decltype(availableVideoDrivers) > drivers;
    drivers.reserve(count);

    for (int i = 0; i < count; i++) {
      drivers.emplace(SDL_GetVideoDriver(i));
    }
    return drivers;
  }

  static inline App *instance = nullptr;

public:
  // Give public access to the SDL_main callbacks
  struct Main;

  // Make singleton Immovable
  App()            = delete;
  App(App const &) = delete;
  App(App &&)      = delete;
  App &
  operator=(App const &) = delete;
  App &
  operator=(App &&) = delete;
};

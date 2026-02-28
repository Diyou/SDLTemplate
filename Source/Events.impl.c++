module;
#ifndef CMAKE_IMPORT_STD
#  include <unordered_map>
#  include <variant>
#endif
#include <SDL3/SDL_video.h>
module App:Events.implementation;

import :Events;
import :Windows;

using namespace std;

namespace Windows {

// These need to know the sizes of the variants and couldn't be inlined
decltype(Container::instances) Container::instances;

decltype(Container::Get(0))
Container::Get(SDL_WindowID windowID)
{
  return instances.at(windowID);
}

decltype(Container::Remove(0))
Container::Remove(SDL_WindowID windowID)
{
  auto const item = instances.find(windowID);
  if (item != instances.end()) [[likely]] {
    return instances.erase(item);
  }
  return End();
}

}

module;
#ifndef CMAKE_IMPORT_STD
#  include <format>
#  include <source_location>
#endif
#include <SDL3/SDL_log.h>
export module Logger;

#ifdef CMAKE_IMPORT_STD
import std;
#endif

import dotcmake;

using namespace std;

using location = source_location;

export {
template< auto F >
void inline Log(string const &text, location current = location::current())
{
  using VoidFunction = void (*)();
  auto const print   = format(
    "[{}:{}][{}({})] {}\n",
    current.line(),
    current.column(),
    VoidFunction(F),
    dotcmake::GetFunctionName< F >(),
    text);
  SDL_Log("%s", print);
}

void inline Log(string const &text, location current = location::current())
{
  auto const print = format(
    "[{}:{}][{}] {}\n",
    current.line(),
    current.column(),
    current.function_name(),
    text);
  SDL_Log("%s", print.c_str());
}

template< auto F >
void inline Debug(string const &text, location current = location::current())
{
  if constexpr (dotcmake::Compiler::DEBUG) {
    Log< F >(text, current);
  }
}

void inline Debug(string const &text, location current = location::current())
{
  if constexpr (dotcmake::Compiler::DEBUG) {
    Log(text, current);
  }
}
}

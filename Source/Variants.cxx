module;
#ifndef CMAKE_IMPORT_STD
#  include <variant>
#endif
export module App:Variants;
// Make App available in downstream partitions
export import App;

#ifdef CMAKE_IMPORT_STD
import std;
#endif

namespace Windows {

// Supported Windows:
using Variants = std::variant< struct WGPUWindow >;

}

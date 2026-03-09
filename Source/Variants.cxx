module;
#ifndef CMAKE_IMPORT_STD
#  include <variant>
#endif
module App:Variants;

#ifdef CMAKE_IMPORT_STD
import std;
#endif

namespace Windows {

using Variants = std::variant< struct Window, struct Renderer >;

}

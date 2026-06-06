module;
#ifndef CMAKE_IMPORT_STD
#  include <array>
#endif
module webgpu;

import wgpu;
import dotcmake;

using namespace dotcmake;

namespace wgpu {

Instance Backend::instance = []() {
  InstanceDescriptor descriptor{};
  InstanceLimits     limits{};

  array              Features{InstanceFeatureName::TimedWaitAny};
  descriptor.requiredFeatureCount = Features.size();
  descriptor.requiredFeatures     = Features.data();

  descriptor.requiredLimits       = &limits;
  return CreateInstance(&descriptor);
}();

}

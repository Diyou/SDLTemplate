module;
#include <webgpu/webgpu_cpp.h>
#ifndef CMAKE_IMPORT_STD
#  include <expected>
#endif
#include <SDL3/SDL_video.h>
export module wgpu;

#ifdef CMAKE_IMPORT_STD
import std;
#endif

import dotcmake;

using namespace std;
using namespace dotcmake;

export namespace wgpu {

constexpr array Features{
  FeatureName::TextureCompressionBC,
  FeatureName::TextureCompressionETC2,
  FeatureName::TextureCompressionASTC,
  FeatureName::TimestampQuery,
  FeatureName::ShaderF16,
  FeatureName::DepthClipControl};

struct Backend
{
  Surface surface = nullptr;
  Adapter adapter = nullptr;
  Device  device  = nullptr;

  enum class Errors : uint8_t
  {
    Unsupported_Surface
  };

  static expected< Surface, Errors >
  GetSurface(SDL_Window *handle);

  Backend(SDL_Window *handle)
  {
    auto _surface = GetSurface(handle);
    if (!_surface) [[unlikely]] {
      return;
    }
    surface = std::move(_surface).value();

    // Request Adapter
    RequestAdapterOptions options{
      .powerPreference   = PowerPreference::HighPerformance,
      .compatibleSurface = surface,
    };

    instance.WaitAny(
      instance.RequestAdapter(
        &options,
        CallbackMode::WaitAnyOnly,
        [](
          RequestAdapterStatus status,
          Adapter              adapter,
          StringView           message,
          Backend             *self) {
          switch (status) {
            case RequestAdapterStatus::Success:
              self->adapter = std::move(adapter);
              break;
            case RequestAdapterStatus::CallbackCancelled:
            case RequestAdapterStatus::Unavailable:
            case RequestAdapterStatus::Error:
              cerr << message.data << "\n";
              break;
          }
        },
        this),
      UINT64_MAX);

    if (adapter == nullptr) [[unlikely]] {
      cerr << "Requesting webgpu Adapter failed\n";
      return;
    }

    // Request Device
    DeviceDescriptor descriptor{};
    descriptor.label = "Device(wgpu)";

    auto filter      = [this](auto const &feature) {
      return adapter.HasFeature(feature);
    };
    auto availableFeatures =
      Features | views::filter(filter) | ranges::to< vector >();

    descriptor.requiredFeatures     = availableFeatures.data();
    descriptor.requiredFeatureCount = availableFeatures.size();
    descriptor.SetDeviceLostCallback(
      CallbackMode::AllowSpontaneous,
      [](
        Device const    &device,
        DeviceLostReason reason,
        StringView       message,
        Backend         *self) {
        switch (reason) {
          case DeviceLostReason::Destroyed:
            break;
          case DeviceLostReason::Unknown:
          case DeviceLostReason::CallbackCancelled:
          case DeviceLostReason::FailedCreation:
            cerr << message.data << "\n";
            break;
        }
      },
      this);
    descriptor.SetUncapturedErrorCallback(
      [](
        Device const &device,
        ErrorType     error,
        StringView    message,
        Backend      *self) {
        switch (error) {
          case ErrorType::NoError:
          case ErrorType::Validation:
          case ErrorType::OutOfMemory:
          case ErrorType::Internal:
          case ErrorType::Unknown:
            cerr << message.data << "\n";
            break;
        }
      },
      this);

    instance.WaitAny(
      adapter.RequestDevice(
        &descriptor,
        CallbackMode::WaitAnyOnly,
        [](
          RequestDeviceStatus status,
          Device              device,
          StringView          message,
          Backend            *self) {
          switch (status) {
            case RequestDeviceStatus::Success:
              self->device = std::move(device);
              break;
            case RequestDeviceStatus::CallbackCancelled:
            case RequestDeviceStatus::Error:
              cerr << message.data << "\n";
              break;
          }
        },
        this),
      UINT64_MAX);
  }

private:
  static Instance instance;
};

}

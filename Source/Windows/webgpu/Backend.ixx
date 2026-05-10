module;
#include <webgpu/webgpu_cpp.h>
#ifndef CMAKE_IMPORT_STD
#  include <expected>
#  include <iostream>
#  include <ranges>
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

struct Backend;

struct IScene
{
  Backend const &Backend;

  IScene(struct Backend const &backend)
  : Backend{backend}
  {}

  virtual void
  Draw() const = 0;
};

constexpr array Features{
  FeatureName::TextureCompressionBC,
  FeatureName::TextureCompressionETC2,
  FeatureName::TextureCompressionASTC,
  FeatureName::TimestampQuery,
  FeatureName::ShaderF16,
  FeatureName::DepthClipControl};

struct Backend
{
  Surface       surface = nullptr;
  Adapter       adapter = nullptr;
  Device        device  = nullptr;

  TextureFormat textureFormat;

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

    // Configure surface
    SurfaceCapabilities caps;
    surface.GetCapabilities(adapter, &caps);

    vector< TextureFormat > TextureFormats(
      caps.formats, caps.formats + caps.formatCount);

    textureFormat = TextureFormats.front();

    vector< CompositeAlphaMode > AlphaModes(
      caps.alphaModes, caps.alphaModes + caps.alphaModeCount);

    vector< PresentMode > PresentModes(
      caps.presentModes, caps.presentModes + caps.presentModeCount);

    uint32_t width;
    uint32_t height;
    SDL_GetWindowSizeInPixels(handle, (int *)&width, (int *)&height);

    SurfaceConfiguration surfaceConfig{
      .device      = device,
      .format      = textureFormat,
      .usage       = TextureUsage::RenderAttachment,
      .width       = width,
      .height      = height,
      .alphaMode   = AlphaModes.front(),
      .presentMode = PresentModes.front()};

    surface.Configure(&surfaceConfig);
  }

  void
  Iterate() const
  {
    surface.Present();
    instance.ProcessEvents();
  }

  void
  Destroy()
  {
    surface = nullptr;
  }

  template< ranges::range R >
  Buffer
  createBuffer(R const &container, BufferUsage usage, uint64_t offset = 0) const
  {
    using T               = ranges::range_value_t< R >;
    size_t const     size = ranges::size(container) * sizeof(T);

    BufferDescriptor descriptor{
      .usage = BufferUsage::CopyDst | usage, .size = size};

    Buffer buffer = device.CreateBuffer(&descriptor);
    device.GetQueue().WriteBuffer(buffer, offset, container.data(), size);
    return buffer;
  }

  template< typename T >
    requires(!ranges::range< T >)
  Buffer
  createBuffer(T const &data, BufferUsage usage, uint64_t offset = 0) const
  {
    uint64_t const   size = sizeof(T);
    BufferDescriptor descriptor{
      .label = "WGSL Triangle Shader",
      .usage = BufferUsage::CopyDst | usage,
      .size  = size};

    Buffer buffer = device.CreateBuffer(&descriptor);
    device.GetQueue().WriteBuffer(buffer, offset, &data, size);
    return buffer;
  }

  ShaderModule
  createWGSLShaderModule(char const *data, char const *label = "WGSL Shader")
    const
  {
    ShaderSourceWGSL source;
    source.code = data;

    ShaderModuleDescriptor descriptor{.nextInChain = &source, .label = label};

    return device.CreateShaderModule(&descriptor);
  }

private:
  static Instance instance;
};

}

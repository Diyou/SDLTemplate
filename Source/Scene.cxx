module;
#ifndef CMAKE_IMPORT_STD
#  include <vector>
#endif
export module Scene;

#ifdef CMAKE_IMPORT_STD
import std;
#endif

import Backend.webgpu;
import wgpu;

using namespace std;
using namespace wgpu;

export struct Scene final : IScene
{
  using IScene::IScene;
  /* clang-format off */
  vector<float> const vertices {
    -0.8F, -0.8F, 0.0F, 0.0F, 1.0F, // BL
    +0.8F, -0.8F, 0.0F, 1.0F, 0.0F, // BR
    -0.0F, +0.8F, 1.0F, 0.0F, 0.0F, // top
  };
  size_t VertexStride = 5 * sizeof(float);
  
  vector<uint16_t> const indices {
    0,  1,  2, //
    0          //
  };

  mutable float rotation = 0;

  string Shader = R"(
struct VertexIn {
    @location(0) aPos : vec2<f32>,
    @location(1) aCol : vec3<f32>
}
struct VertexOut {
    @location(0) vCol : vec3<f32>,
    @builtin(position) Position : vec4<f32>
}
struct Rotation {
    @location(0) degs : f32
}

@group(0) @binding(0) var<uniform> uRot : Rotation;
@vertex
fn main(input : VertexIn) -> VertexOut {
    var rads : f32 = radians(uRot.degs);
    var cosA : f32 = cos(rads);
    var sinA : f32 = sin(rads);
    var rot : mat3x3<f32> = mat3x3<f32>(
        vec3<f32>( cosA, sinA, 0.0),
        vec3<f32>(-sinA, cosA, 0.0),
        vec3<f32>( 0.0,  0.0,  1.0));
    var output : VertexOut;
    output.Position = vec4<f32>(rot * vec3<f32>(input.aPos, 1.0), 1.0);
    output.vCol = input.aCol;
    return output;
}

@fragment
fn frag_main(@location(0) vCol : vec3<f32>) -> @location(0) vec4<f32> {
    return vec4<f32>(vCol, 1.0);
}  
  )";
  /* clang-format on */

  Buffer vertexBuffer{Backend.createBuffer(vertices, BufferUsage::Vertex)};
  Buffer indexBuffer{Backend.createBuffer(indices, BufferUsage::Index)};
  Buffer rotationBuffer{Backend.createBuffer(rotation, BufferUsage::Uniform)};

  BindGroup      bindGroup;
  RenderPipeline pipeline = [this]() -> RenderPipeline {
    ShaderModule    shader = Backend.createWGSLShaderModule(Shader.c_str());

    BindGroupLayout bindGroupLayout;
    {
      array                     entries{BindGroupLayoutEntry{
        .binding    = 0,
        .visibility = ShaderStage::Vertex,
        .buffer     = BufferBindingLayout{.type = BufferBindingType::Uniform}}};

      BindGroupLayoutDescriptor descriptor{
        .entryCount = entries.size(), .entries = entries.data()};

      bindGroupLayout = Backend.device.CreateBindGroupLayout(&descriptor);

      {
        array               entries{BindGroupEntry{
          .binding = 0,
          .buffer  = rotationBuffer,
          .offset  = 0,
          .size    = sizeof(rotation)}};
        BindGroupDescriptor descriptor{
          .layout     = bindGroupLayout,
          .entryCount = entries.size(),
          .entries    = entries.data()};

        bindGroup = Backend.device.CreateBindGroup(&descriptor);
      }
    }

    PipelineLayout pipelineLayout;
    {
      PipelineLayoutDescriptor descriptor{
        .bindGroupLayoutCount = 1, .bindGroupLayouts = &bindGroupLayout};
      pipelineLayout = Backend.device.CreatePipelineLayout(&descriptor);
    }

    array vertexAttributes{
      VertexAttribute{
        .format = VertexFormat::Float32x2, .offset = 0, .shaderLocation = 0},
      VertexAttribute{
        .format         = VertexFormat::Float32x3,
        .offset         = 2 * sizeof(float),
        .shaderLocation = 1}};

    array       vertexBufferLayouts{VertexBufferLayout{
      .arrayStride    = VertexStride,
      .attributeCount = vertexAttributes.size(),
      .attributes     = vertexAttributes.data()}};

    VertexState vertexState{
      .module      = shader,
      .entryPoint  = "main",
      .bufferCount = vertexBufferLayouts.size(),
      .buffers     = vertexBufferLayouts.data()};

    BlendState blendState{
      .color =
        {.operation = BlendOperation::Add,
         .srcFactor = BlendFactor::One,
         .dstFactor = BlendFactor::One},
      .alpha = {
        .operation = BlendOperation::Add,
        .srcFactor = BlendFactor::One,
        .dstFactor = BlendFactor::One}};

    array         colorTargets{ColorTargetState{
      .format    = Backend.textureFormat,
      .blend     = &blendState,
      .writeMask = ColorWriteMask::All}};

    FragmentState fragmentState{
      .module      = shader,
      .entryPoint  = "frag_main",
      .targetCount = colorTargets.size(),
      .targets     = colorTargets.data()};

    RenderPipelineDescriptor descriptor{
      .label  = "Pipeline",
      .layout = pipelineLayout,
      .vertex = vertexState,
      .primitive =
        PrimitiveState{
          .topology         = PrimitiveTopology::TriangleList,
          .stripIndexFormat = IndexFormat::Undefined,
          .frontFace        = FrontFace::CCW,
          .cullMode         = CullMode::None},
      .depthStencil = nullptr,
      .multisample =
        MultisampleState{
          .count = 1, .mask = 0xFFFFFFFF, .alphaToCoverageEnabled = false},
      .fragment = &fragmentState};
    return Backend.device.CreateRenderPipeline(&descriptor);
  }();

  void
  Draw() const override
  {
    SurfaceTexture backBuffer;
    Backend.surface.GetCurrentTexture(&backBuffer);

    array                colorAttachments{RenderPassColorAttachment{
      .view       = backBuffer.texture.CreateView(),
      .loadOp     = LoadOp::Clear,
      .storeOp    = StoreOp::Store,
      .clearValue = {.r = 0.3F, .g = 0.3F, .b = 0.3F, .a = 1.0F}}};

    RenderPassDescriptor renderPass{
      .colorAttachmentCount = colorAttachments.size(),
      .colorAttachments     = colorAttachments.data()};

    CommandEncoder encoder = Backend.device.CreateCommandEncoder();
    {
      // update scene
      rotation += 0.1F;
      Backend.device.GetQueue().WriteBuffer(
        rotationBuffer, 0, &rotation, sizeof(rotation));

      // Pass
      RenderPassEncoder pass = encoder.BeginRenderPass(&renderPass);
      pass.SetPipeline(pipeline);
      pass.SetBindGroup(0, bindGroup);

      pass.SetVertexBuffer(0, vertexBuffer);
      pass.SetIndexBuffer(indexBuffer, IndexFormat::Uint16);

      pass.DrawIndexed(indices.size(), 1);
      pass.End();
    }

    array commands{CommandBuffer{encoder.Finish()}};
    Backend.device.GetQueue().Submit(commands.size(), commands.data());
  }
};

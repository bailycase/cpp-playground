#pragma once
#include "bx/timer.h"
#include "components.hpp"
#include "entity_manager.hpp"
#include <bgfx/bgfx.h>
#include <bx/math.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <memory>

const int creen_width = 1920;
const int creen_height = 1080;

struct PosColorVertex {
  float x;
  float y;
  float z;
  uint32_t abgr;
};

static PosColorVertex cubeVertices[] = {
    {-1.0f, 1.0f, 1.0f, 0xff000000},   {1.0f, 1.0f, 1.0f, 0xff0000ff},
    {-1.0f, -1.0f, 1.0f, 0xff00ff00},  {1.0f, -1.0f, 1.0f, 0xff00ffff},
    {-1.0f, 1.0f, -1.0f, 0xffff0000},  {1.0f, 1.0f, -1.0f, 0xffff00ff},
    {-1.0f, -1.0f, -1.0f, 0xffffff00}, {1.0f, -1.0f, -1.0f, 0xffffffff},
};

static const uint16_t cubeTriList[] = {
    0, 1, 2, 1, 3, 2, 4, 6, 5, 5, 6, 7, 0, 2, 4, 4, 2, 6,
    1, 5, 3, 5, 7, 3, 0, 4, 1, 4, 5, 1, 2, 3, 6, 6, 3, 7,
};

class RenderSystem {

public:
  RenderSystem(EntityManager *entity_manager)
      : m_entity_manager(entity_manager) {

    bgfx::VertexLayout pcvDecl;
    pcvDecl.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .end();
    m_vbh = bgfx::createVertexBuffer(
        bgfx::makeRef(cubeVertices, sizeof(cubeVertices)), pcvDecl);
    m_ibh = bgfx::createIndexBuffer(
        bgfx::makeRef(cubeTriList, sizeof(cubeTriList)));

    bgfx::ShaderHandle vsh = loadShader("vs_cubes.bin");
    bgfx::ShaderHandle fsh = loadShader("fs_cubes.bin");
    m_program = bgfx::createProgram(vsh, fsh, false);
  }

  ~RenderSystem() {
    bgfx::destroy(m_vbh);
    bgfx::destroy(m_ibh);
  }

  void registerRenderable() {}

  void update() {
    float time = (float)((bx::getHPCounter() - last_frame_time) /
                         double(bx::getHPFrequency()));

    auto v = m_entity_manager->INTERNAL_ECS
                 .view<const PositionComponent, RenderComponent>();

    const bx::Vec3 at = {0.0f, 0.0f, 0.0f};
    const bx::Vec3 eye = {0.0f, 0.0f, -30.0f};

    // Set view and projection matrix for view 0.
    {
      float view[16];
      bx::mtxLookAt(view, eye, at);

      float proj[16];
      bx::mtxProj(proj, 60.0f, float(creen_width) / float(creen_height), 0.1f,
                  100.0f, bgfx::getCaps()->homogeneousDepth);
      bgfx::setViewTransform(0, view, proj);

      // Set view 0 default viewport.
      bgfx::setViewRect(0, 0, 0, uint16_t(creen_width), uint16_t(creen_height));
    }

    // 80 bytes stride = 64 bytes for 4x4 matrix + 16 bytes for RGBA color.
    const uint16_t instanceStride = 80;
    // to total number of instances to draw
    uint32_t totalCubes = v.size_hint() * v.size_hint();

    uint32_t drawnCubes =
        bgfx::getAvailInstanceDataBuffer(totalCubes, instanceStride);

    // save how many we couldn't draw due to buffer room so we can display it
    uint32_t m_lastFrameMissing = totalCubes - drawnCubes;

    bgfx::InstanceDataBuffer idb;
    bgfx::allocInstanceDataBuffer(&idb, drawnCubes, instanceStride);

    uint8_t *data = idb.data;

    for (auto entity : v) {
      auto &pos = v.get<PositionComponent>(entity);
      float *mtx = (float *)data;
      bx::mtxTranslate(mtx, -pos.x, -pos.y, 0);
      bx::mtxRotateXY(mtx, time * 2.1f, time * 2.1f);
      mtx[12] = -15.0f + float(pos.x) * 3.0f;
      mtx[13] = -15.0f + float(pos.y) * 3.0f;
      mtx[14] = 0.0f;

      bgfx::setTransform(mtx);

      data += instanceStride;
    }

    bgfx::setVertexBuffer(0, m_vbh);
    bgfx::setIndexBuffer(m_ibh);

    bgfx::submit(0, m_program);
    // Set instance data buffer.
    bgfx::setInstanceDataBuffer(&idb);
  }

private:
  bgfx::VertexBufferHandle m_vbh;
  bgfx::IndexBufferHandle m_ibh;
  bgfx::ProgramHandle m_program;
  EntityManager *m_entity_manager;
  std::unordered_map<Entity, Entity> known_entities;

  int64_t last_frame_time = bx::getHPCounter();

  bgfx::ShaderHandle loadShader(const char *FILENAME) {
    const char *shaderPath = "???";

    switch (bgfx::getRendererType()) {
    case bgfx::RendererType::Noop:
    case bgfx::RendererType::Direct3D9:
      shaderPath = "shaders/dx9/";
      break;
    case bgfx::RendererType::Direct3D11:
    case bgfx::RendererType::Direct3D12:
      shaderPath = "shaders/dx11/";
      break;
    case bgfx::RendererType::Gnm:
      shaderPath = "shaders/pssl/";
      break;
    case bgfx::RendererType::Metal:
      shaderPath = "shaders/metal/";
      break;
    case bgfx::RendererType::OpenGL:
      shaderPath = "shaders/glsl/";
      break;
    case bgfx::RendererType::OpenGLES:
      shaderPath = "shaders/essl/";
      break;
    case bgfx::RendererType::Vulkan:
      shaderPath = "shaders/spirv/";
      break;
    }

    size_t shaderLen = strlen(shaderPath);
    size_t fileLen = strlen(FILENAME);
    char *filePath = (char *)malloc(shaderLen + fileLen);
    memcpy(filePath, shaderPath, shaderLen);
    memcpy(&filePath[shaderLen], FILENAME, fileLen);

    FILE *file = fopen(filePath, "rb");
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    const bgfx::Memory *mem = bgfx::alloc(fileSize + 1);
    fread(mem->data, 1, fileSize, file);
    mem->data[mem->size - 1] = '\0';
    fclose(file);

    return bgfx::createShader(mem);
  }
};
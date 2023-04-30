// TODO: add multiplatform support

#include "bgfx/defines.h"
#include "bx/timer.h"
#include "imgui_impl_glfw.h"
#include "systems/render_system.hpp"
#include <cstddef>
#define GLFW_EXPOSE_NATIVE_COCOA

#include <cstdint>
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/bx.h>
#include <bx/math.h>
#include <imgui.h>
#include <imgui_impl_bgfx.h>

#include "components.hpp"
#include "entity_manager.hpp"
#include "global.hpp"
#include <random>

const int screen_width = 1920;
const int screen_height = 1080;

// Initialize the random number generator with a seed
std::mt19937 rng(std::random_device{}());

// Define a distribution for X positions (0-1919)
std::uniform_int_distribution<> xDist(-4.0f, 14.0f);

// Define a distribution for Y positions (0-1079)
std::uniform_int_distribution<> yDist(0.0f, 10.0f);

float randomFloatBetween0and500() {
  std::srand(
      std::time(nullptr)); // initialize random seed based on current time
  return (std::rand() / static_cast<float>(RAND_MAX)) * 2.f;
}

static void glfw_errorCallback(int error, const char *description) {
  fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

static void glfw_keyCallback(GLFWwindow *window, int key, int scancode,
                             int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwTerminate();
  }
}

int main() {
  glfwSetErrorCallback(glfw_errorCallback);
  GLFWwindow *window;

  if (!glfwInit()) {
    return -1;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  window = glfwCreateWindow(screen_width, screen_height, "playground", nullptr,
                            nullptr);

  glfwSetKeyCallback(window, glfw_keyCallback);

  if (!window) {
    glfwTerminate();
    return -1;
  }

  // bgfx::renderFrame();
  // bgfx::Init init;

  // bgfx::PlatformData pd;

  // pd.nwh = glfwGetCocoaWindow(window);

  // init.type = bgfx::RendererType::Count;
  // init.platformData.nwh = glfwGetCocoaWindow(window);
  // init.resolution.width = screen_width;
  // init.resolution.height = screen_height;
  // init.resolution.reset = BGFX_RESET_VSYNC;
  // init.platformData = pd;

  // if (!bgfx::init(init)) {
  //   return -1;
  // }

  // bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
  // 0x443355FF, 1.0f,
  //                    0);
  // bgfx::setViewRect(0, 0, 0, screen_width, screen_height);

  ImGui::CreateContext();
  // ImGui_Implbgfx_Init(255);

  ImGui_ImplGlfw_InitForOpenGL(window, true);

  // ** PLAYING AROUND **
  // EntityManager entity_manager;

  // for (auto i = 0; i < 10; i++) {
  //   const auto entity = entity_manager.create();

  //   entity_manager.add<PositionComponent>(entity, (float)xDist(rng),
  //                                         (float)yDist(rng));
  //   entity_manager.add<RenderComponent>(entity, 0.f);
  // }

  // RenderSystem rs = RenderSystem(&entity_manager);

  Entity selected_entity;

  int64_t lastTime = bx::getHPCounter();

  // Render Loop
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    bgfx::touch(0);

    ImGui_Implbgfx_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    int64_t currentTime = bx::getHPCounter();
    int64_t deltaTime = currentTime - lastTime;
    float fps = (float)bx::getHPFrequency() / (float)deltaTime;
    lastTime = currentTime;
    ImGui::Text("FPS: %.2f", fps);

    if (ImGui::Button("Click Me!")) {
      Entity new_entity = entity_manager.create();
      entity_manager.add<PositionComponent>(new_entity, (float)xDist(rng),
                                            (float)yDist(rng));
      entity_manager.add<RenderComponent>(new_entity, 0.f);
    }

    ImGui::BeginChild("Entities", ImVec2(0, 300), true);

    entity_manager.INTERNAL_ECS.each([&selected_entity](auto entity) {
      std::string entity_str = std::to_string(static_cast<uint32_t>(entity));
      if (ImGui::Selectable(entity_str.c_str(), entity == selected_entity,
                            ImGuiSelectableFlags_None,
                            ImVec2(0, ImGui::GetTextLineHeightWithSpacing()))) {
        selected_entity = entity;
      }
    });

    ImGui::EndChild();

    ImGui::BeginChild("Entity Controls", ImVec2(300, 300), true);

    ImGui::Button("I Exist!");
    PositionComponent &entity_position =
        entity_manager.INTERNAL_ECS.get<PositionComponent>(selected_entity);

    ImGui::SliderFloat("X", &entity_position.x, -100.0f, 100.f);
    ImGui::SliderFloat("Y", &entity_position.y, -100.0f, 100.f);

    ImGui::EndChild();

    ImGui::Render();
    ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());

    rs.update();

    bgfx::frame();
  }

  ImGui_Implbgfx_Shutdown();
  bgfx::shutdown();
  glfwTerminate();

  return 0;
}

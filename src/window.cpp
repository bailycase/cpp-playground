#include "window.hpp"
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include <iostream>
#include <stdio.h>

static void key_callback(GLFWwindow *window, int key, int scancode, int action,
                         int mods) {

  if (key < 0) {
    return;
  }

  switch (action) {
  case GLFW_PRESS:
    std::cout << key << std::endl;
    break;
  }
}

Window CreateWindow(void (*tickFunc)(void)) {
  Window w;

  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  w.window = glfwCreateWindow(1920, 1080, "Playground", nullptr, nullptr);

  // TODO: add logging
  if (!w.window) {
    std::cout << "Failed to create window" << std::endl;
    glfwTerminate();
  }

  glfwMakeContextCurrent(w.window);
  // gladLoadGL();

  glfwSetKeyCallback(w.window, key_callback);

  w.tickFunc = tickFunc;

  return w;
};

void WindowLoop(GLFWwindow *w) {
  while (!glfwWindowShouldClose(w)) {
    glfwPollEvents();
    glfwSwapBuffers(w);
  }
}
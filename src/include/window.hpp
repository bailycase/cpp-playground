#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

struct Window {
  GLFWwindow *window;
  void (*tickFunc)(void); // Function pointer to tick function

  Window() : window(nullptr), tickFunc(nullptr) {}
};

Window CreateWindow(void (*tickFunc)(void));
void WindowLoop(GLFWwindow *);
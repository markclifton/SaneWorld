#pragma once

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/ext.hpp>
#include <glm/glm.hpp>

extern float deltaX;
extern float deltaY;
extern float deltaZ;

static void error_callback(int error, const char* description) {
  fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action,
  int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);

  if (key == GLFW_KEY_R && (action == GLFW_PRESS || action == GLFW_REPEAT))
    deltaZ = -.01f;
  if (key == GLFW_KEY_R && action == GLFW_RELEASE)
    deltaZ = 0;

  if (key == GLFW_KEY_F && (action == GLFW_PRESS || action == GLFW_REPEAT))
    deltaZ = .01f;
  if (key == GLFW_KEY_F && action == GLFW_RELEASE)
    deltaZ = 0;

  if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT))
    deltaY = -.01f;
  if (key == GLFW_KEY_W && action == GLFW_RELEASE)
    deltaY = 0;

  if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT))
    deltaY = .01f;
  if (key == GLFW_KEY_S && action == GLFW_RELEASE)
    deltaY = 0;

  if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT))
    deltaX = -.01f;
  if (key == GLFW_KEY_A && action == GLFW_RELEASE)
    deltaX = 0;

  if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT))
    deltaX = .01f;
  if (key == GLFW_KEY_D && action == GLFW_RELEASE)
    deltaX = 0;
}

namespace OGL {
  class OGLCore {
  public:
    OGLCore(const char* Name, int Width, int Height);
    ~OGLCore();

    void update();
    inline bool isRunning() { return !glfwWindowShouldClose(window); }

    glm::mat4 getOrthoProjection();
    glm::mat4 getPersProjection();

    operator GLFWwindow* () { return window; }

  private:
    GLFWwindow* window;
    GLuint vao;
  };
} // namespace OGL
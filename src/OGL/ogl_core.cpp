#include "OGL/ogl_core.hpp"

namespace OGL {

OGLCore::OGLCore(const char *Name, int Width, int Height) {
  glfwSetErrorCallback(error_callback);

  if (!glfwInit())
    exit(EXIT_FAILURE);

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(Width, Height, Name, NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetKeyCallback(window, key_callback);

  glfwMakeContextCurrent(window);
  gladLoadGL(glfwGetProcAddress);
  glfwSwapInterval(1);

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
}

OGLCore::~OGLCore() {
  glDeleteVertexArrays(1, &vao);

  glfwDestroyWindow(window);
  glfwTerminate();

  exit(EXIT_SUCCESS);
}

void OGLCore::update() {
  glfwSwapBuffers(window);
  glfwPollEvents();

  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);

  glClear(GL_COLOR_BUFFER_BIT);
}

glm::mat4 OGLCore::getOrthoProjection() {
  float ratio;
  int width, height;

  glfwGetFramebufferSize(window, &width, &height);
  ratio = width / (float)height;

  return glm::perspective(glm::radians(45.f), 4.0f / 3.0f, 0.1f, 1000.0f);
}

} // namespace OGL
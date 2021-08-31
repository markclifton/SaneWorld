#pragma once

static const char *vs_basic = "#version 110\n"
                              "uniform mat4 MVP;\n"
                              "attribute vec3 vCol;\n"
                              "attribute vec2 vPos;\n"
                              "varying vec3 color;\n"
                              "void main()\n"
                              "{\n"
                              "    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
                              "    color = vCol;\n"
                              "}\n";

static const char *fs_basic = "#version 110\n"
                              "varying vec3 color;\n"
                              "void main()\n"
                              "{\n"
                              "    gl_FragColor = vec4(color, 1.0);\n"
                              "}\n";

static const char *vs_modern = "#version 330\n"
                               "layout(location = 0) in vec3 vPos;\n"
                               "layout(location = 1) in vec3 vCol;\n"
                               "out vec3 color;\n"
                               "uniform mat4 MVP;\n"
                               "void main() {\n"
                               "  gl_Position = MVP * vec4(vPos, 1.0);\n"
                               "  color = vCol;\n"
                               "}\n";

static const char *fs_modern = "#version 330\n"
                               "out vec4 outColor;\n"
                               "in vec3 color;\n"
                               "void main() {\n"
                               "  outColor = vec4(color, 1);\n"
                               "}\n";
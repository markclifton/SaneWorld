#pragma once

static const char* vs_basic = R""(
    "#version 110
    uniform mat4 MVP;
    attribute vec3 vCol;
    attribute vec2 vPos;
    varying vec3 color;
    void main()
    {
        gl_Position = MVP * vec4(vPos, 0.0, 1.0);
        color = vCol;
    }
)"";

static const char* fs_basic = R""(
    #version 110
    varying vec3 color
    void main()
    {
        gl_FragColor = vec4(color, 1.0)
    }
)"";

static const char* vs_modern = R""(
    #version 330
    layout(location = 0) in vec3 vPos;
    layout(location = 1) in vec3 vCol;
    out vec3 color;
    uniform mat4 MVP;
    void main() {
      gl_Position = MVP * vec4(vPos, 1.0);
      color = vCol;
    }
)"";

static const char* fs_modern = R""(
    #version 330
    out vec4 outColor;
    in vec3 color;
    void main() {
      outColor = vec4(color, 1);
    }
)"";
#pragma once

#include "OGL/ogl_buffer.hpp"

#include <cmath>

struct OGLVertex {
    float x, y, z, w;
    float r, g, b;
};

class OGLRectangle {
public:
    OGLRectangle(float x, float y, float z, float h, float w) {
        w = w / 2.f;
        h = h / 2.f;

        float color = std::abs(z / 4.f);

        _vertices[0] = { x - w, y + h, z, 1, color, 0, 0 };
        _vertices[1] = { x - w, y - h, z, 1, 0, color, 0 };
        _vertices[2] = { x + w, y - h, z, 1, 0, color, color };
        _vertices[3] = { x + w, y + h, z, 1, 0, 0, color };
    }

    int indicesCount() { return sizeof(_indices) / sizeof(int); }
    unsigned int* indices() { return &_indices[0]; }

    int verticesCount() { return sizeof(_vertices) / sizeof(OGLVertex); }
    OGLVertex* vertices() { return &_vertices[0]; }

private:
    OGLVertex _vertices[4];
    unsigned int _indices[6]{ 0, 1, 2, 2, 3, 0 };
};

namespace OGL {
    class VerticesManager {
    public:
        VerticesManager() : _buffer(GL_ARRAY_BUFFER) {}

        void add(int count, OGLVertex* data) {
            for (int i = 0; i < count; i++) {
                _data.push_back(data[i]);
            }
        }

        void bind() { _buffer.bind(); }

        void buffer(GLenum type) {
            _buffer.bind();
            _buffer.bufferData(_data.size() * sizeof(OGLVertex), &_data[0], type);
        }

        void reset() { _data.clear(); }

    private:
        OGL::Buffer _buffer;
        std::vector<OGLVertex> _data;
    };

    class IndicesManager {
    public:
        IndicesManager() : _buffer(GL_ELEMENT_ARRAY_BUFFER), _vertexCount(0) {}
        ~IndicesManager() = default;

        void add(int count, unsigned int* data) {
            unsigned int vertexCount = 0;
            for (int i = 0; i < count; i++) {
                vertexCount = vertexCount > (data[i] + 1) ? vertexCount : data[i] + 1;
                _data.push_back(data[i] + _vertexCount);
            }
            _vertexCount += vertexCount;
        }

        void buffer(GLenum type) {
            _buffer.bind();
            _buffer.bufferData(_data.size() * sizeof(int), &_data[0], type);
        }

        void draw() {
            _buffer.bind();
            glDrawElements(GL_TRIANGLES, (int)_data.size(), GL_UNSIGNED_INT, (void*)0);
        }

        void reset() {
            _vertexCount = 0;
            _data.clear();
        }

    private:
        OGL::Buffer _buffer;
        std::vector<unsigned int> _data;
        int _vertexCount;
    };
} // namespace OGL
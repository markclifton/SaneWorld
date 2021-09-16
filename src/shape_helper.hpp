#pragma once

#include <sane/graphics/buffer.hpp>

#include <cmath>

struct Vertex {
    float x, y, z, w;
    float r, g, b;
};

class Rectangle {
public:
    Rectangle(float x, float y, float z, float h, float w) {
        w = w / 2.f;
        h = h / 2.f;

        _vertices[0] = { x - w, y + h, z, 1, 1, 0, 0 };
        _vertices[1] = { x - w, y - h, z, 1, 0, 1, 0 };
        _vertices[2] = { x + w, y - h, z, 1, 0, 1, 1 };
        _vertices[3] = { x + w, y + h, z, 1, 0, 0, 1 };
    }

    int indicesCount() { return sizeof(_indices) / sizeof(int); }
    unsigned int* indices() { return &_indices[0]; }

    int verticesCount() { return sizeof(_vertices) / sizeof(Vertex); }
    Vertex* vertices() { return &_vertices[0]; }

private:
    Vertex _vertices[4];
    unsigned int _indices[6]{ 0, 1, 2, 2, 3, 0 };
};

class VerticesManager {
public:
    VerticesManager() : _buffer(GL_ARRAY_BUFFER) {}

    void add(int count, Vertex* data) {
        for (int i = 0; i < count; i++) {
            _data.push_back(data[i]);
        }
    }

    void bind() { _buffer.Bind(); }

    void buffer(GLenum type) {
        _buffer.Bind();
        _buffer.BufferData(_data.size() * sizeof(Vertex), &_data[0], type);
    }

    void reset() { _data.clear(); }

private:
    Sane::Buffer _buffer;
    std::vector<Vertex> _data;
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
        _buffer.Bind();
        _buffer.BufferData(_data.size() * sizeof(int), &_data[0], type);
    }

    void draw() {
        _buffer.Bind();
        glDrawElements(GL_TRIANGLES, (int)_data.size(), GL_UNSIGNED_INT, (void*)0);
    }

    void reset() {
        _vertexCount = 0;
        _data.clear();
    }

private:
    Sane::Buffer _buffer;
    std::vector<unsigned int> _data;
    int _vertexCount;
};
#include "Renderer.h"

#include "Platform.h"

#define GL_GLEXT_PROTOTYPES 1
#include <GL/glcorearb.h>

#include <cmath>
#include <vector>

namespace {
constexpr char vertexShaderSource[] = R"(#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
uniform mat4 projection;
out vec4 vertexColor;
void main() {
    gl_Position = projection * vec4(position, 1.0);
    vertexColor = color;
})";

constexpr char fragmentShaderSource[] = R"(#version 330 core
in vec4 vertexColor;
out vec4 fragmentColor;
void main() {
    fragmentColor = vertexColor;
})";

constexpr char texturedVertexShaderSource[] = R"(#version 330 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 uv;
uniform mat4 projection;
out vec2 textureCoordinate;
void main() { gl_Position = projection * vec4(position, 0.0, 1.0); textureCoordinate = uv; })";
constexpr char texturedFragmentShaderSource[] = R"(#version 330 core
in vec2 textureCoordinate;
uniform sampler2D glyphTexture;
uniform vec4 tint;
out vec4 fragmentColor;
void main() { fragmentColor = texture(glyphTexture, textureCoordinate) * tint; })";

bool compileShader(GLuint shader, const char *source, std::string &error) {
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    GLint compiled = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_TRUE) {
        return true;
    }
    GLint length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    error.resize(static_cast<size_t>(length));
    glGetShaderInfoLog(shader, length, nullptr, error.data());
    return false;
}

bool linkProgram(GLuint &program, const char *vertex, const char *fragment, std::string &error) {
    const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    if (!compileShader(vertexShader, vertex, error) || !compileShader(fragmentShader, fragment, error)) {
        glDeleteShader(vertexShader); glDeleteShader(fragmentShader); return false;
    }
    program = glCreateProgram();
    glAttachShader(program, vertexShader); glAttachShader(program, fragmentShader); glLinkProgram(program);
    glDeleteShader(vertexShader); glDeleteShader(fragmentShader);
    GLint linked = GL_FALSE; glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (linked == GL_TRUE) return true;
    GLint length = 0; glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    error.resize(static_cast<size_t>(length)); glGetProgramInfoLog(program, length, nullptr, error.data());
    glDeleteProgram(program); program = 0; return false;
}

}

Renderer *Renderer::current_ = nullptr;

bool Renderer::initialize(std::string &error) {
    const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    if (!compileShader(vertexShader, vertexShaderSource, error) ||
        !compileShader(fragmentShader, fragmentShaderSource, error)) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }
    glDeleteShader(vertexShader); glDeleteShader(fragmentShader);
    if (!linkProgram(program_, vertexShaderSource, fragmentShaderSource, error) ||
        !linkProgram(textureProgram_, texturedVertexShaderSource, texturedFragmentShaderSource, error)) { shutdown(); return false; }
    glGenVertexArrays(1, &vertexArray_);
    glGenBuffers(1, &vertexBuffer_);
    glBindVertexArray(vertexArray_);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(RenderVertex), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(RenderVertex),
                          reinterpret_cast<const void *>(offsetof(RenderVertex, color)));
    glGenVertexArrays(1, &textureVertexArray_);
    glGenBuffers(1, &textureVertexBuffer_);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    current_ = this;
    return true;
}

void Renderer::shutdown() {
    if (textureVertexBuffer_ != 0) glDeleteBuffers(1, &textureVertexBuffer_);
    if (textureVertexArray_ != 0) glDeleteVertexArrays(1, &textureVertexArray_);
    if (textureProgram_ != 0) glDeleteProgram(textureProgram_);
    if (vertexBuffer_ != 0) glDeleteBuffers(1, &vertexBuffer_);
    if (vertexArray_ != 0) glDeleteVertexArrays(1, &vertexArray_);
    if (program_ != 0) glDeleteProgram(program_);
    vertexBuffer_ = vertexArray_ = program_ = textureVertexBuffer_ = textureVertexArray_ = textureProgram_ = 0;
    if (current_ == this) {
        current_ = nullptr;
    }
}

Renderer *Renderer::current() {
    return current_;
}

void Renderer::beginFrame(const LogicalViewport &viewport, RenderColor clearColor) {
    glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
    glClearColor(clearColor.red, clearColor.green, clearColor.blue, clearColor.alpha);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program_);
    const float projection[] = {2.0F / 640, 0, 0, 0, 0, 2.0F / 480, 0, 0,
                                0, 0, -2.0F / 1280, 0, -1, -1, 0, 1};
    glUniformMatrix4fv(glGetUniformLocation(program_, "projection"), 1, GL_FALSE, projection);
}

void Renderer::triangles(std::span<const RenderVertex> vertices) {
    glBindVertexArray(vertexArray_);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size_bytes()), vertices.data(), GL_STREAM_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));
}

void Renderer::filledCircle(float x, float y, float radius, float rotationDegrees,
                            RenderColor color, unsigned int segments) {
    if (segments < 3) {
        return;
    }
    const float radians = rotationDegrees * 3.141592654F / 180.0F;
    const float sine = std::sin(radians);
    const float cosine = std::cos(radians);
    std::vector<RenderVertex> vertices;
    vertices.reserve(segments * 3);
    for (unsigned int index = 0; index < segments; ++index) {
        const auto point = [&](unsigned int pointIndex) {
            const float angle = 2.0F * 3.141592654F * pointIndex / segments;
            const float localX = radius * std::cos(angle);
            const float localY = radius * std::sin(angle);
            return RenderVertex{x + localX * cosine - localY * sine,
                                y + localX * sine + localY * cosine, 0.0F, color};
        };
        vertices.insert(vertices.end(), {{x, y, 0.0F, color}, point(index), point(index + 1)});
    }
    triangles(vertices);
}

void Renderer::ring(float x, float y, float radius, float thickness, RenderColor color,
                    unsigned int segments) {
    if (segments < 3 || thickness <= 0.0F) return;
    const float inner = radius - thickness / 2.0F;
    const float outer = radius + thickness / 2.0F;
    std::vector<RenderVertex> vertices;
    vertices.reserve(segments * 6);
    for (unsigned int index = 0; index < segments; ++index) {
        const auto point = [&](float r, unsigned int pointIndex) {
            const float angle = 2.0F * 3.141592654F * pointIndex / segments;
            return RenderVertex{x + r * std::cos(angle), y + r * std::sin(angle), 0.0F, color};
        };
        const auto a = point(inner, index); const auto b = point(outer, index);
        const auto c = point(outer, index + 1); const auto d = point(inner, index + 1);
        vertices.insert(vertices.end(), {a, b, c, a, c, d});
    }
    triangles(vertices);
}

void Renderer::filledRectangle(float x, float y, float width, float height, RenderColor color) {
    const RenderVertex vertices[] = {
        {x, y, 0.0F, color}, {x + width, y, 0.0F, color}, {x + width, y + height, 0.0F, color},
        {x, y, 0.0F, color}, {x + width, y + height, 0.0F, color}, {x, y + height, 0.0F, color},
    };
    triangles(vertices);
}

void Renderer::texturedQuad(unsigned int texture, float x, float y, float width, float height,
                            RenderColor color) {
    struct TexturedVertex { float x, y, u, v; };
    // SDL_ttf surfaces are top-down, while logical rendering coordinates increase upward.
    const TexturedVertex vertices[] = {{x, y, 0, 1}, {x + width, y, 1, 1}, {x + width, y + height, 1, 0},
                                       {x, y, 0, 1}, {x + width, y + height, 1, 0}, {x, y + height, 0, 0}};
    const float projection[] = {2.0F / 640, 0, 0, 0, 0, 2.0F / 480, 0, 0, 0, 0, -2.0F / 1280, 0, -1, -1, 0, 1};
    glUseProgram(textureProgram_);
    glUniformMatrix4fv(glGetUniformLocation(textureProgram_, "projection"), 1, GL_FALSE, projection);
    glUniform4f(glGetUniformLocation(textureProgram_, "tint"), color.red, color.green, color.blue, color.alpha);
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(textureProgram_, "glyphTexture"), 0);
    glBindVertexArray(textureVertexArray_); glBindBuffer(GL_ARRAY_BUFFER, textureVertexBuffer_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);
    glEnableVertexAttribArray(0); glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), nullptr);
    glEnableVertexAttribArray(1); glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), reinterpret_cast<const void *>(sizeof(float) * 2));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glUseProgram(program_);
}

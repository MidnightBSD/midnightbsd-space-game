#ifndef RENDERER_H
#define RENDERER_H

#include <span>
#include <string>

struct LogicalViewport;

struct RenderColor {
    float red{};
    float green{};
    float blue{};
    float alpha{1.0F};
};

struct RenderVertex {
    float x{};
    float y{};
    float z{};
    RenderColor color{};
};

class Renderer {
public:
    Renderer() = default;
    Renderer(const Renderer &) = delete;
    Renderer &operator=(const Renderer &) = delete;

    [[nodiscard]] bool initialize(std::string &error);
    void shutdown();
    void beginFrame(const LogicalViewport &viewport, RenderColor clearColor);
    void triangles(std::span<const RenderVertex> vertices);
    void filledCircle(float x, float y, float radius, float rotationDegrees,
                      RenderColor color, unsigned int segments);
    void ring(float x, float y, float radius, float thickness, RenderColor color,
              unsigned int segments);
    void filledRectangle(float x, float y, float width, float height, RenderColor color);
    void texturedQuad(unsigned int texture, float x, float y, float width, float height,
                      RenderColor color);

    [[nodiscard]] static Renderer *current();

private:
    unsigned int program_{};
    unsigned int vertexArray_{};
    unsigned int vertexBuffer_{};
    unsigned int textureProgram_{};
    unsigned int textureVertexArray_{};
    unsigned int textureVertexBuffer_{};
    static Renderer *current_;
};

#endif

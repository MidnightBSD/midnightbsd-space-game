#include "UiText.h"

#include <SDL2/SDL_ttf.h>

#define GL_GLEXT_PROTOTYPES 1
#include <GL/glcorearb.h>

#include <memory>

UiText *UiText::current_ = nullptr;

bool UiText::initialize(const std::string &assetDirectory, std::string &error) {
    if (TTF_Init() != 0) {
        error = TTF_GetError();
        return false;
    }
    assetDirectory_ = assetDirectory;
    initialized_ = true;
    current_ = this;
    return true;
}

void UiText::shutdown() {
    if (initialized_) {
        TTF_Quit();
        initialized_ = false;
    }
    if (current_ == this) {
        current_ = nullptr;
    }
}

UiText *UiText::current() {
    return current_;
}

void UiText::draw(std::string_view text, float x, float y, int pixelHeight,
                  FontFace face, RenderColor color) const {
    if (!initialized_ || text.empty() || Renderer::current() == nullptr) {
        return;
    }
    const char *filename = face == FontFace::Bold ? "Roboto-Bold.ttf" : "Roboto-Regular.ttf";
    const std::string path = assetDirectory_ + "/" + filename;
    std::unique_ptr<TTF_Font, decltype(&TTF_CloseFont)> font(TTF_OpenFont(path.c_str(), pixelHeight), TTF_CloseFont);
    if (!font) {
        return;
    }
    const SDL_Color white{255, 255, 255, 255};
    std::string copy(text);
    std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)> original(
        TTF_RenderUTF8_Blended(font.get(), copy.c_str(), white), SDL_FreeSurface);
    if (!original) {
        return;
    }
    std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)> surface(
        SDL_ConvertSurfaceFormat(original.get(), SDL_PIXELFORMAT_RGBA32, 0), SDL_FreeSurface);
    if (!surface) {
        return;
    }
    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, surface->pixels);
    Renderer::current()->texturedQuad(texture, x, y, static_cast<float>(surface->w),
                                      static_cast<float>(surface->h), color);
    glDeleteTextures(1, &texture);
}

float UiText::width(std::string_view text, int pixelHeight, FontFace face) const {
    if (!initialized_ || text.empty()) return 0.0F;
    const char *filename = face == FontFace::Bold ? "Roboto-Bold.ttf" : "Roboto-Regular.ttf";
    const std::string path = assetDirectory_ + "/" + filename;
    std::unique_ptr<TTF_Font, decltype(&TTF_CloseFont)> font(TTF_OpenFont(path.c_str(), pixelHeight), TTF_CloseFont);
    if (!font) return 0.0F;
    std::string copy(text);
    int width = 0;
    if (TTF_SizeUTF8(font.get(), copy.c_str(), &width, nullptr) != 0) return 0.0F;
    return static_cast<float>(width);
}

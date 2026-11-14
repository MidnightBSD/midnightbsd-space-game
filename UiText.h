#ifndef UI_TEXT_H
#define UI_TEXT_H

#include "Renderer.h"

#include <string>
#include <string_view>

enum class FontFace { Regular, Bold };

class UiText {
public:
    [[nodiscard]] bool initialize(const std::string &assetDirectory, std::string &error);
    void shutdown();
    void draw(std::string_view text, float x, float y, int pixelHeight,
              FontFace face, RenderColor color) const;
    [[nodiscard]] float width(std::string_view text, int pixelHeight, FontFace face) const;

    [[nodiscard]] static UiText *current();

private:
    std::string assetDirectory_;
    bool initialized_{};
    static UiText *current_;
};

#endif

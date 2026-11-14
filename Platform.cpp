#include "Platform.h"

#include <algorithm>

namespace {
ApplicationScreen currentScreen = ApplicationScreen::Title;
DeadlineQueue gameCallbacks;
}

void setActiveScreen(ApplicationScreen screen) {
    currentScreen = screen;
}

ApplicationScreen activeScreen() {
    return currentScreen;
}

std::uint64_t platformMilliseconds() {
    return SDL_GetTicks64();
}

void scheduleGameCallback(std::uint64_t delayMilliseconds, void (*callback)(int), int parameter) {
    gameCallbacks.schedule(platformMilliseconds() + delayMilliseconds,
                           [callback, parameter] { callback(parameter); });
}

void runScheduledGameCallbacks() {
    gameCallbacks.runDue(platformMilliseconds());
}

std::optional<int> legacyKeyFromSdl(SDL_Keycode keycode) {
    if (keycode >= 1 && keycode <= 127) {
        return static_cast<int>(keycode);
    }

    if (keycode >= SDLK_F1 && keycode <= SDLK_F12) {
        return -1 - static_cast<int>(keycode - SDLK_F1);
    }

    switch (keycode) {
    case SDLK_LEFT: return LegacyKeyLeft;
    case SDLK_UP: return LegacyKeyUp;
    case SDLK_RIGHT: return LegacyKeyRight;
    case SDLK_DOWN: return LegacyKeyDown;
    case SDLK_PAGEUP: return LegacyKeyPageUp;
    case SDLK_PAGEDOWN: return LegacyKeyPageDown;
    case SDLK_HOME: return LegacyKeyHome;
    case SDLK_END: return LegacyKeyEnd;
    case SDLK_INSERT: return LegacyKeyInsert;
    case SDLK_KP_ENTER: return 3;
    case SDLK_KP_0: return '0';
    case SDLK_KP_1: return '1';
    case SDLK_KP_2: return '2';
    case SDLK_KP_3: return '3';
    case SDLK_KP_4: return '4';
    case SDLK_KP_5: return '5';
    case SDLK_KP_6: return '6';
    case SDLK_KP_7: return '7';
    case SDLK_KP_8: return '8';
    case SDLK_KP_9: return '9';
    default: return std::nullopt;
    }
}

LogicalViewport makeLogicalViewport(int drawableWidth, int drawableHeight,
                                    int logicalWidth, int logicalHeight) {
    if (drawableWidth <= 0 || drawableHeight <= 0 || logicalWidth <= 0 || logicalHeight <= 0) {
        return {};
    }
    const float scale = std::min(static_cast<float>(drawableWidth) / logicalWidth,
                                 static_cast<float>(drawableHeight) / logicalHeight);
    const int width = static_cast<int>(logicalWidth * scale);
    const int height = static_cast<int>(logicalHeight * scale);
    return {(drawableWidth - width) / 2, (drawableHeight - height) / 2, width, height};
}

std::optional<LogicalPoint> logicalPointFromWindow(
    int mouseX, int mouseY, int windowWidth, int windowHeight,
    const LogicalViewport &drawableViewport, int drawableWidth, int drawableHeight,
    int logicalWidth, int logicalHeight) {
    if (windowWidth <= 0 || windowHeight <= 0 || drawableWidth <= 0 || drawableHeight <= 0 ||
        drawableViewport.width <= 0 || drawableViewport.height <= 0) {
        return std::nullopt;
    }
    const float drawableX = static_cast<float>(mouseX) * drawableWidth / windowWidth;
    const float drawableY = static_cast<float>(mouseY) * drawableHeight / windowHeight;
    if (drawableX < drawableViewport.x || drawableY < drawableViewport.y ||
        drawableX >= drawableViewport.x + drawableViewport.width ||
        drawableY >= drawableViewport.y + drawableViewport.height) {
        return std::nullopt;
    }
    return LogicalPoint{
        (drawableX - drawableViewport.x) * logicalWidth / drawableViewport.width,
        logicalHeight - (drawableY - drawableViewport.y) * logicalHeight / drawableViewport.height};
}

void DeadlineQueue::schedule(std::uint64_t dueAtMilliseconds, Task task) {
    items_.push({dueAtMilliseconds, nextSequence_++, std::move(task)});
}

void DeadlineQueue::runDue(std::uint64_t nowMilliseconds) {
    while (!items_.empty() && items_.top().dueAtMilliseconds <= nowMilliseconds) {
        auto task = std::move(items_.top().task);
        items_.pop();
        task();
    }
}

std::optional<std::uint64_t> DeadlineQueue::nextDeadline() const {
    if (items_.empty()) {
        return std::nullopt;
    }
    return items_.top().dueAtMilliseconds;
}

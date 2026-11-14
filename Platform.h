#ifndef PLATFORM_H
#define PLATFORM_H

#include <cstdint>
#include <functional>
#include <optional>
#include <queue>
#include <vector>

#include <SDL2/SDL.h>

inline constexpr int LegacyKeyLeft = -100;
inline constexpr int LegacyKeyUp = -101;
inline constexpr int LegacyKeyRight = -102;
inline constexpr int LegacyKeyDown = -103;
inline constexpr int LegacyKeyPageUp = -104;
inline constexpr int LegacyKeyPageDown = -105;
inline constexpr int LegacyKeyHome = -106;
inline constexpr int LegacyKeyEnd = -107;
inline constexpr int LegacyKeyInsert = -108;

enum class ApplicationScreen { Title, Configure, Game };

void setActiveScreen(ApplicationScreen screen);
[[nodiscard]] ApplicationScreen activeScreen();
[[nodiscard]] std::uint64_t platformMilliseconds();
void scheduleGameCallback(std::uint64_t delayMilliseconds, void (*callback)(int), int parameter);
void runScheduledGameCallbacks();

[[nodiscard]] std::optional<int> legacyKeyFromSdl(SDL_Keycode keycode);

struct LogicalViewport {
    int x{};
    int y{};
    int width{};
    int height{};
};

struct LogicalPoint {
    float x{};
    float y{};
};

[[nodiscard]] LogicalViewport makeLogicalViewport(int drawableWidth, int drawableHeight,
                                                   int logicalWidth, int logicalHeight);
[[nodiscard]] std::optional<LogicalPoint> logicalPointFromWindow(
    int mouseX, int mouseY, int windowWidth, int windowHeight,
    const LogicalViewport &drawableViewport, int drawableWidth, int drawableHeight,
    int logicalWidth, int logicalHeight);

class DeadlineQueue {
public:
    using Task = std::function<void()>;

    void schedule(std::uint64_t dueAtMilliseconds, Task task);
    void runDue(std::uint64_t nowMilliseconds);
    [[nodiscard]] std::optional<std::uint64_t> nextDeadline() const;

private:
    struct Item {
        std::uint64_t dueAtMilliseconds;
        std::uint64_t sequence;
        Task task;
    };
    struct Compare {
        bool operator()(const Item &left, const Item &right) const {
            return left.dueAtMilliseconds == right.dueAtMilliseconds
                ? left.sequence > right.sequence
                : left.dueAtMilliseconds > right.dueAtMilliseconds;
        }
    };

    std::priority_queue<Item, std::vector<Item>, Compare> items_;
    std::uint64_t nextSequence_{};
};

#endif

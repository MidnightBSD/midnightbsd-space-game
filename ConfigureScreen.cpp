#include "Allenroids.h"
#include "ConfigureScreen.h"
#include "Platform.h"
#include "Renderer.h"
#include "TitleScreen.h"
#include "UiText.h"

#include <array>
#include <cstdio>
#include <cstring>

extern GLubyte playerColors[][3];

std::array<MyButton, MAX_NUMBER_PLAYERS> configureButtons;
std::array<std::array<MyButton, NUMBER_ACTIONS>, MAX_NUMBER_PLAYERS> playerButtons;
int buttonWidth;
int buttonHeight;
PlayerKeys playerKeys[MAX_NUMBER_PLAYERS];
int activeButton = -1;
int buttonBeingPressed = -1;

namespace {
RenderColor colorFromBytes(const GLubyte color[4]) {
    return {color[0] / 255.0F, color[1] / 255.0F, color[2] / 255.0F, color[3] / 255.0F};
}

void drawCentered(UiText &text, std::string_view value, float centerX, float y, int size,
                  FontFace face, RenderColor color) {
    text.draw(value, centerX - text.width(value, size, face) / 2.0F, y, size, face, color);
}

void drawButton(const MyButton &button, int id, int height) {
    auto *renderer = Renderer::current();
    auto *text = UiText::current();
    if (renderer == nullptr || text == nullptr) return;
    RenderColor background = colorFromBytes(button.color.data());
    if (activeButton == id) background = {128.0F / 255.0F, 0.0F, 64.0F / 255.0F, 128.0F / 255.0F};
    else if (buttonBeingPressed == id) { background.red *= 0.5F; background.green *= 0.5F; background.blue *= 0.5F; }
    drawCentered(*text, button.text.data(), button.position.x, button.position.y - 6.0F, 14,
                 FontFace::Regular, {1.0F, 1.0F, 1.0F, 1.0F});
    renderer->filledRectangle(button.position.x - buttonWidth / 2.0F, button.position.y - height / 2.0F,
                              static_cast<float>(buttonWidth), static_cast<float>(height), background);
}

void drawConfigureScreen() {
    auto *renderer = Renderer::current();
    auto *text = UiText::current();
    if (renderer == nullptr || text == nullptr) return;
    constexpr RenderColor white{1.0F, 1.0F, 1.0F, 1.0F};
    drawCentered(*text, "CONFIGURE KEYS", FRAMEWIDTH / 2.0F, FRAMEHEIGHT - 70.0F, 32, FontFace::Bold, white);
    drawCentered(*text, "Click a box and type the new key to change that control.", FRAMEWIDTH / 2.0F,
                 FRAMEHEIGHT - 112.0F, 14, FontFace::Regular, white);
    constexpr std::array<std::string_view, NUMBER_ACTIONS> actionNames{
        "Thrust", "Rotate CCW (Left)", "Rotate CW (Right)", "Fire", "Mystical Reverse*"};
    for (unsigned player = 1; player <= MAX_NUMBER_PLAYERS; ++player) {
        const float x = FRAMEWIDTH * 0.25F + player * FRAMEWIDTH * (0.75F / MAX_NUMBER_PLAYERS)
                      - FRAMEWIDTH * (0.75F / MAX_NUMBER_PLAYERS) / 2.0F;
        const GLubyte color[]{playerColors[P(player)][0], playerColors[P(player)][1], playerColors[P(player)][2], 255};
        drawCentered(*text, "Player " + std::to_string(player), x, FRAMEHEIGHT - 158.0F, 17, FontFace::Bold, colorFromBytes(color));
    }
    for (unsigned action = 0; action < NUMBER_ACTIONS; ++action) {
        const float y = FRAMEHEIGHT - (FRAMEHEIGHT * 3.0F / 8.0F + action * 36.0F) + 6.0F;
        text->draw(actionNames[action], FRAMEWIDTH / 16.0F, y, 14, FontFace::Regular, white);
    }
    for (unsigned player = 1; player <= MAX_NUMBER_PLAYERS; ++player)
        for (unsigned action = 0; action < NUMBER_ACTIONS; ++action)
            drawButton(playerButtons[P(player)][action], 100 * static_cast<int>(player) + static_cast<int>(action), 24);
    drawButton(configureButtons[0], 0, 32);
    drawButton(configureButtons[1], 1, 32);
    text->draw("*Nobody seems to know how this works, and it's best not to ask.", FRAMEWIDTH / 16.0F,
               FRAMEHEIGHT / 32.0F, 11, FontFace::Regular, {72.0F / 255.0F, 72.0F / 255.0F, 72.0F / 255.0F, 1.0F});
}

int hitTest(const MyButton *buttons, std::size_t count, int height, int modifier, float x, float y) {
    for (std::size_t index = 0; index < count; ++index) {
        const MyButton &button = buttons[index];
        if (x >= button.position.x - buttonWidth / 2.0F && x <= button.position.x + buttonWidth / 2.0F &&
            y >= button.position.y - height / 2.0F && y <= button.position.y + height / 2.0F)
            return static_cast<int>(index) + modifier;
    }
    return -1;
}
}

void configureInit() {
    setActiveScreen(ApplicationScreen::Configure);
    activeButton = -1;
    buttonWidth = 128;
    buttonHeight = 24;
    for (unsigned action = 0; action < NUMBER_ACTIONS; ++action) {
        for (unsigned player = 1; player <= MAX_NUMBER_PLAYERS; ++player) {
            const float x = FRAMEWIDTH * 0.25F + player * FRAMEWIDTH * (0.75F / MAX_NUMBER_PLAYERS)
                          - FRAMEWIDTH * (0.75F / MAX_NUMBER_PLAYERS) / 2.0F;
            const float y = FRAMEHEIGHT - (FRAMEHEIGHT * 3.0F / 8.0F + action * 36.0F);
            PlayerKeys &keys = playerKeys[P(player)];
            const int value = action == 0 ? keys.thrust : action == 1 ? keys.rotateCounterClockwise
                : action == 2 ? keys.rotateClockwise : action == 3 ? keys.fire : keys.reverse;
            MyButton &button = playerButtons[P(player)][action];
            setButtonText(button, convertASCIItoText(value));
            button.position = {x, y};
            button.color = {playerColors[P(player)][0], playerColors[P(player)][1], playerColors[P(player)][2], 128};
        }
    }
    setButtonText(configureButtons[0], "Cancel");
    configureButtons[0].position = {FRAMEWIDTH / 3.0F, FRAMEHEIGHT / 8.0F};
    configureButtons[0].color = {255, 0, 0, 128};
    setButtonText(configureButtons[1], "OK");
    configureButtons[1].position = {FRAMEWIDTH * 2.0F / 3.0F, FRAMEHEIGHT / 8.0F};
    configureButtons[1].color = {0, 255, 0, 128};
}

void configureDisplay() { drawStarfield(); drawConfigureScreen(); }

void writeConfiguration() {
    if (FILE *file = std::fopen("Key Configuration", "w")) {
        for (PlayerKeys keys : playerKeys) {
#if defined(__APPLE__) && defined(__LITTLE_ENDIAN__)
            flipBytes(&keys.thrust, sizeof(keys.thrust));
            flipBytes(&keys.rotateCounterClockwise, sizeof(keys.rotateCounterClockwise));
            flipBytes(&keys.rotateClockwise, sizeof(keys.rotateClockwise));
            flipBytes(&keys.fire, sizeof(keys.fire));
            flipBytes(&keys.reverse, sizeof(keys.reverse));
#endif
            std::fwrite(&keys, sizeof(keys), 1, file);
        }
        std::fclose(file);
    }
}

void readConfiguration() {
    FILE *file = std::fopen("Key Configuration", "r");
    if (file == nullptr) { restoreDefaults(); return; }
    for (PlayerKeys &keys : playerKeys) {
        if (std::fread(&keys, sizeof(keys), 1, file) != 1) { std::fclose(file); restoreDefaults(); return; }
#if defined(__APPLE__) && defined(__LITTLE_ENDIAN__)
        flipBytes(&keys.thrust, sizeof(keys.thrust));
        flipBytes(&keys.rotateCounterClockwise, sizeof(keys.rotateCounterClockwise));
        flipBytes(&keys.rotateClockwise, sizeof(keys.rotateClockwise));
        flipBytes(&keys.fire, sizeof(keys.fire));
        flipBytes(&keys.reverse, sizeof(keys.reverse));
#endif
    }
    std::fclose(file);
}

void restoreDefaults() {
    playerKeys[P(1)] = {LegacyKeyUp, LegacyKeyLeft, LegacyKeyRight, ' ', LegacyKeyDown};
    playerKeys[P(2)] = {'8', '4', '6', '0', '5'};
    writeConfiguration();
}

std::string_view convertASCIItoText(int key) {
    static std::array<char, 2> printable{};
    if (key >= 33 && key < 127) { printable[0] = static_cast<char>(key); return {printable.data(), 1}; }
    switch (key) {
    case 0: return "Unassigned";
    case 3: return "Enter";
    case 8: return "Delete";
    case 9: return "Tab";
    case 13: return "Return";
    case 32: return "Space";
    case LegacyKeyLeft: return "Left Arrow";
    case LegacyKeyUp: return "Up Arrow";
    case LegacyKeyRight: return "Right Arrow";
    case LegacyKeyDown: return "Down Arrow";
    case LegacyKeyPageUp: return "Page Up";
    case LegacyKeyPageDown: return "Page Down";
    case LegacyKeyHome: return "Home";
    case LegacyKeyEnd: return "End";
    case LegacyKeyInsert: return "Insert";
    default:
        if (key <= -1 && key >= -12) { printable[0] = 'F'; return key == -1 ? "F1" : key == -2 ? "F2" : key == -3 ? "F3" : key == -4 ? "F4" : key == -5 ? "F5" : key == -6 ? "F6" : key == -7 ? "F7" : key == -8 ? "F8" : key == -9 ? "F9" : key == -10 ? "F10" : key == -11 ? "F11" : "F12"; }
        return "Invalid";
    }
}

void configureHandleLogicalMouse(bool pressed, float x, float y) {
    int hit = hitTest(configureButtons.data(), configureButtons.size(), 32, 0, x, y);
    if (hit == -1) hit = hitTest(playerButtons[P(1)].data(), playerButtons[P(1)].size(), 24, 100, x, y);
    if (hit == -1) hit = hitTest(playerButtons[P(2)].data(), playerButtons[P(2)].size(), 24, 200, x, y);
    if (pressed) { buttonBeingPressed = hit; return; }
    if (buttonBeingPressed == hit) {
        if (hit == 0) titleInit();
        else if (hit == 1) { writeConfiguration(); titleInit(); }
        else if (hit >= 100) activeButton = hit;
    }
    buttonBeingPressed = -1;
}

void assignKey(int key) {
    if (activeButton < 100) return;
    const int player = activeButton / 100;
    const int action = activeButton % 100;
    if (player < 1 || player > MAX_NUMBER_PLAYERS || action < 0 || action >= NUMBER_ACTIONS) return;
    PlayerKeys &keys = playerKeys[P(player)];
    if (action == 0) keys.thrust = key;
    else if (action == 1) keys.rotateCounterClockwise = key;
    else if (action == 2) keys.rotateClockwise = key;
    else if (action == 3) keys.fire = key;
    else keys.reverse = key;
    setButtonText(playerButtons[P(player)][action], convertASCIItoText(key));
    activeButton = -1;
}

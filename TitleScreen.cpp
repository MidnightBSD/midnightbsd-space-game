#include "Allenroids.h"
#include "ConfigureScreen.h"
#include "Platform.h"
#include "Renderer.h"
#include "TitleScreen.h"
#include "UiText.h"

#include <cstdio>
#include <cstring>

extern GLubyte playerColors[][3];
extern int buttonBeingPressed;
extern int buttonWidth;
extern int buttonHeight;
extern short numberOfPlayers;
extern Player playerObjects[MAX_NUMBER_PLAYERS];
extern boolean paused;

std::array<MyButton, 3> titleButtons;
boolean enterHighScoredMode = false;
Score highscores[NUMBER_HIGH_SCORES];
Score newScore;
short indexNewScore;
short playerNumberWithScore;

namespace {
RenderColor colorFromBytes(const GLubyte color[4]) {
    return {color[0] / 255.0F, color[1] / 255.0F, color[2] / 255.0F, color[3] / 255.0F};
}

void drawCentered(UiText &text, std::string_view value, float centerX, float y, int size,
                  FontFace face, RenderColor color) {
    text.draw(value, centerX - text.width(value, size, face) / 2.0F, y, size, face, color);
}

void drawButton(const MyButton &button) {
    auto *renderer = Renderer::current();
    auto *text = UiText::current();
    if (renderer == nullptr || text == nullptr) return;
    const std::string_view label(button.text.data());
    drawCentered(*text, label, button.position.x, button.position.y - 6.0F, 16,
                 FontFace::Regular, {1.0F, 1.0F, 1.0F, 1.0F});
    renderer->filledRectangle(button.position.x - buttonWidth / 2.0F,
                              button.position.y - buttonHeight / 2.0F,
                              static_cast<float>(buttonWidth), static_cast<float>(buttonHeight),
                              colorFromBytes(button.color.data()));
}

void drawHighScores() {
    auto *text = UiText::current();
    auto *renderer = Renderer::current();
    if (text == nullptr || renderer == nullptr) return;
    constexpr RenderColor white{1.0F, 1.0F, 1.0F, 1.0F};
    const std::string heading = enterHighScoredMode
        ? "Player " + std::to_string(playerNumberWithScore) + " earned a new high score!"
        : "High Scores";
    drawCentered(*text, heading, FRAMEWIDTH / 2.0F, FRAMEHEIGHT - 144.0F, 24, FontFace::Bold, white);
    constexpr float left = 80.0F;
    constexpr float nameColumn = left + 30.0F;
    constexpr float levelColumn = 350.0F;
    constexpr float scoreColumn = 425.0F;
    constexpr float dateColumn = 505.0F;
    text->draw("#", left, FRAMEHEIGHT - 180.0F, 13, FontFace::Bold, white);
    text->draw("Name", nameColumn, FRAMEHEIGHT - 180.0F, 13, FontFace::Bold, white);
    text->draw("Level", levelColumn, FRAMEHEIGHT - 180.0F, 13, FontFace::Bold, white);
    text->draw("Score", scoreColumn, FRAMEHEIGHT - 180.0F, 13, FontFace::Bold, white);
    text->draw("Date", dateColumn, FRAMEHEIGHT - 180.0F, 13, FontFace::Bold, white);
    renderer->filledRectangle(left, FRAMEHEIGHT - 187.0F, 480.0F, 1.0F, white);
    for (int index = 0; index < NUMBER_HIGH_SCORES; ++index) {
        const bool entering = enterHighScoredMode && index == indexNewScore;
        const Score &row = entering ? newScore : highscores[index];
        const RenderColor color = entering ? RenderColor{1.0F, 0.0F, 0.0F, 1.0F} : white;
        char rank[8]{};
        char level[16]{};
        char score[24]{};
        char date[20]{};
        std::snprintf(rank, sizeof(rank), "%d.", index + 1);
        std::snprintf(level, sizeof(level), "%d", row.level);
        std::snprintf(score, sizeof(score), "%d", row.score);
        if (const std::tm *time = std::localtime(&row.date); time != nullptr && !entering)
            std::strftime(date, sizeof(date), "%b. %d, %Y", time);
        const std::string name = entering ? std::string(newScore.name) + "_" : row.name;
        const float y = FRAMEHEIGHT - 207.0F - index * 14.0F;
        text->draw(rank, left, y, 13, FontFace::Regular, color);
        text->draw(name, nameColumn, y, 13, FontFace::Regular, color);
        text->draw(level, levelColumn, y, 13, FontFace::Regular, color);
        text->draw(score, scoreColumn, y, 13, FontFace::Regular, color);
        if (!entering) text->draw(date, dateColumn, y, 13, FontFace::Regular, color);
    }
    if (!enterHighScoredMode) {
        drawCentered(*text, "MidnightBSD Space Game " ALLENROIDS_VERSION
                     ": This masterpiece brought to you by Allen M. Smith, 2002-" +
                     std::to_string(ALLENROIDS_VERSION_DATE),
                     FRAMEWIDTH / 2.0F, FRAMEHEIGHT / 32.0F, 11, FontFace::Regular, white);
    }
}

void activateTitleButton(int button) {
    switch (button) {
    case 0: gameInit(1); break;
    case 1: gameInit(2); break;
    case 2: configureInit(); break;
    default: break;
    }
}
}

void titleInit() {
    setActiveScreen(ApplicationScreen::Title);
    readHighScores();
    readConfiguration();
    spawnStarfield();
    buttonWidth = 128;
    buttonHeight = 32;
    setButtonText(titleButtons[0], "One-player Game");
    setButtonText(titleButtons[1], "Two-player Game");
    setButtonText(titleButtons[2], "Configure");
    for (std::size_t index = 0; index < titleButtons.size(); ++index) {
        MyButton &button = titleButtons[index];
        button.position = {FRAMEWIDTH * static_cast<float>(index + 1) / 4.0F, FRAMEHEIGHT * 3.0F / 16.0F};
        button.color[0] = index < 2 ? playerColors[index][0] : 255;
        button.color[1] = index < 2 ? playerColors[index][1] : 0;
        button.color[2] = index < 2 ? playerColors[index][2] : 0;
        button.color[3] = 128;
    }
}

void titleDisplay() {
    drawStarfield();
    printTitle();
    printHighScores();
    if (!enterHighScoredMode) for (const MyButton &button : titleButtons) drawButton(button);
}

void printTitle() {
    if (auto *text = UiText::current())
        drawCentered(*text, "MidnightBSD Space Game", FRAMEWIDTH / 2.0F, FRAMEHEIGHT - 70.0F,
                     34, FontFace::Bold, {1.0F, 1.0F, 1.0F, 1.0F});
}

void printHighScores() { drawHighScores(); }

void writeHighScores() {
    if (FILE *file = std::fopen("High Scores", "w+")) {
        for (Score score : highscores) {
#if defined(__APPLE__) && defined(__LITTLE_ENDIAN__)
            flipBytes(&score.level, sizeof(score.level));
            flipBytes(&score.score, sizeof(score.score));
            flipBytes(&score.date, sizeof(score.date));
#endif
            std::fwrite(&score, sizeof(score), 1, file);
        }
        std::fclose(file);
    }
}

void readHighScores() {
    FILE *file = std::fopen("High Scores", "r");
    if (file == nullptr) { writeHighScores(); return; }
    for (Score &score : highscores) {
        if (std::fread(&score, sizeof(score), 1, file) != 1) {
            std::fclose(file);
            std::memset(highscores, 0, sizeof(highscores));
            writeHighScores();
            return;
        }
#if defined(__APPLE__) && defined(__LITTLE_ENDIAN__)
        flipBytes(&score.level, sizeof(score.level));
        flipBytes(&score.score, sizeof(score.score));
        flipBytes(&score.date, sizeof(score.date));
#endif
    }
    std::fclose(file);
}

void newHighScore(int player) {
    indexNewScore = -1;
    for (int index = 0; index < NUMBER_HIGH_SCORES; ++index) {
        if (highscores[index].score < playerObjects[P(player)].score) { indexNewScore = index; break; }
    }
    if (indexNewScore == -1) {
        if (numberOfPlayers > player) newHighScore(player + 1);
        return;
    }
    for (int index = NUMBER_HIGH_SCORES - 1; index > indexNewScore; --index) highscores[index] = highscores[index - 1];
    newScore = {};
    newScore.level = static_cast<short>(playerObjects[P(player)].highestLevelAttained);
    newScore.score = playerObjects[P(player)].score;
    newScore.date = std::time(nullptr);
    playerNumberWithScore = player;
    enterHighScoredMode = true;
}

void titleHandleLogicalMouse(bool pressed, float x, float y) {
    if (enterHighScoredMode) return;
    int hit = -1;
    for (std::size_t index = 0; index < titleButtons.size(); ++index) {
        const MyButton &button = titleButtons[index];
        if (x >= button.position.x - buttonWidth / 2.0F && x <= button.position.x + buttonWidth / 2.0F &&
            y >= button.position.y - buttonHeight / 2.0F && y <= button.position.y + buttonHeight / 2.0F) {
            hit = static_cast<int>(index); break;
        }
    }
    if (pressed) { buttonBeingPressed = hit; return; }
    if (buttonBeingPressed == hit) activateTitleButton(hit);
    buttonBeingPressed = -1;
}

void titleHandleKeyboard(unsigned char key, int, int) {
    if (!enterHighScoredMode) return;
    if (key == 13 || key == 3) {
        highscores[indexNewScore] = newScore;
        enterHighScoredMode = false;
        writeHighScores();
        if (numberOfPlayers > playerNumberWithScore) newHighScore(playerNumberWithScore + 1);
    } else if (key == 8 || key == 127) {
        const std::size_t length = std::strlen(newScore.name);
        if (length > 0) newScore.name[length - 1] = '\0';
    } else if (key >= 32 && key < 127) {
        const std::size_t length = std::strlen(newScore.name);
        if (length < sizeof(newScore.name) - 1) {
            newScore.name[length] = static_cast<char>(key);
            newScore.name[length + 1] = '\0';
        }
    }
}

int main(int, char **) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) { std::fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError()); return 1; }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_Window *window = SDL_CreateWindow("MidnightBSD Space Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        FRAMEWIDTH, FRAMEHEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (window == nullptr) { std::fprintf(stderr, "SDL window creation failed: %s\n", SDL_GetError()); SDL_Quit(); return 1; }
    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (context == nullptr) { std::fprintf(stderr, "OpenGL 3.3 context creation failed: %s\n", SDL_GetError()); SDL_DestroyWindow(window); SDL_Quit(); return 1; }
    SDL_GL_SetSwapInterval(1);
    Renderer renderer;
    UiText text;
    std::string error;
    if (!renderer.initialize(error) || !text.initialize("assets/fonts", error)) {
        std::fprintf(stderr, "Renderer initialization failed: %s\n", error.c_str());
        text.shutdown(); renderer.shutdown(); SDL_GL_DeleteContext(context); SDL_DestroyWindow(window); SDL_Quit(); return 1;
    }
    titleInit();
    SDL_StartTextInput();
    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) { running = false; continue; }
            if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP) {
                int windowWidth = 0, windowHeight = 0, drawableWidth = 0, drawableHeight = 0;
                SDL_GetWindowSize(window, &windowWidth, &windowHeight); SDL_GL_GetDrawableSize(window, &drawableWidth, &drawableHeight);
                const auto point = logicalPointFromWindow(event.button.x, event.button.y, windowWidth, windowHeight,
                    makeLogicalViewport(drawableWidth, drawableHeight, FRAMEWIDTH, FRAMEHEIGHT), drawableWidth, drawableHeight, FRAMEWIDTH, FRAMEHEIGHT);
                if (event.button.button == SDL_BUTTON_LEFT && point) {
                    const bool pressed = event.type == SDL_MOUSEBUTTONDOWN;
                    if (activeScreen() == ApplicationScreen::Title) titleHandleLogicalMouse(pressed, point->x, point->y);
                    else if (activeScreen() == ApplicationScreen::Configure) configureHandleLogicalMouse(pressed, point->x, point->y);
                }
            }
            if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
                if (const auto key = legacyKeyFromSdl(event.key.keysym.sym)) {
                    if (activeScreen() == ApplicationScreen::Game) handleKey(*key);
                    else if (activeScreen() == ApplicationScreen::Configure) assignKey(*key);
                    else if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER || event.key.keysym.sym == SDLK_BACKSPACE || event.key.keysym.sym == SDLK_DELETE)
                        titleHandleKeyboard(static_cast<unsigned char>(*key), 0, 0);
                }
            }
            if (event.type == SDL_KEYUP) {
                if (const auto key = legacyKeyFromSdl(event.key.keysym.sym); key && activeScreen() == ApplicationScreen::Game) handleKeyUp(*key);
            }
            if (event.type == SDL_TEXTINPUT && activeScreen() == ApplicationScreen::Title && enterHighScoredMode) {
                for (const unsigned char *value = reinterpret_cast<const unsigned char *>(event.text.text); *value != '\0'; ++value) titleHandleKeyboard(*value, 0, 0);
            }
        }
        runScheduledGameCallbacks();
        if (activeScreen() == ApplicationScreen::Game && !paused) idle();
        int drawableWidth = 0, drawableHeight = 0;
        SDL_GL_GetDrawableSize(window, &drawableWidth, &drawableHeight);
        renderer.beginFrame(makeLogicalViewport(drawableWidth, drawableHeight, FRAMEWIDTH, FRAMEHEIGHT), {0.0F, 0.0F, 0.0F, 1.0F});
        switch (activeScreen()) {
        case ApplicationScreen::Title: titleDisplay(); break;
        case ApplicationScreen::Configure: configureDisplay(); break;
        case ApplicationScreen::Game: display(); break;
        }
        SDL_GL_SwapWindow(window);
    }
    SDL_StopTextInput();
    text.shutdown(); renderer.shutdown(); SDL_GL_DeleteContext(context); SDL_DestroyWindow(window); SDL_Quit();
    return 0;
}

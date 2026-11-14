#include "Asteroid.h"
#include "Crystal.h"
#include "Kerblooie.h"
#include "Platform.h"
#include "Renderer.h"
#include "Shot.h"
#include "Spaceship.h"
#include "Starfield.h"
#include "UiText.h"

#include <SDL2/SDL.h>

#define GL_GLEXT_PROTOTYPES 1
#include <GL/glcorearb.h>

#include <cassert>
#include <string>

int main() {
    assert(SDL_Init(SDL_INIT_VIDEO) == 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_Window *window = SDL_CreateWindow("renderer smoke", SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED, 64, 64,
                                          SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
    assert(window != nullptr);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    assert(context != nullptr);

    Renderer renderer;
    std::string error;
    assert(renderer.initialize(error));
    renderer.beginFrame(makeLogicalViewport(64, 64, 640, 480), {});
    const RenderVertex triangle[] = {
        {100.0F, 100.0F, 0.0F, {1.0F, 0.0F, 0.0F, 1.0F}},
        {300.0F, 100.0F, 0.0F, {0.0F, 1.0F, 0.0F, 1.0F}},
        {200.0F, 300.0F, 0.0F, {0.0F, 0.0F, 1.0F, 1.0F}},
    };
    renderer.triangles(triangle);
    Starfield starfield;
    starfield.SetStarCount(8);
    starfield.Draw();
    UiText text;
    assert(text.initialize("assets/fonts", error));
    assert(text.width("OpenGL 3.3", 18, FontFace::Regular) > 0.0F);
    renderer.filledRectangle(10.0F, 10.0F, 160.0F, 32.0F, {0.0F, 0.5F, 1.0F, 0.5F});
    text.draw("OpenGL 3.3", 20.0F, 20.0F, 18, FontFace::Regular,
              {1.0F, 1.0F, 1.0F, 1.0F});
    text.shutdown();
    Asteroid asteroid(ASTEROID_SMALL, 10.0F, 10.0F);
    asteroid.drawInGame();
    Shot shot;
    shot.position = {20.0F, 20.0F};
    shot.radius = SHOT_RADIUS;
    shot.color[0] = 255;
    shot.drawInGame();
    GLubyte color[] = {255, 64, 0};
    Kerblooie particle({30.0F, 30.0F}, {}, 100, 6.0F, color);
    particle.drawInGame();
    Spaceship spaceship;
    spaceship.position = {60.0F, 60.0F};
    spaceship.radius = 20.0F;
    spaceship.maxStrength = 8;
    spaceship.strength = 8;
    spaceship.playerNumber = 1;
    spaceship.thrusting = true;
    spaceship.drawShieldsTimeToLive = 10;
    spaceship.drawInGame();
    Crystal crystal;
    crystal.position = {100.0F, 100.0F};
    crystal.radius = 8.0F;
    crystal.thickness = 2.0F;
    crystal.rotationAngle = 30.0F;
    crystal.color[0] = 64;
    crystal.color[1] = 192;
    crystal.color[2] = 255;
    crystal.color[3] = 128;
    crystal.drawInGame();
    assert(glGetError() == GL_NO_ERROR);
    renderer.shutdown();
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

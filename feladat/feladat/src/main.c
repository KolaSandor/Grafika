#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include <stdbool.h>
#include "input.h"
#include "render.h"
#include "model.h"

// globális modellpointer (render.h-ból)
Model* carModel = NULL;

int main(void) {
    if (SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }
    if (!(IMG_Init(IMG_INIT_PNG|IMG_INIT_JPG)&(IMG_INIT_PNG|IMG_INIT_JPG))) {
        SDL_Log("IMG_Init failed: %s", IMG_GetError());
        return 1;
    }

   SDL_Window* window = SDL_CreateWindow(
      "Autó vezetés",
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      800, 600,
      SDL_WINDOW_OPENGL
      | SDL_WINDOW_RESIZABLE
      | SDL_WINDOW_FULLSCREEN_DESKTOP
    );
    if (!window) {
      SDL_Log("Window creation failed: %s", SDL_GetError());
      return 1;
    }
    SDL_GLContext ctx = SDL_GL_CreateContext(window);
    if (!ctx) {
      SDL_Log("GL context creation failed: %s", SDL_GetError());
      return 1;
    }

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f);

    // OBJ + MTL + per‐anyag JPG textúrák beolvasása
    carModel = loadModel("assets/models/fiatjo.obj");
    if (!carModel) {
        SDL_Log("Model load failed");
        return 1;
    }

    // Autó induló állapot: nézzen előre (0°)
    Car    car    = { 0.0f, 0.5f, 0.0f, 180.0f };
    // Kamera induló pozíció: 10 egység távolság, 5 egység magasság, 0° yaw, enyhe lefelé nézés
    Camera camera = { 0.0f, 5.0f, 10.0f, -20.0f, 0.0f };
    bool   running = true;

    while (running) {
        handle_input(&running, &car);
        update_car(&car);

        // Kamera mindig car mögött: 10 egység távolság, 5 egység magasság
        float rad = camera.yaw * (float)M_PI / 50.0f;
        camera.x = car.x - sinf(rad) * 15.0f;
        camera.z = car.z + cosf(rad) * 15.0f;
        camera.y = car.y + 10.0f;

        render_scene(&camera, &car, window);
        SDL_Delay(16);
    }

    destroyModel(carModel);
    SDL_GL_DeleteContext(ctx);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return 0;
}

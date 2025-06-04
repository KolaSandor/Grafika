#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "input.h"
#include "render.h"
#include "model.h"
#include "texture.h"

// Globális változók (Egyetlen definíció)
Model* carModel = NULL;
GLuint helpTex = 0;
bool showHelp = false; // Toggle változó

// A help overlay kapcsolója
void handle_help_toggle(const SDL_Event* event) {
    if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_h)
        showHelp = !showHelp;
}

void draw_help_overlay(SDL_Window* window, GLuint helpTex) {
    if (!helpTex) return;
    int w, h;
    SDL_GetWindowSize(window, &w, &h);

    // --- 2D ortho mód beállítása ---
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, w, h, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, helpTex);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(1,1,1,1);
    glBegin(GL_QUADS);
      glTexCoord2f(0,0); glVertex2f(0,0);
      glTexCoord2f(1,0); glVertex2f(w,0);
      glTexCoord2f(1,1); glVertex2f(w,h);
      glTexCoord2f(0,1); glVertex2f(0,h);
    glEnd();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);

    // Mátrix visszaállítása
    glPopMatrix();
    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

int main(void) {
    if (SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }
    if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) & (IMG_INIT_PNG | IMG_INIT_JPG))) {
        SDL_Log("IMG_Init failed: %s", SDL_GetError());
        return 1;
    }

    // Teljes képernyős ablak, okostelefon-szerű „borderless”
    SDL_Window* window = SDL_CreateWindow(
        "Autó vezetés",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 720,
        SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP
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
    // VSync bekapcsolása
    SDL_GL_SetSwapInterval(1);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f);

    carModel = loadModel("assets/models/fiatjo.obj");
    if (!carModel) {
        SDL_Log("Model load failed");
        return 1;
    }
    helpTex = loadTextureSimple("assets/help.png");
    if (!helpTex) {
        SDL_Log("Help image not found!");
    }

    // Kezdeti állapot
    Car car = { 0.0f, 0.5f, 0.0f, 180.0f };
    Camera camera = { 0.0f, 5.0f, 10.0f, -20.0f, 0.0f };
    bool running = true;

    // Utolsó időbélyeg frame-ek között (ms-ban)
    Uint32 lastTicks = SDL_GetTicks();

    while (running) {
        // 1) Eseménykezelés (egyszeri PollEvent)
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            handle_help_toggle(&event);
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                if (showHelp) {
                    showHelp = false;
                } else {
                    running = false;
                }
            }
        }

        // 2) DeltaTime kiszámítása
        Uint32 currentTicks = SDL_GetTicks();
        float deltaTime = (currentTicks - lastTicks) / 1000.0f; // másodpercben
        lastTicks = currentTicks;

        // 3) Billentyűzet‐állapot olvasása és mozgás‐frissítés
        handle_input(deltaTime, &car);

        // 4) Autó frissítése (ütközések, pozíció)
        update_car(deltaTime, &car);

        // 5) Kamera követése
        float rad = camera.yaw * (float)M_PI / 50.0f;
        camera.x = car.x - sinf(rad) * 15.0f;
        camera.z = car.z + cosf(rad) * 15.0f;
        camera.y = car.y + 10.0f;

        // 6) Kirajzoljuk a 3D-scene-t
        render_scene(&camera, &car, window);

        // 7) Ha kell, rárajzoljuk a help overlay-t
        if (showHelp) {
            draw_help_overlay(window, helpTex);
        }

        // 8) Buffer‐váltás (csak egyszer)
        SDL_GL_SwapWindow(window);

        // 9) Rövid várakozás (~60 FPS)
        SDL_Delay(1); // minimális 1 ms, a VSync kijavítja a többit
    }

    destroyModel(carModel);
    SDL_GL_DeleteContext(ctx);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    if (helpTex) glDeleteTextures(1, &helpTex);
    return 0;
}

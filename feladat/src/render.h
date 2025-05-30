#ifndef RENDER_H
#define RENDER_H

#include <SDL2/SDL.h>
#include "input.h"
#include "model.h"

// Camera struktúra
typedef struct Camera {
    float x, y, z;
    float pitch, yaw;   // dőlés és elfordulás fokban
} Camera;

// Globális modellpointer (main.c-ben definiáljuk)
extern Model* carModel;

// Jelenet kirajzolása: út + betöltött modell anyagonként textúrázva
void render_scene(const Camera* camera,
                  const Car*    car,
                  SDL_Window*   window);

#endif // RENDER_H

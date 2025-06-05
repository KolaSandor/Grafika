// render.h
#ifndef RENDER_H
#define RENDER_H

#include <SDL2/SDL.h>
#include "input.h"
#include "model.h"
#include <GL/gl.h>

extern GLuint helpTex;

// Árnyékrajzoló függvények prototípusai
// (belső használatra, ha később máshol is hívni szeretnénk őket)
void draw_tree_shadow(float x, float z);
void draw_car_shadow(const Car* car);

typedef struct Camera {
    float x, y, z;
    float pitch, yaw;
} Camera;

extern Model* carModel;

// Fő kirajzoló függvény
void render_scene(const Camera* camera,
                  const Car*    car,
                  SDL_Window*   window);

#endif // RENDER_H

// render.h
#ifndef RENDER_H
#define RENDER_H

#include <SDL2/SDL.h>
#include "input.h"
#include "model.h"
#include <GL/gl.h>

extern GLuint helpTex;

typedef struct Camera {
    float x, y, z;
    float pitch, yaw;
} Camera;

extern Model* carModel;

void render_scene(const Camera* camera,
                  const Car*    car,
                  SDL_Window*   window);

#endif // RENDER_H
// render.h
#ifndef RENDER_H
#define RENDER_H

#include <SDL2/SDL.h>
#include "input.h"
#include "model.h"
#include <GL/gl.h>

extern GLuint helpTex;

typedef struct Camera {
    float x, y, z;
    float pitch, yaw;
} Camera;

extern Model* carModel;

void render_scene(const Camera* camera,
                  const Car*    car,
                  SDL_Window*   window);

#endif // RENDER_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <math.h>
#include "input.h"

// Autó sebességadat
static float carSpeed = 0.0f;

// Gyorsítás, súrlódás
const float accel     = 0.05f;   // erősebb gyorsulás
const float maxSpeed  = 2.0f;    // nagyobb max sebesség
const float friction  = 0.03f;   // enyhébb fékező hatás

// Megnövelt, fix fordulási sebesség (fok/frame)
const float turnSpeed = 2.5f;

void handle_input(bool* running, Car* car)
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            *running = false;
        }
        else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                *running = false;
            }
        }
    }

    const Uint8* keystates = SDL_GetKeyboardState(NULL);

    // Előre-hátra
    if (keystates[SDL_SCANCODE_S]) {
        carSpeed += accel;
        if (carSpeed > maxSpeed) carSpeed = maxSpeed;
    }
    else if (keystates[SDL_SCANCODE_W]) {
        carSpeed -= accel;
        if (carSpeed < -maxSpeed) carSpeed = -maxSpeed;
    }
    else {
        // Súrlódásos lassítás
        if (carSpeed > 0.0f) {
            carSpeed -= friction;
            if (carSpeed < 0.0f) carSpeed = 0.0f;
        } else if (carSpeed < 0.0f) {
            carSpeed += friction;
            if (carSpeed > 0.0f) carSpeed = 0.0f;
        }
    }

    // Balra-jobbra kanyarodás (most már mindig, nem csak mozgás közben)
    if (keystates[SDL_SCANCODE_A]) {
        car->rotationY += turnSpeed;
        if (car->rotationY >= 360.0f) car->rotationY -= 360.0f;
    }
    if (keystates[SDL_SCANCODE_D]) {
        car->rotationY -= turnSpeed;
        if (car->rotationY < 0.0f) car->rotationY += 360.0f;
    }
}

void update_car(Car* car)
{
    float rad = car->rotationY * (float)M_PI / 180.0f;
    car->x -= sinf(rad) * carSpeed;
    car->z -= cosf(rad) * carSpeed;
}

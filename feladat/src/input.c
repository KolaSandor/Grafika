#include <SDL2/SDL.h>
#include <stdbool.h>
#include <math.h>
#include "input.h"

// Autóhoz tartozó sebesség, világítás és "help" állapot—csak deklarációk
extern bool showHelp;         // definiálva: main.c-ben
float lightIntensity = 1.0f;  // definiálva: input.h-ban is, de itt nem inicializáljuk

static float carSpeed = 0.0f;

// Ezek most *sebességek másodpercenként*, nem frame‐per‐frame egységek:
//    accel: másodpercenkénti gyorsulás
//    maxSpeed: max sebesség
//    friction: másodpercenkénti lassulás (súrlódásból)
//    turnSpeed: fok/másodperc
const float accel     =  30.0f;  // pl. 2 egység/s²
const float maxSpeed  = 100.0f;  // pl. max 10 egység/s
const float friction  =  40.0f;  // pl. 1.5 egység/s² lassulás
const float turnSpeed = 90.0f;  // 90 fok/s (másodpercenként)

void handle_input(float deltaTime, Car* car) {
    // Ha help overlay aktív, ne változzon a sebesség és rotáció
    if (showHelp) return;

    const Uint8* keystates = SDL_GetKeyboardState(NULL);

    // Előre‐hátra (S / W)
    if (keystates[SDL_SCANCODE_S]) {
        carSpeed += accel * deltaTime;
        if (carSpeed > maxSpeed) carSpeed = maxSpeed;
    }
    else if (keystates[SDL_SCANCODE_W]) {
        carSpeed -= accel * deltaTime;
        if (carSpeed < -maxSpeed) carSpeed = -maxSpeed;
    }
    else {
        // Súrlódásos lassítás: ha mozog, lassul
        if (carSpeed > 0.0f) {
            carSpeed -= friction * deltaTime;
            if (carSpeed < 0.0f) carSpeed = 0.0f;
        } else if (carSpeed < 0.0f) {
            carSpeed += friction * deltaTime;
            if (carSpeed > 0.0f) carSpeed = 0.0f;
        }
    }

    // Balra‐jobbra kanyarodás (A / D)
    if (keystates[SDL_SCANCODE_A]) {
        car->rotationY += turnSpeed * deltaTime;
        if (car->rotationY >= 360.0f) car->rotationY -= 360.0f;
    }
    if (keystates[SDL_SCANCODE_D]) {
        car->rotationY -= turnSpeed * deltaTime;
        if (car->rotationY < 0.0f) car->rotationY += 360.0f;
    }

    // Fényerő + / - (billentyűk: +, = és -)
    if (keystates[SDL_SCANCODE_EQUALS] || keystates[SDL_SCANCODE_KP_PLUS]) {
        lightIntensity += 0.5f * deltaTime;
        if (lightIntensity > 3.0f) lightIntensity = 3.0f;
    }
    else if (keystates[SDL_SCANCODE_MINUS] || keystates[SDL_SCANCODE_KP_MINUS]) {
        lightIntensity -= 0.5f * deltaTime;
        if (lightIntensity < 0.1f) lightIntensity = 0.1f;
    }
}

void update_car(float deltaTime, Car* car) {
    // A sebesség carSpeed egység/másodperc, így a displacement:
    float distance = carSpeed * deltaTime;

    float rad = car->rotationY * (float)M_PI / 180.0f;
    float new_x = car->x - sinf(rad) * distance;
    float new_z = car->z - cosf(rad) * distance;

    // ------------ FÁK ütközése ------------
    const float treeRadius = 5.5f;
    const float carRadius  = 0.5f;
    for (float z = -1000.0f; z < 1000.0f; z += 60.0f) {
        // Bal oldali fa
        float tx = -16.0f, tz = z;
        float dx = new_x - tx, dz = new_z - tz;
        float dist = sqrtf(dx*dx + dz*dz);
        if (dist < (treeRadius + carRadius)) {
            carSpeed = 0;
            return;
        }
        // Jobb oldali fa
        tx = 16.0f;
        dx = new_x - tx;
        dz = new_z - tz;
        dist = sqrtf(dx*dx + dz*dz);
        if (dist < (treeRadius + carRadius)) {
            carSpeed = 0;
            return;
        }
    }

    // ------------ HEGYEK ütközése ------------
    const float offX[4] = { -150.0f, -75.0f, 75.0f, 150.0f };
    const float rowZ[5] = { -500.0f, -250.0f, 0.0f, 250.0f, 500.0f };

    // Háttér‐hegyek
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 5; ++j) {
            float hx = offX[i], hz = rowZ[j];
            float height = 40.0f + 12.0f * sinf((float)i*2 + (float)j*1.4f);
            float baseR  = height * (1.1f + 0.15f * cosf((float)j + i));
            float hegyRadius = baseR;
            float dx = new_x - hx, dz = new_z - hz;
            float dist = sqrtf(dx*dx + dz*dz);
            if (dist < (hegyRadius + carRadius)) {
                carSpeed = 0;
                return;
            }
        }
    }

    // Előtér‐hegyek
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 5; ++j) {
            float hx = offX[i], hz = rowZ[j] + 150.0f;
            float height = 60.0f + 15.0f * ((i + j) & 1);
            float baseR  = height * 0.8f;
            float hegyRadius = baseR;
            float dx = new_x - hx, dz = new_z - hz;
            float dist = sqrtf(dx*dx + dz*dz);
            if (dist < (hegyRadius + carRadius)) {
                carSpeed = 0;
                return;
            }
        }
    }

    // Ha nincs ütközés, frissítjük a pozíciót
    car->x = new_x;
    car->z = new_z;
}

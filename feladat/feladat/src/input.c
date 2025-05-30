#include <SDL2/SDL.h>
#include <stdbool.h>
#include <math.h>
#include "input.h"

// Autó sebességadat
static float carSpeed = 0.0f;
bool showHelp = false;

// Gyorsítás, súrlódás
const float accel     = 0.05f;   // erősebb gyorsulás
const float maxSpeed  = 2.0f;    // nagyobb max sebesség
const float friction  = 0.03f;   // enyhébb fékező hatás
float lightIntensity = 1.0f;

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
                // Ha help overlay van, csak azt zárja be
                if (showHelp) {
                    showHelp = false;
                } else {
                    *running = false;
                }
            }
            // H: help overlay ki/bekapcsolása
            else if (event.key.keysym.sym == SDLK_h) {
                showHelp = !showHelp;
            }
            // Fényerő növelése: + (vagy =)
            else if (event.key.keysym.sym == SDLK_PLUS 
                  || event.key.keysym.sym == SDLK_KP_PLUS 
                  || event.key.keysym.sym == SDLK_EQUALS) {
                lightIntensity += 0.1f;
                if (lightIntensity > 3.0f) lightIntensity = 3.0f;
            }
            // Fényerő csökkentése: -
            else if (event.key.keysym.sym == SDLK_MINUS 
                  || event.key.keysym.sym == SDLK_KP_MINUS) {
                lightIntensity -= 0.1f;
                if (lightIntensity < 0.1f) lightIntensity = 0.1f;
            }
        }
    }

    // Ha a help overlay látszik, ne legyen egyéb vezérlés
    if (showHelp)
        return;

    const Uint8* keystates = SDL_GetKeyboardState(NULL);

    // Előre-hátra (S/W)
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

    // Balra-jobbra kanyarodás (A/D)
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
    float new_x = car->x - sinf(rad) * carSpeed;
    float new_z = car->z - cosf(rad) * carSpeed;

    // ------------- FÁK ütközése -------------
    const float treeRadius = 5.5f;  // lomb gömb + törzs
    const float carRadius  = 0.5f;
    for (float z = -1000.0f; z < 1000.0f; z += 60.0f) {
        // Bal oldali fa
        float tx = -16.0f;
        float tz = z;
        float dx = new_x - tx;
        float dz = new_z - tz;
        float dist = sqrtf(dx*dx + dz*dz);
        if (dist < (treeRadius + carRadius)) {
            carSpeed = 0; // ütközés!
            return;
        }
        // Jobb oldali fa
        tx = 16.0f;
        dx = new_x - tx;
        dz = new_z - tz;
        dist = sqrtf(dx*dx + dz*dz);
        if (dist < (treeRadius + carRadius)) {
            carSpeed = 0; // ütközés!
            return;
        }
    }

    // ------------- HEGYEK ütközése -------------
    // Ezek pontosan a rajzoláshoz használt helyek, méret kicsit kisebb, hogy ne legyen "láthatatlan fal"
    const float offX[4] = { -150.0f, -75.0f, 75.0f, 150.0f };
    const float rowZ[5] = { -500.0f, -250.0f, 0.0f, 250.0f, 500.0f };

    // Háttér-hegyek (világosabb, laposabb, randomizált)
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 5; ++j) {
            float hx = offX[i], hz = rowZ[j];
            float height = 40.0f + 12.0f * sinf((float)i*2 + (float)j*1.4f);
            float baseR  = height * (1.1f + 0.15f * cosf((float)j+i));
            // bounding radius legyen a baseR kb. 60%-a (hegy nem teljes kör)
            float hegyRadius = baseR * 1.0f;
            float dx = new_x - hx;
            float dz = new_z - hz;
            float dist = sqrtf(dx*dx + dz*dz);
            if (dist < (hegyRadius + carRadius)) {
                carSpeed = 0;
                return;
            }
        }
    }

    // Előtér-hegyek (sötétebb, magasabb)
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 5; ++j) {
            float hx = offX[i], hz = rowZ[j] + 150.0f;
            float height = 60.0f + 15.0f * ((i + j) & 1);
            float baseR  = height * 1.0f;
            float hegyRadius = baseR * 0.8f;
            float dx = new_x - hx;
            float dz = new_z - hz;
            float dist = sqrtf(dx*dx + dz*dz);
            if (dist < (hegyRadius + carRadius)) {
                carSpeed = 0;
                return;
            }
        }
    }

    // Ha nincs ütközés, mozgás engedélyezett
    car->x = new_x;
    car->z = new_z;
}


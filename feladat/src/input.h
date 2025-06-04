#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>

// Car struktúra
typedef struct Car {
    float x, y, z;
    float rotationY; // fokban
} Car;

// Külső világítás‐intenzitás és help‐állapot
extern float lightIntensity;
extern bool showHelp;

// A mozgás‐ és forgásfrissítés delta‐time‐mal
void handle_input(float deltaTime, Car* car);
void update_car(float deltaTime, Car* car);

#endif // INPUT_H

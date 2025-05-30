#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>

// Car struktúra
typedef struct Car {
    float x, y, z;
    float rotationY;
} Car;

// Külső fényerő változó
extern float lightIntensity;
extern bool showHelp;

void handle_input(bool* running, Car* car);
void update_car(Car* car);

#endif // INPUT_H

#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>

// Car struktúra — egyszer definiálva, így mindkét modul ugyanazt használja
typedef struct Car {
    float x, y, z;
    float rotationY;    // Y-tengely körüli forgás fokban
} Car;

// Bemeneti függvények prototípusai
void handle_input(bool* running, Car* car);
void update_car(Car* car);

#endif // INPUT_H

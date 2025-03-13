#include "gyak3_circle.h"
#include <math.h>

void set_circle_data(Circle* circle, double x, double y, double radius, double r, double g, double b)
{
    circle->x = x;
    circle->y = y;
    if (radius > 0.0) {
        circle->radius = radius;
    } else {
        circle->radius = NAN;
    }
    circle->r = r;
    circle->g = g;
    circle->b = b;
}

double calc_circle_area(const Circle* circle)
{
    return circle->radius * circle->radius * M_PI;
}

#ifndef CIRCLE_H
#define CIRCLE_H

typedef struct {
    double x, y;
    double radius;
    double r, g, b;  // Szín komponensek (RGB)
} Circle;

void set_circle_data(Circle* circle, double x, double y, double radius, double r, double g, double b);
double calc_circle_area(const Circle* circle);

#endif

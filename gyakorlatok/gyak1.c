#include <stdio.h>
#include <stdbool.h>

typedef struct {
    double length;
    double width;
    double height;
} Cuboid;

void set_size(Cuboid *c, double length, double width, double height) {
    if (length > 0 && width > 0 && height > 0) {
        c->length = length;
        c->width = width;
        c->height = height;
    } else {
        printf("Hiba: A méreteknek pozitívnak kell lenniük!\n");
    }
}

double calc_volume(Cuboid c) {
    return c.length * c.width * c.height;
}

double calc_surface(Cuboid c) {
    return 2 * (c.length * c.width + c.width * c.height + c.height * c.length);
}

bool has_square_face(Cuboid c) {
    return (c.length == c.width || c.width == c.height || c.height == c.length);
}

int main() {
    Cuboid myCuboid;
    set_size(&myCuboid, 3.0, 4.0, 3.0);
    
    printf("Térfogat: %.2f\n", calc_volume(myCuboid));
    printf("Felszín: %.2f\n", calc_surface(myCuboid));
    
    if (has_square_face(myCuboid)) {
        printf("A téglatestnek van négyzet alakú lapja.\n");
    } else {
        printf("A téglatestnek nincs négyzet alakú lapja.\n");
    }
    
    return 0;
}

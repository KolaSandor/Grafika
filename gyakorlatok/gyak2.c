#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#define SIZE 3

typedef struct {
    double length;
    double width;
    double height;
} Cuboid;

typedef struct {
    double matrix[SIZE][SIZE];
} Matrix;

typedef struct {
    double x;
    double y;
    double w;
} Point;

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

void init_identity_matrix(Matrix *m) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            m->matrix[i][j] = (i == j) ? 1.0 : 0.0;
        }
    }
}

void multiply_matrix_scalar(Matrix *m, double scalar) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            m->matrix[i][j] *= scalar;
        }
    }
}

void multiply_matrices(Matrix *result, Matrix *a, Matrix *b) {
    Matrix temp = {0};
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            for (int k = 0; k < SIZE; k++) {
                temp.matrix[i][j] += a->matrix[i][k] * b->matrix[k][j];
            }
        }
    }
    *result = temp;
}

Point transform_point(Matrix *m, Point p) {
    Point result;
    result.x = m->matrix[0][0] * p.x + m->matrix[0][1] * p.y + m->matrix[0][2] * p.w;
    result.y = m->matrix[1][0] * p.x + m->matrix[1][1] * p.y + m->matrix[1][2] * p.w;
    result.w = m->matrix[2][0] * p.x + m->matrix[2][1] * p.y + m->matrix[2][2] * p.w;
    return result;
}

void scale(Matrix *m, double sx, double sy) {
    init_identity_matrix(m);
    m->matrix[0][0] = sx;
    m->matrix[1][1] = sy;
}

void shift(Matrix *m, double dx, double dy) {
    init_identity_matrix(m);
    m->matrix[0][2] = dx;
    m->matrix[1][2] = dy;
}

void rotate(Matrix *m, double angle) {
    init_identity_matrix(m);
    double rad = angle * M_PI / 180.0;
    m->matrix[0][0] = cos(rad);
    m->matrix[0][1] = -sin(rad);
    m->matrix[1][0] = sin(rad);
    m->matrix[1][1] = cos(rad);
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
    
    Matrix identity, scaling, shifting, rotation;
    init_identity_matrix(&identity);
    scale(&scaling, 2.0, 3.0);
    shift(&shifting, 5.0, -3.0);
    rotate(&rotation, 45.0);
    
    Point p = {1.0, 1.0, 1.0};
    printf("Eredeti pont: (%.2f, %.2f)\n", p.x, p.y);
    p = transform_point(&scaling, p);
    printf("Skálázott pont: (%.2f, %.2f)\n", p.x, p.y);
    p = transform_point(&shifting, p);
    printf("Eltolt pont: (%.2f, %.2f)\n", p.x, p.y);
    p = transform_point(&rotation, p);
    printf("Forgatott pont: (%.2f, %.2f)\n", p.x, p.y);
    
    return 0;
}

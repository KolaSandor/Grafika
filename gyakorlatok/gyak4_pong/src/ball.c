// ball.c
#include "ball.h"
#include <GL/gl.h>
#include <math.h>

void init_ball(Ball* ball, float x, float y) {
    ball->x = x;
    ball->y = y;
    ball->radius = 50;
    ball->speed_x = 120;
    ball->speed_y = 120;
    ball->rotation = 0;
    ball->rotation_speed = 30;
}

void update_ball(Ball* ball, double time) {
    ball->x += ball->speed_x * time;
    ball->y += ball->speed_y * time;
    ball->rotation += ball->rotation_speed * time;
}

void render_ball(Ball* ball) {
    double angle;
    double x, y;
    
    glPushMatrix();
    glTranslatef(ball->x, ball->y, 0.0);
    glRotatef(ball->rotation, 0, 0, 1);
    
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(1.0, 0.9, 0.8);
    glVertex2f(0, 0);
    
    angle = 0;
    while (angle < 2.0 * M_PI) {
        x = cos(angle) * ball->radius;
        y = sin(angle) * ball->radius;
        glVertex2f(x, y);
        angle += 0.1;
    }
    
    glEnd();
    glPopMatrix();
}

void change_ball_size(Ball* ball, float delta) {
    ball->radius += delta;
    if (ball->radius < 10) ball->radius = 10;
    if (ball->radius > 100) ball->radius = 100;
}

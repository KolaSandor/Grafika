#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include "gyak3_circle.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define MAX_CIRCLE_COUNT 10
#define SEGMENTS 36  

Circle circles[MAX_CIRCLE_COUNT];
int circle_count = 0;
int selected_circle = -1;

void draw_circle(SDL_Renderer* renderer, Circle circle)
{
    SDL_SetRenderDrawColor(renderer, (int)(circle.r * 255), (int)(circle.g * 255), (int)(circle.b * 255), 255);
    double angle_step = 2.0 * M_PI / SEGMENTS;
    
    for (int i = 0; i < SEGMENTS; i++) {
        double angle1 = i * angle_step;
        double angle2 = (i + 1) * angle_step;

        int x1 = circle.x + cos(angle1) * circle.radius;
        int y1 = circle.y + sin(angle1) * circle.radius;
        int x2 = circle.x + cos(angle2) * circle.radius;
        int y2 = circle.y + sin(angle2) * circle.radius;

        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }
}

void handle_mouse_click(int x, int y)
{
    if (circle_count < MAX_CIRCLE_COUNT) {
        set_circle_data(&circles[circle_count], x, y, 40, (double)rand() / RAND_MAX, (double)rand() / RAND_MAX, (double)rand() / RAND_MAX);
        circle_count++;
    }
}

void handle_mouse_motion(int x, int y)
{
    for (int i = 0; i < circle_count; i++) {
        double dx = circles[i].x - x;
        double dy = circles[i].y - y;
        if (sqrt(dx * dx + dy * dy) < circles[i].radius) {
            selected_circle = i;
            return;
        }
    }
    selected_circle = -1;
}

void draw_cross(SDL_Renderer* renderer, Circle circle)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(renderer, circle.x - 5, circle.y, circle.x + 5, circle.y);
    SDL_RenderDrawLine(renderer, circle.x, circle.y - 5, circle.x, circle.y + 5);
}

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL nem sikerült indítani! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Kör rajzoló", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Nem sikerült létrehozni az ablakot! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Nem sikerült létrehozni a renderelőt! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    bool running = true;
    SDL_Event event;
    bool dragging = false;
    
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (selected_circle != -1) {
                    dragging = true;
                } else {
                    int x, y;
                    SDL_GetMouseState(&x, &y);
                    handle_mouse_click(x, y);
                }
            } else if (event.type == SDL_MOUSEBUTTONUP) {
                dragging = false;
            } else if (event.type == SDL_MOUSEMOTION) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                handle_mouse_motion(x, y);
                if (dragging && selected_circle != -1) {
                    circles[selected_circle].x = x;
                    circles[selected_circle].y = y;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (int i = 0; i < circle_count; i++) {
            draw_circle(renderer, circles[i]);
            if (i == selected_circle) {
                draw_cross(renderer, circles[i]);
            }
        }
        
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

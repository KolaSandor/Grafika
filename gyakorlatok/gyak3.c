#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#define SIZE 3
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define MAX_LINE_COUNT 100

typedef struct {
    double r, g, b;
} Color;

typedef struct {
    int x1, y1, x2, y2;
    Color color;
} Line;

typedef struct {
    int x, y, w;
} Point;

Line lines[MAX_LINE_COUNT];
int line_count = 0;
Color selected_color = {1.0, 1.0, 1.0};

void draw_line(SDL_Renderer *renderer, Line line) {
    SDL_SetRenderDrawColor(renderer, (int)(line.color.r * 255), (int)(line.color.g * 255), (int)(line.color.b * 255), 255);
    SDL_RenderDrawLine(renderer, line.x1, line.y1, line.x2, line.y2);
}

void draw_palette(SDL_Renderer *renderer) {
    Color colors[] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}, {1, 1, 0}, {1, 0, 1}, {0, 1, 1}};
    int num_colors = sizeof(colors) / sizeof(colors[0]);
    for (int i = 0; i < num_colors; i++) {
        SDL_SetRenderDrawColor(renderer, (int)(colors[i].r * 255), (int)(colors[i].g * 255), (int)(colors[i].b * 255), 255);
        SDL_Rect rect = {10 + i * 50, 10, 40, 40};
        SDL_RenderFillRect(renderer, &rect);
    }
}

void handle_mouse_click(int x, int y) {
    if (y < 50) {
        int color_index = (x - 10) / 50;
        Color colors[] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}, {1, 1, 0}, {1, 0, 1}, {0, 1, 1}};
        if (color_index >= 0 && color_index < 6) {
            selected_color = colors[color_index];
        }
    } else if (line_count < MAX_LINE_COUNT) {
        static bool first_click = true;
        static int start_x, start_y;
        if (first_click) {
            start_x = x;
            start_y = y;
        } else {
            lines[line_count++] = (Line){start_x, start_y, x, y, selected_color};
        }
        first_click = !first_click;
    }
}

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL nem sikerült indítani! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Szakasz rajzoló", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                printf("Egér pozíció: %d, %d\n", x, y);
                handle_mouse_click(x, y);
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        draw_palette(renderer);
        for (int i = 0; i < line_count; i++) {
            draw_line(renderer, lines[i]);
        }
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
// src/texture.c
#include "texture.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>

GLuint loadTextureSimple(const char* path) {
    SDL_Surface* surf = IMG_Load(path);
    if (!surf) {
        fprintf(stderr, "IMG_Load failed: %s\n", SDL_GetError());
        return 0;
    }
    GLenum mode = surf->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, mode, surf->w, surf->h, 0, mode, GL_UNSIGNED_BYTE, surf->pixels);
    SDL_FreeSurface(surf);
    return tex;
}

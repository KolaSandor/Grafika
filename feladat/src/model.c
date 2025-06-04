#include "model.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL_image.h>

// segéd: kép betöltése OpenGL textúrába
static GLuint loadTexture(const char* path) {
    SDL_Surface* surf = IMG_Load(path);
    if (!surf) { fprintf(stderr, "IMG_Load %s failed: %s\n", path, SDL_GetError()); return 0; }
    GLenum mode = surf->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, mode,
                 surf->w, surf->h, 0,
                 mode, GL_UNSIGNED_BYTE,
                 surf->pixels);
    SDL_FreeSurface(surf);
    return tex;
}

// segéd: elérési út építése
static char* joinPath(const char* dir, const char* file) {
    size_t n = strlen(dir), m = strlen(file);
    char* out = malloc(n + 1 + m + 1);
    strcpy(out, dir);
    if (dir[n-1] != '/' && dir[n-1] != '\\') strcat(out, "/");
    strcat(out, file);
    return out;
}

// csak az assets/models és assets/textures mappákat használjuk
static GLuint loadTextureFromModelsOrTextures(const char* modelsDir, const char* texFile) {
    // 1) próbáljuk meg az assets/models könyvtárban
    char* p1 = joinPath(modelsDir, texFile);
    GLuint tex = loadTexture(p1);
    free(p1);
    if (tex) return tex;

    // 2) ha nem sikerült, használjuk az sibling textures mappát
    // modelsDir például "assets/models", ebből parent="assets"
    char* parent = strdup(modelsDir);
    for (int i = strlen(parent)-1; i >= 0; --i) {
        if (parent[i] == '/' || parent[i] == '\\') { parent[i] = 0; break; }
    }
    char* texturesDir = joinPath(parent, "textures");
    free(parent);
    char* p2 = joinPath(texturesDir, texFile);
    tex = loadTexture(p2);
    free(p2);
    free(texturesDir);
    return tex;
}

Model* loadModel(const char* objPath) {
    FILE* f = fopen(objPath, "r");
    if (!f) { perror("fopen obj"); return NULL; }

    // objPath könyvtára
    char* dir = strdup(objPath);
    for (int i = strlen(dir)-1; i >= 0; --i) {
        if (dir[i]=='/' || dir[i]=='\\') { dir[i] = 0; break; }
    }

    char mtlName[256] = {0};
    float (*v)[3]   = malloc(65536 * sizeof *v);
    float (*vt)[2]  = malloc(65536 * sizeof *vt);
    float (*vn)[3]  = malloc(65536 * sizeof *vn);
    int vcnt=0, vtcnt=0, vncnt=0;
    int faceAlloc=4096, faceCnt=0;
    int (*fidx)[9]  = malloc(faceAlloc * sizeof *fidx);
    int *fmatl      = malloc(faceAlloc * sizeof *fmatl);
    int curMtl = -1;
    char line[512], texFile[256];

    while (fgets(line, sizeof line, f)) {
        if (!strncmp(line,"mtllib ",7)) {
            sscanf(line+7, "%255s", mtlName);
        }
        else if (!strncmp(line,"usemtl ",7)) {
            curMtl++;
            fmatl[faceCnt] = curMtl;
        }
        else if (line[0]=='v' && line[1]==' ') {
            sscanf(line+2, "%f %f %f",
                   &v[vcnt][0], &v[vcnt][1], &v[vcnt][2]); vcnt++;
        }
        else if (line[0]=='v' && line[1]=='t') {
            sscanf(line+3, "%f %f",
                   &vt[vtcnt][0], &vt[vtcnt][1]); vtcnt++;
        }
        else if (line[0]=='v' && line[1]=='n') {
            sscanf(line+3, "%f %f %f",
                   &vn[vncnt][0], &vn[vncnt][1], &vn[vncnt][2]); vncnt++;
        }
        else if (line[0]=='f' && line[1]==' ') {
            if (faceCnt >= faceAlloc) {
                faceAlloc *= 2;
                fidx  = realloc(fidx,  faceAlloc * sizeof *fidx);
                fmatl = realloc(fmatl, faceAlloc * sizeof *fmatl);
            }
            int i0,i1,i2, t0,t1,t2, n0,n1,n2;
            sscanf(line+2,
                   "%d/%d/%d %d/%d/%d %d/%d/%d",
                   &i0,&t0,&n0,
                   &i1,&t1,&n1,
                   &i2,&t2,&n2);
            fidx[faceCnt][0]=i0-1; fidx[faceCnt][1]=t0-1; fidx[faceCnt][2]=n0-1;
            fidx[faceCnt][3]=i1-1; fidx[faceCnt][4]=t1-1; fidx[faceCnt][5]=n1-1;
            fidx[faceCnt][6]=i2-1; fidx[faceCnt][7]=t2-1; fidx[faceCnt][8]=n2-1;
            faceCnt++;
        }
    }
    fclose(f);

    // MTL beolvasása
    char mtlPath[512];
    snprintf(mtlPath, sizeof mtlPath, "%s/%s", dir, mtlName);
    FILE* mf = fopen(mtlPath, "r");
    if (!mf) { perror("fopen mtl"); free(dir); return NULL; }

    Material* mtls = malloc(16 * sizeof(Material));
    int mtlCount = 0;
    char curName[128] = {0};

    while (fgets(line, sizeof line, mf)) {
        if (!strncmp(line,"newmtl ",7)) {
            sscanf(line+7, "%127s", curName);
            mtls[mtlCount].name  = strdup(curName);
            mtls[mtlCount].texID = 0;
        }
        else if (!strncmp(line,"map_Kd ",7)) {
            sscanf(line+7, "%255s", texFile);
            mtls[mtlCount].texID = loadTextureFromModelsOrTextures(dir, texFile);
            mtlCount++;
        }
    }
    fclose(mf);
    free(dir);

    Model* m = calloc(1, sizeof *m);
    m->numFaces     = faceCnt;
    m->numMaterials = mtlCount;
    m->mtls         = mtls;
    m->faceMtl      = malloc(faceCnt * sizeof(int));
    m->vertices     = malloc(faceCnt * 3 * 3 * sizeof(float));
    m->normals      = malloc(faceCnt * 3 * 3 * sizeof(float));
    m->texcoords    = malloc(faceCnt * 3 * 2 * sizeof(float));

    for (int fI = 0; fI < faceCnt; ++fI) {
        int matIdx = fmatl[fI] < mtlCount ? fmatl[fI] : 0;
        m->faceMtl[fI] = matIdx;
        for (int c = 0; c < 3; ++c) {
            int vi = fidx[fI][c*3+0],
                ti = fidx[fI][c*3+1],
                ni = fidx[fI][c*3+2];
            for (int k = 0; k < 3; ++k)
                m->vertices[(fI*3 + c)*3 + k] = v[vi][k],
                m->normals[(fI*3 + c)*3 + k]  = vn[ni][k];
            m->texcoords[(fI*3 + c)*2 + 0] = vt[ti][0];
            m->texcoords[(fI*3 + c)*2 + 1] = 1.0f - vt[ti][1];
        }
    }

    free(v); free(vt); free(vn);
    free(fidx); free(fmatl);
    return m;
}

// Kirajzolás anyagonként
void renderModel(const Model* m) {
    glEnable(GL_TEXTURE_2D);
    for (int i = 0; i < m->numMaterials; ++i) {
        glBindTexture(GL_TEXTURE_2D, m->mtls[i].texID);
        glBegin(GL_TRIANGLES);
        for (int fI = 0; fI < m->numFaces; ++fI) {
            if (m->faceMtl[fI] != i) continue;
            for (int c = 0; c < 3; ++c) {
                int idx = (fI*3 + c);
                glNormal3fv(&m->normals[idx*3]);
                glTexCoord2fv(&m->texcoords[idx*2]);
                glVertex3fv(&m->vertices[idx*3]);
            }
        }
        glEnd();
    }
    glDisable(GL_TEXTURE_2D);
}

void destroyModel(Model* m) {
    if (!m) return;
    for (int i = 0; i < m->numMaterials; ++i) {
        glDeleteTextures(1, &m->mtls[i].texID);
        free(m->mtls[i].name);
    }
    free(m->mtls);
    free(m->faceMtl);
    free(m->vertices);
    free(m->normals);
    free(m->texcoords);
    free(m);
}

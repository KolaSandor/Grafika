#ifndef MODEL_H
#define MODEL_H

#include <GL/gl.h>

// Egy anyagra vonatkozó adatok
typedef struct {
    char*   name;    // anyag neve (newmtl)
    GLuint  texID;   // OpenGL textúra azonosító
} Material;

// A teljes háromszög-alapú modell
typedef struct {
    float   *vertices;   // numFaces*3*3 float (x,y,z)
    float   *normals;    // numFaces*3*3 float (nx,ny,nz)
    float   *texcoords;  // numFaces*3*2 float (u,v)
    int     *faceMtl;    // numFaces int (anyag index)
    int      numFaces;
    Material *mtls;      // numMaterials hosszú tömb
    int      numMaterials;
} Model;

// Betölt OBJ + MTL + map_Kd textúrák alapján
Model* loadModel(const char* objPath);

// Kirajzolás anyagonként
void renderModel(const Model* m);

// Felszabadítás
void destroyModel(Model* m);

#endif // MODEL_H

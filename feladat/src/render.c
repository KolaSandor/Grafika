// src/render.c
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>          // sinf, cosf, fabsf
#include "render.h"
#include "input.h"
#define RAIN_COUNT 500

extern GLuint helpTex;    // csak deklaráció, definíció a main.c-ben

typedef struct {
    float x, y, z;
    float speed;
} Raindrop;

static Raindrop rain[RAIN_COUNT];

// ---- FÁK ----
static void draw_tree(float x, float z) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);

    // Törzs (henger)
    glColor3f(0.45f, 0.27f, 0.09f);
    static GLUquadric* cyl = NULL;
    if (!cyl) cyl = gluNewQuadric();
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    gluCylinder(cyl, 1.0f, 0.5f, 9.0f, 20, 15);
    glPopMatrix();

    // Lomb (gömb)
    glColor3f(0.1f + 0.1f * (sinf(z/20.0f)), 0.6f, 0.1f);
    static GLUquadric* sph = NULL;
    if (!sph) sph = gluNewQuadric();
    glPushMatrix();
    glTranslatef(0.0f, 15.0f, 0.0f);
    gluSphere(sph, 6.8f, 18, 12);
    glPopMatrix();

    glPopMatrix();
}

// ---- MADARAK ----
static void draw_bird(float x, float y, float z, float scale, float wingAngle) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(scale, scale, scale);
    glRotatef(wingAngle, 1, 0, 0);

    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_LINES);
    glVertex3f(0,0,0);  glVertex3f(-1.0f,0.4f,0);
    glVertex3f(0,0,0);  glVertex3f( 1.0f,0.4f,0);
    glEnd();
    glPopMatrix();
}

static void render_road() {
    glDisable(GL_LIGHTING);

    const float startZ      = -1000.0f;
    const float endZ        =  1000.0f;
    const float roadHalfW   =   10.0f;
    const float grassHalfW  =  300.0f;
    const float curbW       =    0.2f;
    const float lineWidth   =    0.1f;
    const float dashLen     =    4.0f;
    const float gapLen      =    4.0f;

    // 1) Fűpadka
    glColor3f(0.2f, 0.6f, 0.2f);
    glBegin(GL_QUADS);
      glVertex3f(-grassHalfW, -0.1f, startZ);
      glVertex3f(-roadHalfW,  -0.1f, startZ);
      glVertex3f(-roadHalfW,  -0.1f, endZ);
      glVertex3f(-grassHalfW, -0.1f, endZ);

      glVertex3f( roadHalfW,  -0.1f, startZ);
      glVertex3f( grassHalfW, -0.1f, startZ);
      glVertex3f( grassHalfW, -0.1f, endZ);
      glVertex3f( roadHalfW,  -0.1f, endZ);
    glEnd();

    // 2) Aszfalt
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_QUADS);
      glVertex3f(-roadHalfW, -0.1f, startZ);
      glVertex3f( roadHalfW, -0.1f, startZ);
      glVertex3f( roadHalfW, -0.1f, endZ);
      glVertex3f(-roadHalfW, -0.1f, endZ);
    glEnd();

    // 3) Padka
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
      glVertex3f(-roadHalfW - curbW, 0.01f, startZ);
      glVertex3f(-roadHalfW,         0.01f, startZ);
      glVertex3f(-roadHalfW,         0.01f, endZ);
      glVertex3f(-roadHalfW - curbW, 0.01f, endZ);

      glVertex3f( roadHalfW,         0.01f, startZ);
      glVertex3f( roadHalfW + curbW, 0.01f, startZ);
      glVertex3f( roadHalfW + curbW, 0.01f, endZ);
      glVertex3f( roadHalfW,         0.01f, endZ);
    glEnd();

    // 4) Középvonal
    {
      float z = startZ;
      glColor3f(1.0f, 1.0f, 1.0f);
      while (z < endZ) {
        glBegin(GL_QUADS);
          glVertex3f(-lineWidth, 0.02f, z);
          glVertex3f( lineWidth, 0.02f, z);
          glVertex3f( lineWidth, 0.02f, z + dashLen);
          glVertex3f(-lineWidth, 0.02f, z + dashLen);
        glEnd();
        z += dashLen + gapLen;
      }
    }

    glEnable(GL_LIGHTING);
}

void update_rain(const Camera* camera) {
    for (int i=0; i<RAIN_COUNT; ++i) {
        rain[i].y -= rain[i].speed;
        // Ha leért, újra random helyre (a kamera előtt) dobjuk:
        if (rain[i].y < 0) {
            // X: kamera x ± 8
            rain[i].x = camera->x - 8.0f + ((float)rand()/RAND_MAX)*16.0f;
            // Z: kamera z + 3 .. + 25 (mindig előtte legyen)
            rain[i].z = camera->z + 3.0f + ((float)rand()/RAND_MAX)*22.0f;
            // Magasan induljon
            rain[i].y = camera->y + 12.0f + ((float)rand()/RAND_MAX)*8.0f;
            rain[i].speed = 0.9f + ((float)rand()/RAND_MAX)*0.8f;
        }
    }
}

void draw_rain() {
    glDisable(GL_LIGHTING);
    glColor4f(1.0f,1.0f,1.0f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    for (int i=0; i<RAIN_COUNT; ++i) {
        glVertex3f(rain[i].x, rain[i].y, rain[i].z);
        glVertex3f(rain[i].x, rain[i].y - 3.5f, rain[i].z);
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

void init_rain() {
    for (int i = 0; i < RAIN_COUNT; ++i) {
        rain[i].x = -80.0f + ((float)rand()/RAND_MAX)*160.0f;
        rain[i].y = 65.0f + ((float)rand()/RAND_MAX)*40.0f;
        rain[i].z = -120.0f + ((float)rand()/RAND_MAX)*240.0f;
        rain[i].speed = 0.8f + ((float)rand()/RAND_MAX)*0.6f;
    }
}

void render_scene(const Camera* camera,
                  const Car*    car,
                  SDL_Window*   window)
{
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    glViewport(0, 0, w, h);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    // Először állítsunk be ortho proj-t ablakméret szerint:
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, w, 0, h, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Tisztán színes quad, tetején kék, alul fehéres
    glBegin(GL_QUADS);

      glColor3f(0.2f, 0.5f, 0.9f);
      glVertex2f(0.0f, (float)h);
      glVertex2f((float)w, (float)h);

      glColor3f(0.8f, 0.9f, 1.0f);
      glVertex2f((float)w, 0.0f);
      glVertex2f(0.0f, 0.0f);
    glEnd();

    // Visszaállítjuk a mátrixokat
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    // Visszakapcsoljuk a mélységet és a világítást
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    // Töröljük a mélységi puffer tartalmát, hogy a következő 3D rajz OK legyen:
    glClear(GL_DEPTH_BUFFER_BIT);

    // Köd és blend beállítások
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_FOG);
    {
      GLfloat fogColor[] = {0.7f, 0.7f, 0.9f, 1.0f};
      glFogfv(GL_FOG_COLOR, fogColor);
      glFogi(GL_FOG_MODE, GL_LINEAR);
      glFogf(GL_FOG_START, 120.0f);
      glFogf(GL_FOG_END,   220.0f);
    }

    // Világítás + depth teszt (hagyjuk hogy 3D objektumok világítsanak)
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    extern float lightIntensity; // input.c-ből, ha használod

    {
      GLfloat pos[]  = {0,10,10,1};
      GLfloat amb[]  = {0.25f * lightIntensity, 0.25f * lightIntensity, 0.22f * lightIntensity, 1.0f};
      GLfloat dif[]  = {0.8f * lightIntensity, 0.8f * lightIntensity, 0.75f * lightIntensity, 1.0f};
      GLfloat spec[] = {0.5f * lightIntensity, 0.5f * lightIntensity, 0.5f * lightIntensity, 1.0f};
      glLightfv(GL_LIGHT0, GL_POSITION, pos);
      glLightfv(GL_LIGHT0, GL_AMBIENT,  amb);
      glLightfv(GL_LIGHT0, GL_DIFFUSE,  dif);
      glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
    }

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    // kamera
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float)w/h, 0.1, 500.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
      camera->x, camera->y + 4.0f, camera->z + 9.0f,
      car->x,    car->y + 3.0f,    car->z - 20.0f,
      0, 1, 0
    );

    // --- FORGÓ NAP SUGARAKKAL ---
   {
    // 1) Nap koordinátái
    const float sunX = car->x;
    const float sunY = car->y + 40.0f;
    const float sunZ = car->z - 100.0f;

    // 2) Kamera koordinátái (csak XZ síkban szükséges)
    float camX = camera->x;
    float camZ = camera->z;

    // 3) Számítsuk ki a kamera irányát a naphoz viszonyítva XZ-síkban
    float dx = camX - sunX;
    float dz = camZ - sunZ;

    float baseAngleRad = atan2f(dx, dz);
    float baseAngleDeg = baseAngleRad * 180.0f / (float)M_PI;

    // 4) Forgó komponens hozzáadása (óra járásával megegyezően növelünk)
    static float sunSpin = 0.0f;
    sunSpin += 0.5f;                   // pl. 0.5 fok/frame vagy 0.5 fok/másodperc
    if (sunSpin >= 360.0f) sunSpin -= 360.0f;

    // Mivel CW (óra járásával megegyező) forgást akarunk,
    // ezért negatív előjellel adjuk hozzá a síkra.
    float finalAngle = baseAngleDeg + sunSpin;

    // 5) Rajzoljuk meg a napot és a sugarakat
    static GLUquadric* sunQuad = NULL;
    if (!sunQuad) sunQuad = gluNewQuadric();

    const float sunR   = 6.0f;   // nap sugara
    const float rayLen = 10.0f;  // sugarak hossza
    const int   rays   = 12;     // sugarak száma

    glDisable(GL_LIGHTING);
    glPushMatrix();
      // 5a) Tedd a napot a helyére
      glTranslatef(sunX, sunY, sunZ);

      // 5b) FORGASD AZ Y-TENGELY KÖRÜL, negatív előjellel, hogy CW legyen
      glRotatef(-finalAngle, 0.0f, 1.0f, 0.0f);

      // 5c) Nap kirajzolása (sárga gömb)
      glColor3f(1.0f, 1.0f, 0.0f);
      gluSphere(sunQuad, sunR, 16, 16);

      // 5d) Sugarak kirajzolása (alapból a +X irányból indul)
      glColor3f(1.0f, 0.9f, 0.0f);
      glLineWidth(2.0f);
      glBegin(GL_LINES);
        for (int i = 0; i < rays; ++i) {
          float a  = (2.0f * M_PI / rays) * i;
          float x0 = cosf(a) * sunR;
          float z0 = sinf(a) * sunR;
          float x1 = cosf(a) * (sunR + rayLen);
          float z1 = sinf(a) * (sunR + rayLen);
          glVertex3f(x0, 0.0f, z0);
          glVertex3f(x1, 0.0f, z1);
        }
      glEnd();
    glPopMatrix();
    glEnable(GL_LIGHTING);
}


    // --- HÁTTÉR-HEGYEK, SZÍN ÉS MAGASSÁG VÁLTOZÁSSAL ---
    {
        static GLUquadric* cone = NULL;
        if (!cone) cone = gluNewQuadric();

        const float roadW   = 10.0f;
        const float grassW  = 300.0f;
        const float offX[4] = { -150.0f, -75.0f, 75.0f, 150.0f };
        const float rowZ[5] = { -500.0f, -250.0f, 0.0f, 250.0f, 500.0f };

        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 5; ++j) {
                float x = offX[i], z = rowZ[j];
                if (fabsf(x) > roadW && fabsf(x) < grassW) {
                    float height = 40.0f + 12.0f * sinf((float)i*2 + (float)j*1.4f);
                    float baseR  = height * (1.1f + 0.15f * cosf((float)j+i));
                    glColor3f(0.6f + 0.1f*sinf(i+j), 0.5f + 0.1f*cosf(i), 0.5f + 0.1f*sinf(j));
                    glPushMatrix();
                      glTranslatef(x, -0.1f, z);
                      glRotatef(-90, 1, 0, 0);
                      gluCylinder(cone, baseR, 0.0f, height, 14, 3);
                    glPopMatrix();
                }
            }
        }

        glColor3f(0.5f, 0.3f, 0.3f);
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 5; ++j) {
                float x = offX[i], z = rowZ[j] + 150.0f;
                if (fabsf(x) > roadW && fabsf(x) < grassW) {
                    float height = 60.0f + 15.0f * ((i + j) & 1);
                    float baseR  = height * 0.8f;
                    glPushMatrix();
                      glTranslatef(x, -0.1f, z);
                      glRotatef(-90, 1, 0, 0);
                      gluCylinder(cone, baseR, 0.0f, height, 12, 3);
                    glPopMatrix();
                }
            }
        }
    }

    // --- FÁK AZ ÚT MENTÉN ---
    for (float z = -1000; z < 1000; z += 60.0f) {
        draw_tree(-16.0f, z);
        draw_tree( 16.0f, z);
    }

    // --- MADARAK AZ ÉGEN ---
    float t = SDL_GetTicks() * 0.001f;
    for (int i = 0; i < 3; ++i) {
        float bx = -80.0f + 80.0f*i + 30.0f*sinf(t+i);
        float by = 40.0f + 3.0f*cosf(t+i);
        float bz = car->z - 120.0f + 20.0f*i;
        float wing = 50.0f*sinf(t*2.0f + i);
        draw_bird(bx, by, bz, 4.0f, wing);
    }

    // út és autó kirajzolása
    render_road();
    if (carModel) {
      glPushMatrix();
        glTranslatef(car->x, car->y, car->z);
        glRotatef(car->rotationY, 0, 1, 0);
        renderModel(carModel);
      glPopMatrix();
    }

    float overlayAlpha = 1.0f - lightIntensity; // 1=teljesen sötét, 0=világos
    if (overlayAlpha > 0.01f) {
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0, 1, 0, 1, -1, 1);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);

        glColor4f(0, 0, 0, overlayAlpha);
        glBegin(GL_QUADS);
          glVertex2f(0, 0);
          glVertex2f(1, 0);
          glVertex2f(1, 1);
          glVertex2f(0, 1);
        glEnd();

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    }

    if (showHelp && helpTex) {
        // 2D overlay mód
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        int w, h; SDL_GetWindowSize(window, &w, &h);
        glOrtho(0, w, h, 0, -1, 1);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, helpTex);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glColor4f(1,1,1,1);
        glBegin(GL_QUADS);
          glTexCoord2f(0,0); glVertex2f(0,0);
          glTexCoord2f(1,0); glVertex2f(w,0);
          glTexCoord2f(1,1); glVertex2f(w,h);
          glTexCoord2f(0,1); glVertex2f(0,h);
        glEnd();

        glDisable(GL_BLEND);
        glDisable(GL_TEXTURE_2D);

        glPopMatrix();
        glMatrixMode(GL_PROJECTION); glPopMatrix();
        glMatrixMode(GL_MODELVIEW);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
    }

    update_rain(camera);
    draw_rain();

   
}

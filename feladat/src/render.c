#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>          // sinf, cosf, fabsf
#include "render.h"

// extern Model* carModel;  // render.h-ból

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
    glColor3f(0.1f,0.1f,0.1f);
    glBegin(GL_QUADS);
      glVertex3f(-roadHalfW,-0.1f,startZ);
      glVertex3f( roadHalfW,-0.1f,startZ);
      glVertex3f( roadHalfW,-0.1f,endZ);
      glVertex3f(-roadHalfW,-0.1f,endZ);
    glEnd();

    // 3) Padka
    glColor3f(1,1,1);
    glBegin(GL_QUADS);
      glVertex3f(-roadHalfW-curbW,0.01f,startZ);
      glVertex3f(-roadHalfW,      0.01f,startZ);
      glVertex3f(-roadHalfW,      0.01f,endZ);
      glVertex3f(-roadHalfW-curbW,0.01f,endZ);

      glVertex3f( roadHalfW,      0.01f,startZ);
      glVertex3f( roadHalfW+curbW,0.01f,startZ);
      glVertex3f( roadHalfW+curbW,0.01f,endZ);
      glVertex3f( roadHalfW,      0.01f,endZ);
    glEnd();

    // 4) Középvonal
    {
      float z = startZ;
      glColor3f(1,1,1);
      while(z<endZ){
        glBegin(GL_QUADS);
          glVertex3f(-lineWidth,0.02f,z);
          glVertex3f( lineWidth,0.02f,z);
          glVertex3f( lineWidth,0.02f,z+dashLen);
          glVertex3f(-lineWidth,0.02f,z+dashLen);
        glEnd();
        z+=dashLen+gapLen;
      }
    }

    glEnable(GL_LIGHTING);
}

void render_scene(const Camera* camera,
                  const Car*    car,
                  SDL_Window*   window)
{
    int w,h;
    SDL_GetWindowSize(window,&w,&h);
    glViewport(0,0,w,h);

    // világítás + depth
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    {
      GLfloat pos[]  = {0,10,10,1},
              amb[]  = {0.2f,0.2f,0.2f,1},
              dif[]  = {0.8f,0.8f,0.8f,1},
              spec[] = {1,1,1,1};
      glLightfv(GL_LIGHT0,GL_POSITION,pos);
      glLightfv(GL_LIGHT0,GL_AMBIENT, amb);
      glLightfv(GL_LIGHT0,GL_DIFFUSE, dif);
      glLightfv(GL_LIGHT0,GL_SPECULAR,spec);
    }
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // kamera
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0,(float)w/h,0.1,500.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
      camera->x, camera->y+4.0f, camera->z+9.0f,
      car->x,    car->y+3.0f,    car->z-20.0f,
      0,1,0
    );

    // forgó nap sugaraival
    {
        static float sunAngle = 0.0f;
        static GLUquadric* sunQuad = NULL;
        if(!sunQuad) sunQuad=gluNewQuadric();

        sunAngle+=0.5f;
        if(sunAngle>=360.0f) sunAngle-=360.0f;

        const float sunY   = car->y+80.0f;
        const float sunZ   = car->z-200.0f;
        const float sunR   = 15.0f;
        const float rayLen = 25.0f;
        const int   rays   = 12;

        glDisable(GL_LIGHTING);
        glPushMatrix();
          glTranslatef(car->x, sunY, sunZ);
          // forgatás Y helyett Z tengely körül
          glRotatef(sunAngle, 0.0f, 0.0f, 1.0f);

          // nap
          glColor3f(1,1,0);
          gluSphere(sunQuad, sunR,16,16);

          // sugarak X-Y sík helyett X-Z síkon
          glColor3f(1,0.9f,0);
          glLineWidth(2.0f);
          glBegin(GL_LINES);
          for(int i=0;i<rays;++i){
            float a = (2*M_PI/rays)*i;
            float x0 = cosf(a)*sunR;
            float z0 = sinf(a)*sunR;
            float x1 = cosf(a)*(sunR+rayLen);
            float z1 = sinf(a)*(sunR+rayLen);
            glVertex3f(x0, 0.0f, z0);
            glVertex3f(x1, 0.0f, z1);
          }
          glEnd();
        glPopMatrix();
        glEnable(GL_LIGHTING);
    }

    // hegyek a fűn
    glDisable(GL_LIGHTING);
    {
      const float roadW=10.0f, grassW=300.0f;
      const float offX[]={-200,-150,150,200};
      const float rowZ[]={-600,-300,0,300,600};

      glColor3f(0.6f,0.5f,0.5f);
      for(int i=0;i<4;i++)for(int j=0;j<5;j++){
        float x=offX[i], bz=rowZ[j];
        if(fabsf(x)>roadW&&fabsf(x)<grassW){
          float h0=40+10*((i+j)&1);
          glBegin(GL_TRIANGLES);
            glVertex3f(x-100,-0.1f,bz);
            glVertex3f(x,h0,bz+50);
            glVertex3f(x+100,-0.1f,bz);
          glEnd();
        }
      }

      glColor3f(0.4f,0.3f,0.3f);
      for(int i=0;i<4;i++)for(int j=0;j<5;j++){
        float x=offX[i], bz=rowZ[j];
        if(fabsf(x)>roadW&&fabsf(x)<grassW){
          float h0=40+10*((i+j)&1);
          glBegin(GL_TRIANGLES);
            glVertex3f(x-80,-0.1f,bz+20);
            glVertex3f(x,h0+10,bz+70);
            glVertex3f(x+80,-0.1f,bz+20);
          glEnd();
        }
      }
    }
    glEnable(GL_LIGHTING);

    // út és autó
    render_road();
    if(carModel){
      glPushMatrix();
        glTranslatef(car->x,car->y,car->z);
        glRotatef(car->rotationY,0,1,0);
        renderModel(carModel);
      glPopMatrix();
    }

    SDL_GL_SwapWindow(window);
}

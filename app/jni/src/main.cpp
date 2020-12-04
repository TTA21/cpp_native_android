//#include <SDL.h>
#include <android/log.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../SDL/src/core/android/SDL_android.h"
#include <vector>           ///Doesnt see it for some reason
#include <SDL.h>

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

const uint32_t WINDOW_WIDTH = 2560;
const uint32_t WINDOW_HEIGHT = 1550;

struct vec3d {float x, y, z;};
struct triangle {vec3d p[3];};
struct mesh {std::vector <triangle> tris;};
struct mat4x4 {float m[4][4] = {0};};

struct mesh meshCube;
struct mat4x4 matProj;

float fTheta;

void MultiplyMatrixVector(vec3d &i, vec3d &o, mat4x4 &m)
{
    o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
    o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
    o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
    float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

    if (w != 0.0f)
    {
        o.x /= w; o.y /= w; o.z /= w;
    }
}
void Create_Cube(){
    meshCube.tris = {

            // SOUTH
            { 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

            // EAST
            { 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
            { 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

            // NORTH
            { 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
            { 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

            // WEST
            { 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

            // TOP
            { 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
            { 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

            // BOTTOM
            { 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
            { 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },

    };

    // Projection Matrix
    float fNear = 0.1f;
    float fFar = 1000.0f;
    float fFov = 90.0f;
    float fAspectRatio = (float)WINDOW_HEIGHT / (float)WINDOW_WIDTH;
    float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

    matProj.m[0][0] = fAspectRatio * fFovRad;
    matProj.m[1][1] = fFovRad;
    matProj.m[2][2] = fFar / (fFar - fNear);
    matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
    matProj.m[2][3] = 1.0f;
    matProj.m[3][3] = 0.0f;

}

int normalize_coordinate_X(int x) { return (WINDOW_WIDTH / 2) + x; }

int normalize_coordinate_Y(int y) { return (WINDOW_HEIGHT / 2) + y; }

void SDL_RenderDrawTriangle(SDL_Renderer *ren   ,
                            int X1 , int Y1     ,
                            int X2 , int Y2     ,
                            int X3 , int Y3     ) {


    SDL_RenderDrawLine(ren, X1, Y1, X2, Y2);
    SDL_RenderDrawLine(ren, X2, Y2, X3, Y3);
    SDL_RenderDrawLine(ren, X3, Y3, X1, Y1);

}

int SDL_main(int argc, char **argv) {
    SDL_Window *win = 0;
    SDL_Renderer *ren = 0;

    SDL_bool done = SDL_FALSE;

    if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
        __android_log_print(ANDROID_LOG_VERBOSE, "SDL", "SDL_Init: %s\n", SDL_GetError());
        return 1;
    }
    ///Window size: 2560x1550 , Device size: 2560x1600
    win = SDL_CreateWindow("Hello World!", 0, 0, 2560, 1550,
                           SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN);
    if (win == 0) {
        __android_log_print(ANDROID_LOG_ERROR, "SDL", "SDL_CreateWindow: %s\n", SDL_GetError());
        return 1;
    }
    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren == 0) {
        __android_log_print(ANDROID_LOG_ERROR, "SDL", "SDL_CreateRenderer: %s\n", SDL_GetError());
        return 1;
    }

    Create_Cube();

    mat4x4 matRotZ, matRotX;

    while (!done) {

        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);

        // Set up rotation matrices

        fTheta += 0.01f;

        // Rotation Z
        matRotZ.m[0][0] = cosf(fTheta);
        matRotZ.m[0][1] = sinf(fTheta);
        matRotZ.m[1][0] = -sinf(fTheta);
        matRotZ.m[1][1] = cosf(fTheta);
        matRotZ.m[2][2] = 1;
        matRotZ.m[3][3] = 1;

        // Rotation X
        matRotX.m[0][0] = 1;
        matRotX.m[1][1] = cosf(fTheta * 0.5f);
        matRotX.m[1][2] = sinf(fTheta * 0.5f);
        matRotX.m[2][1] = -sinf(fTheta * 0.5f);
        matRotX.m[2][2] = cosf(fTheta * 0.5f);
        matRotX.m[3][3] = 1;


        for (auto tri : meshCube.tris)
        {

            triangle triProjected, triTranslated, triRotatedZ, triRotatedZX;

            // Rotate in Z-Axis
            MultiplyMatrixVector(tri.p[0], triRotatedZ.p[0], matRotZ);
            MultiplyMatrixVector(tri.p[1], triRotatedZ.p[1], matRotZ);
            MultiplyMatrixVector(tri.p[2], triRotatedZ.p[2], matRotZ);

            // Rotate in X-Axis
            MultiplyMatrixVector(triRotatedZ.p[0], triRotatedZX.p[0], matRotX);
            MultiplyMatrixVector(triRotatedZ.p[1], triRotatedZX.p[1], matRotX);
            MultiplyMatrixVector(triRotatedZ.p[2], triRotatedZX.p[2], matRotX);

            // Offset into the screen
            triTranslated = triRotatedZX;
            triTranslated.p[0].z = triRotatedZX.p[0].z + 3.0f;
            triTranslated.p[1].z = triRotatedZX.p[1].z + 3.0f;
            triTranslated.p[2].z = triRotatedZX.p[2].z + 3.0f;

            // Project triangles from 3D --> 2D
            MultiplyMatrixVector(triTranslated.p[0], triProjected.p[0], matProj);
            MultiplyMatrixVector(triTranslated.p[1], triProjected.p[1], matProj);
            MultiplyMatrixVector(triTranslated.p[2], triProjected.p[2], matProj);

            // Scale into view
            triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
            triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
            triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;
            triProjected.p[0].x *= 0.5f * (float)WINDOW_WIDTH;
            triProjected.p[0].y *= 0.5f * (float)WINDOW_HEIGHT;
            triProjected.p[1].x *= 0.5f * (float)WINDOW_WIDTH;
            triProjected.p[1].y *= 0.5f * (float)WINDOW_HEIGHT;
            triProjected.p[2].x *= 0.5f * (float)WINDOW_WIDTH;
            triProjected.p[2].y *= 0.5f * (float)WINDOW_HEIGHT;

            SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);

            SDL_RenderDrawTriangle( ren ,   triProjected.p[0].x, triProjected.p[0].y,
                                            triProjected.p[1].x, triProjected.p[1].y,
                                            triProjected.p[2].x, triProjected.p[2].y        );

        }

        SDL_RenderPresent(ren);

    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}


#ifdef __cplusplus
}
#endif
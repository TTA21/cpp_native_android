//#include <SDL.h>
#include <android/log.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../SDL/src/core/android/SDL_android.h"
#include <vector>           ///Doesnt see it for some reason
#include <SDL.h>
#include "data_structures.h"
#include "lin_alg.h"
#include <algorithm>

#ifdef __cplusplus
extern "C" {
#endif

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

    meshCube.file_readLine( "teapot.obj" );
    __android_log_print(ANDROID_LOG_DEBUG, "MESH", "MESH ALERT: %s\n", "Finished getting mesh");




    mat4x4 matRotZ, matRotX , matRotY;
    float accelValues[3];

    while (!done) {

        Android_JNI_GetAccelerometerValues(accelValues);

        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);

        Rotate_Z( matRotZ , accelValues[0] );
        Rotate_X( matRotX , accelValues[1] + 90 );
        Rotate_Y( matRotY , accelValues[2] + 90 );

        std::vector<triangle> vecTrianglesToRaster;

        for (auto tri : meshCube.tris)
        {

            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT || event.type == SDL_KEYDOWN ||
                    event.type == SDL_FINGERDOWN || event.type == SDL_FINGERMOTION ) {

                    int int_accel[3];
                    int_accel[0] = accelValues[0]*1000;
                    int_accel[1] = accelValues[1]*1000;
                    int_accel[2] = accelValues[2]*1000;
                    SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);
                    SDL_RenderDrawLine( ren , 750 , 750 , int_accel[0]+750 , 750 );
                    SDL_SetRenderDrawColor(ren, 0, 0, 255, 255);
                    SDL_RenderDrawLine( ren , 750 , 750 , 750 , int_accel[1]+750 );
                    SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
                    SDL_RenderDrawLine( ren , 750 , 750 , int_accel[2] + 750 , int_accel[2] + 750 );

                }
            }

            triangle triProjected, triTranslated;

            Apply_Transformation( tri , tri , matRotZ );    ///Rotate on z
            Apply_Transformation( tri , tri , matRotX );    ///Rotate on x
            Apply_Transformation( tri , tri , matRotY );    ///Rotate on y , sensor failure

            // Offset into the screen
            triTranslated = tri;
            triTranslated.p[0].z = tri.p[0].z + 5.0f;
            triTranslated.p[1].z = tri.p[1].z + 5.0f;
            triTranslated.p[2].z = tri.p[2].z + 5.0f;

            // Use Cross-Product to get surface normal
            vec3d normal;
            Cross_Normal( normal , triTranslated );

            // It's normally normal to normalise the normal
            float l = sqrtf(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
            normal.x /= l; normal.y /= l; normal.z /= l;

            if(Cross_View( normal , triTranslated.p[0] ) < 0.0f) {

                // Project triangles from 3D --> 2D
                Apply_Transformation( triTranslated , triProjected , matProj ); ///ERR

                // Scale into view
                triProjected.p[0].x += 1.0f;
                triProjected.p[0].y += 1.0f;
                triProjected.p[1].x += 1.0f;
                triProjected.p[1].y += 1.0f;
                triProjected.p[2].x += 1.0f;
                triProjected.p[2].y += 1.0f;
                triProjected.p[0].x *= 0.5f * (float) WINDOW_WIDTH;
                triProjected.p[0].y *= 0.5f * (float) WINDOW_HEIGHT;
                triProjected.p[1].x *= 0.5f * (float) WINDOW_WIDTH;
                triProjected.p[1].y *= 0.5f * (float) WINDOW_HEIGHT;
                triProjected.p[2].x *= 0.5f * (float) WINDOW_WIDTH;
                triProjected.p[2].y *= 0.5f * (float) WINDOW_HEIGHT;

                //SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);

                //SDL_RenderDrawTriangle(ren, triProjected.p[0].x, triProjected.p[0].y,
                //                       triProjected.p[1].x, triProjected.p[1].y,
                //                       triProjected.p[2].x, triProjected.p[2].y);

                vecTrianglesToRaster.push_back(triProjected);

            }

        }

        // Sort triangles from back to front
        sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](triangle &t1, triangle &t2)
        {
            float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
            float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
            return z1 > z2;
        });

        for (auto &triProjected : vecTrianglesToRaster)
        {
            SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);

            SDL_RenderDrawTriangle(ren, triProjected.p[0].x, triProjected.p[0].y,
                                   triProjected.p[1].x, triProjected.p[1].y,
                                   triProjected.p[2].x, triProjected.p[2].y);
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
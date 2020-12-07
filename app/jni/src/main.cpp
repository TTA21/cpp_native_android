//#include <SDL.h>

///Thank god for OLC[javidx9] for teaching me 3D projection!  https://www.youtube.com/channel/UC-yuWVUplUJZvieEligKBkA
///                                                           https://github.com/OneLoneCoder

#include <android/log.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../SDL/src/core/android/SDL_android.h"
#include <vector>           ///Doesnt see it for some reason
#include <SDL.h>
#include "data_structures.h"
#include "lin_alg.h"
#include <algorithm>
#include <string>

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

    Create_Projection_matrix();
    meshCube.load_obj( "teapot.obj" );

    mat4x4 matRotZ, matRotX , matRotY;
    float accelValues[3];

    long finger_coord_X = 0 , finger_coord_Y = 0;
    float finger_rotation_X = 1 , finger_rotation_Y = 1 , finger_rotation_Z = 1 , finger_distance = 5;    ///Min distance is 5

    while (!done) {

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if ( event.type == SDL_FINGERDOWN || event.type == SDL_FINGERMOTION ) {

                if( event.tfinger.x * WINDOW_WIDTH > finger_coord_X ){  ///Going right
                    finger_rotation_Y += 0.02;
                }else{                                                  ///Going left
                    finger_rotation_Y -= 0.02;
                }

                if( event.tfinger.y * WINDOW_HEIGHT > finger_coord_Y ){  ///Going right
                    finger_rotation_X -= 0.02;
                }else{                                                  ///Going left
                    finger_rotation_X += 0.02;
                }

                finger_coord_X = event.tfinger.x * WINDOW_WIDTH;
                finger_coord_Y = event.tfinger.y * WINDOW_HEIGHT;

            }else if( event.type == SDL_MULTIGESTURE ){

                finger_rotation_Z += event.mgesture.dTheta * 2;

                finger_distance -= event.mgesture.dDist * 100;

                if( finger_distance < 5 ){
                    finger_distance = 5;
                }

            }
        }


        Android_JNI_GetAccelerometerValues(accelValues);

        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);

        Rotate_Z( matRotZ , accelValues[0] );
        Rotate_X( matRotX , accelValues[1] + 90 );
        Rotate_Y( matRotY , accelValues[2] + 90 );

        Rotate_Z( matRotZ , finger_rotation_Z + 90 );
        Rotate_X( matRotX , finger_rotation_X + 90 );
        Rotate_Y( matRotY , finger_rotation_Y + 90 );

        std::vector<triangle> vecTrianglesToRaster;

        for (auto tri : meshCube.tris)
        {

            triangle triProjected, triTranslated;

            Apply_Transformation( tri , tri , matRotZ );    ///Rotate on z
            Apply_Transformation( tri , tri , matRotX );    ///Rotate on x
            Apply_Transformation( tri , tri , matRotY );    ///Rotate on y , sensor failure

            // Offset into the screen
            triTranslated = tri;
            triTranslated.p[0].z = tri.p[0].z + finger_distance;
            triTranslated.p[1].z = tri.p[1].z + finger_distance;
            triTranslated.p[2].z = tri.p[2].z + finger_distance;

            // Use Cross-Product to get surface normal
            vec3d normal;
            Cross_Normal( normal , triTranslated );

            // It's normally normal to normalise the normal
            float l = sqrtf(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
            normal.x /= l; normal.y /= l; normal.z /= l;

            if(Cross_View( normal , triTranslated.p[0] ) < 0.0f) {

                // Illumination
                vec3d light_direction = { 0.0f, 0.0f, -1.0f };
                triProjected.color = Find_Brightness( light_direction , normal );

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

            SDL_SetRenderDrawColor(ren, 0, 255 * triProjected.color , 0, 255);
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
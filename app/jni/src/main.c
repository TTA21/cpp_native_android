//
// Created by XposeMedia on 18/05/2017.
//
#include <SDL.h>
#include <android/log.h>
#include <stdlib.h>
#include <stdbool.h>

SDL_Texture* CriarImagem(const char* Source, SDL_Renderer * Render){

    SDL_Surface* img = SDL_LoadBMP(Source);
    SDL_Texture *imagem = SDL_CreateTextureFromSurface(Render,img);
    SDL_FreeSurface(img);

    return imagem;
}

const uint32_t WINDOW_WIDTH = 2560;
const uint32_t WINDOW_HEIGHT = 1550;

int normalize_coordinate_X( int x ){    return ( WINDOW_WIDTH/2 ) + x;  }

int normalize_coordinate_Y( int y ){    return ( WINDOW_HEIGHT/2 ) + y; }

void SDL_RenderDrawTriangle( SDL_Renderer *ren  ,
                             uint32_t point1[2] ,
                             uint32_t point2[2] ,
                             uint32_t point3[2]){

    SDL_RenderDrawLine( ren , point1[0] , point1[1] , point2[0] , point2[1] );
    SDL_RenderDrawLine( ren , point2[0] , point2[1] , point3[0] , point3[1] );
    SDL_RenderDrawLine( ren , point3[0] , point3[1] , point1[0] , point1[1] );

}

int SDL_main(int argc, char **argv)
{
    SDL_Window *win = 0;
    SDL_Renderer *ren = 0;

    SDL_bool done = SDL_FALSE;

    if (SDL_Init(SDL_INIT_EVERYTHING) == -1){
        __android_log_print(ANDROID_LOG_VERBOSE, "SDL", "SDL_Init: %s\n", SDL_GetError());
        return 1;
    }
    ///Window size: 2560x1550 , Device size: 2560x1600
    win = SDL_CreateWindow("Hello World!", 0, 0, 2560, 1550, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN);
    if (win == 0){
        __android_log_print(ANDROID_LOG_ERROR, "SDL", "SDL_CreateWindow: %s\n", SDL_GetError());
        return 1;
    }
    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren == 0){
        __android_log_print(ANDROID_LOG_ERROR, "SDL", "SDL_CreateRenderer: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Texture *tex;
    tex = CriarImagem( "character.bmp" , ren );
    //C:\Users\USER\AndroidStudioProjects\android-project\app\src\main\assets\yourimage.bmp
    if( tex == NULL ) __android_log_print(ANDROID_LOG_INFO, "SDL", "SDL_CreateImage: %s\n", SDL_GetError());
    else __android_log_print(ANDROID_LOG_INFO, "SDL", "SDL_Success: %s\n", "Image created succesfully");


    //__android_log_print(ANDROID_LOG_INFO, "SDL", "SDL_Success: %s\n", SDL_GetError());


    while (!done) {

        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);

        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT || event.type == SDL_KEYDOWN || event.type == SDL_FINGERDOWN) {
                SDL_RenderCopy( ren , tex , NULL , NULL );
            }
        }

        int point1[2];
        point1[0] = normalize_coordinate_X( rand() % 2560 );
        point1[1] = normalize_coordinate_Y( rand() % 1550 );
        int point2[2];
        point2[0] = normalize_coordinate_X( rand() % 2560 );
        point2[1] = normalize_coordinate_Y( rand() % 1550 );
        int point3[2];
        point3[0] = normalize_coordinate_X( rand() % 2560 );;
        point3[1] = normalize_coordinate_Y( rand() % 1550 );


        ///Stuff
        SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);
        SDL_RenderDrawTriangle( ren , point1 , point2 , point3 );

        SDL_RenderPresent(ren);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
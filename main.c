//Using SDL and standard IO
#include <SDL2/SDL.h>
#include <stdio.h>
#include <windows.h>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

BOOL quit = FALSE;

SDL_Surface* screenSurface = NULL;

SDL_Surface* image1;
SDL_Surface* image2;

void HandleEvents(SDL_Event* e){
    switch(e->type){
                case SDL_QUIT:
                    quit = TRUE;
                    break;

                case SDL_KEYDOWN:
                    switch(e->key.keysym.sym){
                        case SDLK_1:
                        SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0xFF, 0xFF, 0xFF ) );
                        SDL_BlitSurface(image1, NULL, screenSurface, NULL);
                        break;

                        case SDLK_2:
                        SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0xFF, 0xFF, 0xFF ) );
                        SDL_BlitSurface(image2, NULL, screenSurface, NULL);
                        break;
                    }
                break;
            }
}

BOOL LoadMedia(SDL_Surface** container, const char * path){
    *container = SDL_LoadBMP(path);
    if (*container == NULL){
        printf( "Unable to load image %s! SDL Error: %s\n", path, SDL_GetError() );
        return FALSE;
    }

    return TRUE;
}

int main( int argc, char* args[] )
{
    //The window we'll be rendering to
    SDL_Window* window = NULL;
    
    //The surface contained by the window
    

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );

        SDL_Quit();
        return -1;
    }

    //Create window
    window = SDL_CreateWindow( "EliasMog", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if( window == NULL )
    {
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );

        SDL_Quit();
        return -1;
    }

    //Get window surface
    screenSurface = SDL_GetWindowSurface( window );

    
    LoadMedia(&image1, "Assets/eliasmog.bmp");
    LoadMedia(&image2, "Assets/hihi.bmp");

    //Fill the surface white
    
    //Update the surface
    

    //Hack to get window to stay up
    SDL_Event e; 
    
    while( quit == FALSE ){ 

        while( SDL_PollEvent( &e ) != 0 ) HandleEvents(&e);

        SDL_UpdateWindowSurface( window );
    }    
    
      //Destroy window
    SDL_DestroyWindow( window );
    SDL_FreeSurface(image1);
    SDL_FreeSurface(image2);

    //Quit SDL subsystems
    SDL_Quit();

    return 0;
}
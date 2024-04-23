#define WindowName "Chip8Emu"

//Using SDL and standard IO
#include <windows.h>

#include <GL/glew.h>
#include <cglm/cglm.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <stdio.h>
#include <math.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_SDL_GL3_IMPLEMENTATION
#include <nuklear.h>
#include <nuklear_sdl_gl3.h>

#include "headers/input.h"
#include "headers/texture.h"
#include "headers/fileReader.h"
#include "headers/shader.h"
#include "headers/frame.h"
#include "headers/chip8.h"

#define MAX_VERTEX_MEMORY 512 * 1024
#define MAX_ELEMENT_MEMORY 128 * 1024

//Screen dimension constants
const float SCREEN_WIDTH = 1200.0f;
const float SCREEN_HEIGHT = 800.0f;

SDL_Window* window = NULL;
int win_width, win_height;

vec2 oldMouse;

bool debugging = false;
nk_bool quit = FALSE;

SDL_Surface* screenSurface = NULL;

void GetEvents(SDL_Event* e){
    switch(e->type){
                case SDL_QUIT:
                    quit = TRUE;
                    break;

                case SDL_KEYDOWN:
                    switch(e->key.keysym.sym){

                        case SDLK_x:
                            emu.input[0x0] = 1;
                        break;

                        case SDLK_1:
                            emu.input[0x1] = 1;
                        break;

                        case SDLK_2:
                            emu.input[0x2] = 1;
                        break;

                        case SDLK_3:
                            emu.input[0x3] = 1;
                        break;

                        case SDLK_q:
                            emu.input[0x4] = 1;
                        break;

                        case SDLK_w:
                            emu.input[0x5] = 1;
                        break;

                        case SDLK_e:
                            emu.input[0x6] = 1;
                        break;

                        case SDLK_a:
                            emu.input[0x7] = 1;
                        break;

                        case SDLK_s:
                            emu.input[0x8] = 1;
                        break;

                        case SDLK_d:
                            emu.input[0x9] = 1;
                        break;

                        case SDLK_z:
                            emu.input[0xA] = 1;
                        break;

                        case SDLK_c:
                            emu.input[0xB] = 1;
                        break;

                        case SDLK_4:
                            emu.input[0xC] = 1;
                        break;

                        case SDLK_r:
                            emu.input[0xD] = 1;
                        break;

                        case SDLK_f:
                            emu.input[0xE] = 1;
                        break;

                        case SDLK_v:
                            emu.input[0xF] = 1;
                        break;

                        case SDLK_KP_ENTER:
                            if(debugging & emu.active) Fetch();
                        break;
                    }
                break;

                case SDL_KEYUP:
                    switch(e->key.keysym.sym){

                        case SDLK_x:
                            emu.input[0x0] = 0;
                        break;

                        case SDLK_1:
                            emu.input[0x1] = 0;
                        break;

                        case SDLK_2:
                            emu.input[0x2] = 0;
                        break;

                        case SDLK_3:
                            emu.input[0x3] = 0;
                        break;

                        case SDLK_q:
                            emu.input[0x4] = 0;
                        break;

                        case SDLK_w:
                            emu.input[0x5] = 0;
                        break;

                        case SDLK_e:
                            emu.input[0x6] = 0;
                        break;

                        case SDLK_a:
                            emu.input[0x7] = 0;
                        break;

                        case SDLK_s:
                            emu.input[0x8] = 0;
                        break;

                        case SDLK_d:
                            emu.input[0x9] = 0;
                        break;

                        case SDLK_z:
                            emu.input[0xA] = 0;
                        break;

                        case SDLK_c:
                            emu.input[0xB] = 0;
                        break;

                        case SDLK_4:
                            emu.input[0xC] = 0;
                        break;

                        case SDLK_r:
                            emu.input[0xD] = 0;
                        break;

                        case SDLK_f:
                            emu.input[0xE] = 0;
                        break;

                        case SDLK_v:
                            emu.input[0xF] = 0;
                        break;
                    }
                break;

                case SDL_MOUSEBUTTONDOWN:
                    switch(e->button.which){
                        case 0:
                        {
                            TryToGrab(window);
                        }
                        break;
                    }
                break;

                case SDL_MOUSEBUTTONUP:
                Release();
                break;
            }
}

int main( int argc, char* args[] )
{
    //The window we'll be rendering to
    SDL_GLContext glContext;
    
    struct nk_context *ctx;
    struct nk_colorf bg;

    //Initialize SDL
    if( SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_EVENTS) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );

        SDL_Quit();
        return -1;
    }
    SDL_GL_SetAttribute (SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute (SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    //Create window
    window = SDL_CreateWindow( "Emu8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_ALLOW_HIGHDPI|SDL_WINDOW_UTILITY|SDL_WINDOW_RESIZABLE  );
    if( window == NULL )
    {
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );

        SDL_Quit();
        return -1;
    }

    glContext = SDL_GL_CreateContext(window);
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glewExperimental = 1;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to setup GLEW\n");
        return -1;
    }

    ctx = nk_sdl_init(window);
    /* Load Fonts: if none of these are loaded a default font will be used  */
    /* Load Cursor: if you uncomment cursor loading please hide the cursor */
    struct nk_font_atlas *atlas;
    nk_sdl_font_stash_begin(&atlas);
    nk_sdl_font_stash_end();

    //Get window surface
    screenSurface = SDL_GetWindowSurface( window );

    CreateFrame(640, 320);

    //Fill the surface white
    
    //Update the surface
    //InitChip8(window);

    //Hack to get window to stay up
    float lastTime;

    SDL_Event e; 
    bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;
    while( quit == FALSE ){ 
        uint64_t deltaTime = SDL_GetTicks64() - lastTime;
        lastTime = SDL_GetTicks64();
        int currentMouse[2];
        SDL_GetMouseState(&currentMouse[0], &currentMouse[1]);
        nk_input_begin(ctx);

        int index = 0;
        while( SDL_PollEvent( &e ) != 0 ) {
            GetEvents(&e);
            nk_sdl_handle_event(&e);
            SaveEvents(&e, index);
            index++;
            }

        nk_sdl_handle_grab(); /* optional grabbing behavior */
        nk_input_end(ctx);

        if (grabbing) {
            framePos[0] += currentMouse[0] - oldMouse[0];
            framePos[1] -= currentMouse[1] - oldMouse[1];
        }
        /* GUI */
        if (nk_begin(ctx, "Demo", nk_rect(50, 50, 230, 250),
            NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
            NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
        {   
            char time[10];
            gcvt(deltaTime, 10, time);
            nk_layout_row_dynamic(ctx, 30, 1);
            nk_label(ctx, time, NK_TEXT_LEFT);
            nk_layout_row_static(ctx, 30, 80, 1);
            if (nk_button_label(ctx, "Open Rom")){
                InitChip8(window);
            }
            nk_layout_row_dynamic(ctx, 30, 1);
            nk_checkbox_label(ctx, "Debugging", (nk_bool*)&debugging);
            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, "background:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            if (nk_combo_begin_color(ctx, nk_rgb_cf(bg), nk_vec2(nk_widget_width(ctx),400))) {
                nk_layout_row_dynamic(ctx, 120, 1);
                bg = nk_color_picker(ctx, bg, NK_RGBA);
                nk_layout_row_dynamic(ctx, 25, 1);
                bg.r = nk_propertyf(ctx, "#R:", 0, bg.r, 1.0f, 0.01f,0.005f);
                bg.g = nk_propertyf(ctx, "#G:", 0, bg.g, 1.0f, 0.01f,0.005f);
                bg.b = nk_propertyf(ctx, "#B:", 0, bg.b, 1.0f, 0.01f,0.005f);
                bg.a = nk_propertyf(ctx, "#A:", 0, bg.a, 1.0f, 0.01f,0.005f);
                nk_combo_end(ctx);
            }
        }
        nk_end(ctx);

        /* Draw */
        SDL_GetWindowSize(window, &win_width, &win_height);
        glViewport(0, 0, win_width, win_height);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(bg.r, bg.g, bg.b, bg.a);

        
        /* IMPORTANT: `nk_sdl_render` modifies some global OpenGL state
         * with blending, scissor, face culling, depth test and viewport and
         * defaults everything back into a default state.
         * Make sure to either a.) save and restore or b.) reset your own state after
         * rendering the UI. */
        int cyclesPerFrame = deltaTime;
        if (emu.active && !debugging){
            for (int c = 0; c < cyclesPerFrame; c++){
                Fetch();
            }

            SetFrame();
            ReBindFrame();
        }
            
        if (emu.active){
            if(emu.soundTimer > 0) emu.soundTimer -= 60 * deltaTime * 0.001;
            if(emu.delayTimer > 0) emu.delayTimer -= 60 * deltaTime * 0.001;
        }

        DrawFrame(window);
        nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);
        EndInput();

        SDL_GL_SwapWindow(window);

        oldMouse[0] = currentMouse[0];
        oldMouse[1] = currentMouse[1];
    }    
    
      //Destroy window
    SDL_DestroyWindow( window );
    DeleteFrame();
    nk_font_atlas_clear(atlas);
    nk_free(ctx);
    //Quit SDL subsystems
    SDL_Quit();

    return 0;
}
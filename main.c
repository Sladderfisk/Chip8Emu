#define WindowName "Chip8Emu"

//Using SDL and standard IO
#include <windows.h>

#include <GL/glew.h>
#include <cglm/cglm.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_syswm.h>

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

#include "headers/texture.h"
#include "headers/shader.h"

#define MAX_VERTEX_MEMORY 512 * 1024
#define MAX_ELEMENT_MEMORY 128 * 1024

//Screen dimension constants
const float SCREEN_WIDTH = 1200.0f;
const float SCREEN_HEIGHT = 800.0f;

BOOL grabbing = FALSE;
vec2 oldMouse;
vec2 pos;

BOOL quit = FALSE;

SDL_Surface* screenSurface = NULL;

SDL_Surface* image1;
SDL_Surface* image2;

texture tex = {0};
shader sh = {0};
unsigned int VBO, VAO, EBO;

void RegenImage(float time){
    for (int x = 0; x < 400; x++){
        float V = x / 50.0f;
        int y = sin(V) * 100 + 200;
        pixel pix = {255, 255, 255};
        SetTexPixel(&tex, pix, x, y);
        SetTexPixel(&tex, pix, x + 1, y + 1);
        SetTexPixel(&tex, pix, x, y + 1);
        SetTexPixel(&tex, pix, x + 1, y);
        SetTexPixel(&tex, pix, x - 1, y);
        SetTexPixel(&tex, pix, x - 1, y - 1);
        SetTexPixel(&tex, pix, x, y - 1);
    }

    ReBindTex(&tex, &sh);
}

void GenTexture(){
    
    pixel image[400*400];

    for(int y = 0; y < 400; y++){
        for (int x = 0; x < 400; x++){
            image[(y * 400) + x][0] = (byte)glm_lerp(0.0f, 255.0f, y / 400.0f);
            image[(y * 400) + x][1] = (byte)glm_lerp(0.0f, 255.0f, x / 400.0f);
            image[(y * 400) + x][2] = (byte)glm_lerp(0.0f, 255.0f, (400 - x) / 400.0f);
        }
    }

    CreateTexture(&tex, 400, 400, image);
    ReBindTex(&tex, &sh);
}

void InitShaderStuff(){
    CompileShader(&sh, "shaders/sh.vert", "shaders/sh.frag");
    float vertices[] = {
        -200.0f, 200.0f, 0.0f,  0.0f, 1.0f,
        200.0f, 200.0f, 0.0f,   1.0f, 1.0f,
        -200.0f, -200.0f, 0.0f, 0.0f, 0.0f,
        200.0f, -200.0f, 0.0f, 1.0f, 0.0f
    };
    unsigned int indices[] = {
        0, 1, 2,
        1, 3, 2
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void Draw(){

    RegenImage(0);
    DrawTex(&tex);

    glUseProgram(sh.ID);
    mat4 proj = {0};
    glm_ortho(-SCREEN_WIDTH / 2, SCREEN_WIDTH / 2, -SCREEN_HEIGHT / 2, SCREEN_HEIGHT / 2, -1000.0f, 1000.0f, proj); 

    mat4 view = {0};
    vec3 eye = {0}; eye[0] = 0.0f; eye[1] = 0.0f; eye[2] = 1.0f;
    vec3 center = {0}; center[0] = 0.0f; center[1] = 0.0f; center[2] = 0.0f;
    vec3 up = {0}; up[0] = 0.0f; up[1] = 1.0f; up[2] = 0.0f;
    glm_lookat(eye, center, up, view);
    
    mat4 tra = {0};
    glm_mat4_identity(tra);
    float newPos[] = {pos[0], pos[1], 0.0f};
    glm_translate(tra, newPos);

    glm_mat4_identity(view);

    glUniformMatrix4fv(glGetUniformLocation(sh.ID, "model"), 1, GL_FALSE, &tra[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(sh.ID, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(sh.ID, "projection"), 1, GL_FALSE, &proj[0][0]);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

char* OpenExplorer(SDL_Window* win, char* pathBuffer){
    OPENFILENAME ofn;

    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(win, &wmInfo);
    HWND hwnd = wmInfo.info.win.window;

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = pathBuffer;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(pathBuffer);
    ofn.lpstrFilter = "ROM Files (*.)\0*.\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn)==FALSE) printf("File explorer dialogue could not open for some reason!\n");
    return pathBuffer;
}

void HandleEvents(SDL_Event* e){
    switch(e->type){
                case SDL_QUIT:
                    quit = TRUE;
                    break;

                case SDL_KEYDOWN:
                    switch(e->key.keysym.sym){
                    }
                break;

                case SDL_MOUSEBUTTONDOWN:
                    switch(e->button.which){
                        case 0:
                        {
                            int mX;
                            int mY;
                            SDL_GetMouseState(&mX, &mY);
                            mY = SCREEN_HEIGHT - mY;

                            int halfX = SCREEN_WIDTH / 2 + pos[0];
                            int halfY = SCREEN_HEIGHT / 2 + pos[1];

                            if (halfX - 200 < mX && halfX + 200 > mX && halfY - 200 < mY && halfY + 200 > mY) grabbing = TRUE;
                            else grabbing = FALSE;
                        }
                        break;
                    }
                break;

                case SDL_MOUSEBUTTONUP:
                grabbing = FALSE;
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
    SDL_GLContext glContext;
    int win_width, win_height;
    
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
    window = SDL_CreateWindow( "EliasMog", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_ALLOW_HIGHDPI|SDL_WINDOW_UTILITY|SDL_WINDOW_RESIZABLE  );
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
    /*struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "../../../extra_font/DroidSans.ttf", 14, 0);*/
    /*struct nk_font *roboto = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Roboto-Regular.ttf", 16, 0);*/
    /*struct nk_font *future = nk_font_atlas_add_from_file(atlas, "../../../extra_font/kenvector_future_thin.ttf", 13, 0);*/
    /*struct nk_font *clean = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyClean.ttf", 12, 0);*/
    /*struct nk_font *tiny = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyTiny.ttf", 10, 0);*/
    /*struct nk_font *cousine = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13, 0);*/
    nk_sdl_font_stash_end();

    //Get window surface
    screenSurface = SDL_GetWindowSurface( window );

    
    LoadMedia(&image1, "Assets/eliasmog.bmp");
    LoadMedia(&image2, "Assets/hihi.bmp");


    InitShaderStuff();
    GenTexture();

    //Fill the surface white
    
    //Update the surface
    

    //Hack to get window to stay up
    SDL_Event e; 
    bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;
    while( quit == FALSE ){ 
        int currentMouse[2];
        SDL_GetMouseState(&currentMouse[0], &currentMouse[1]);
        nk_input_begin(ctx);
        while( SDL_PollEvent( &e ) != 0 ) {
            HandleEvents(&e);
            nk_sdl_handle_event(&e);
            }

        nk_sdl_handle_grab(); /* optional grabbing behavior */
        nk_input_end(ctx);

        if (grabbing) {
            pos[0] += currentMouse[0] - oldMouse[0];
            pos[1] -= currentMouse[1] - oldMouse[1];
        }
        /* GUI */
        if (nk_begin(ctx, "Demo", nk_rect(50, 50, 230, 250),
            NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
            NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
        {
            enum {EASY, HARD};
            static int op = EASY;
            static int property = 20;
            struct nk_image img;

            nk_layout_row_static(ctx, 30, 80, 1);
            if (nk_button_label(ctx, "Open Rom")){
                char path[MAX_PATH];
                OpenExplorer(window, path);
            }
            nk_layout_row_dynamic(ctx, 30, 2);
            if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
            if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;
            nk_layout_row_dynamic(ctx, 22, 1);
            nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);

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
        nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);

        Draw();

        SDL_GL_SwapWindow(window);

        oldMouse[0] = currentMouse[0];
        oldMouse[1] = currentMouse[1];
    }    
    
      //Destroy window
    SDL_DestroyWindow( window );
    SDL_FreeSurface(image1);
    SDL_FreeSurface(image2);

    //Quit SDL subsystems
    SDL_Quit();

    return 0;
}
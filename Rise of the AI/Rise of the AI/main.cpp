/**
* Author: Carl Shi
* Assignment: Rise of the AI
* Date due: 2025-04-05, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/


#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8
#define LEVEL1_LEFT_EDGE 5.0f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "LevelA.h"
#include "LevelB.h"
#include "LevelC.h"
#include "MenuScene.h"
#include "Win.h"
#include "Lose.h"

// ————— CONSTANTS ————— //
constexpr int WINDOW_WIDTH  = 640,
          WINDOW_HEIGHT = 480;

constexpr float BG_RED     = 0.1922f,
            BG_BLUE    = 0.549f,
            BG_GREEN   = 0.9059f,
            BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

enum AppStatus { RUNNING, TERMINATED };

// ————— GLOBAL VARIABLES ————— //
Scene *g_current_scene;
MenuScene *g_menu;
LevelA *g_levelA;
LevelB *g_levelB;
LevelC *g_levelC;
Win    *g_win;
Lose   *g_lost;

Scene   *g_levels[6];

SDL_Window* g_display_window;

AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

Mix_Music* g_bgm = nullptr;
Mix_Chunk* g_win_sound = nullptr;
Mix_Chunk* g_lost_sound = nullptr;

void switch_to_scene(Scene *scene)
{
    if (g_current_scene != nullptr) {
            scene->set_life(g_current_scene->get_state().life);
    }
    
    g_current_scene = scene;
    g_current_scene->initialise();
}

void initialise();
void process_input();
void update();
void render();
void shutdown();


void initialise()
{
    // ————— VIDEO ————— //
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Hello, Scenes!",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    if (context == nullptr)
    {
        shutdown();
    }
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    // ————— GENERAL ————— //
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    
    
    g_menu = new MenuScene();
    g_levelA = new LevelA();
    g_levelB = new LevelB();
    g_levelC = new LevelC();
    g_win    = new Win();
    g_lost   = new Lose();
    
    g_levels[0] = g_menu;
    g_levels[1] = g_levelA;
    g_levels[2] = g_levelB;
    g_levels[3] = g_levelC;
    g_levels[4] = g_win;
    g_levels[5] = g_lost;
        
    g_current_scene = g_menu;  
    switch_to_scene(g_menu);
    
    // ————— BLENDING ————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    g_bgm = Mix_LoadMUS("assets/tom_and_jerry_theme.mp3");
    Mix_PlayMusic(g_bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
    
    g_lost_sound = Mix_LoadWAV("assets/lose.wav");
    g_win_sound = Mix_LoadWAV("assets/win.wav");
}

void process_input()
{
    if(g_current_scene->get_state().player){
        g_current_scene->get_state().player->set_movement(glm::vec3(0.0f));
    }
    
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        // ————— KEYSTROKES ————— //
        switch (event.type) {
            // ————— END GAME ————— //
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_app_status = TERMINATED;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_q:
                        // Quit the game with a keystroke
                        g_app_status = TERMINATED;
                        break;
                        
                    case SDLK_SPACE:
                        // ————— JUMPING ————— //
                        if(g_current_scene != g_menu && g_current_scene != g_win && g_current_scene != g_lost){
                            if (g_current_scene->get_state().player->get_collided_bottom())
                            {
                                g_current_scene->get_state().player->jump();
                                Mix_PlayChannel(-1,  g_current_scene->get_state().jump_sfx, 0);
                            }
                        }
                        
                         break;
                    case SDLK_RETURN:
                        if(g_current_scene == g_menu){
                            switch_to_scene(g_levelA);
                        }
                        
                    default:
                        break;
                }
                
            default:
                break;
        }
    }
    
    // ————— KEY HOLD ————— //
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);
    
    
    if (g_current_scene != g_menu && g_current_scene != g_win && g_current_scene != g_lost){
        if (key_state[SDL_SCANCODE_LEFT])        g_current_scene->get_state().player->move_left();
        else if (key_state[SDL_SCANCODE_RIGHT])  g_current_scene->get_state().player->move_right();
        
    }

    
    if (g_current_scene != g_menu && g_current_scene != g_win && g_current_scene != g_lost){
        if (glm::length( g_current_scene->get_state().player->get_movement()) > 1.0f)
            g_current_scene->get_state().player->normalise_movement();
    }
    
   
 
}

void update()
{
    // ————— DELTA TIME / FIXED TIME STEP CALCULATION ————— //
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    delta_time += g_accumulator;
    
    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }
    
    while (delta_time >= FIXED_TIMESTEP) {
        // ————— UPDATING THE SCENE (i.e. map, character, enemies...) ————— //
        g_current_scene->update(FIXED_TIMESTEP);
        
        delta_time -= FIXED_TIMESTEP;
    }
    
    g_accumulator = delta_time;
    
    
    // ————— PLAYER CAMERA ————— //
    g_view_matrix = glm::mat4(1.0f);
    
    if(g_current_scene != g_menu && g_current_scene != g_win && g_current_scene != g_lost){
        if (g_current_scene->get_state().player->get_position().x > LEVEL1_LEFT_EDGE) {
            g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->get_state().player->get_position().x, 3.75, 0));
        } else {
            g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5, 3.75, 0));
        }
    }
    

    if (g_current_scene == g_levelA && g_current_scene->get_state().player->get_position().y < -10.0f) {
        switch_to_scene(g_levelB);
    }
    
    if (g_current_scene == g_levelB && g_current_scene->get_state().player->get_position().y < -10.0f) {
        switch_to_scene(g_levelC);
    }
    
    if (g_current_scene == g_levelC && g_current_scene->get_state().player->get_position().y < -10.0f) {
        if(g_current_scene->get_state().life > 0){
            Mix_PlayChannel(-1, g_win_sound, 0);
            switch_to_scene(g_win);
        }else{
            Mix_PlayChannel(-1, g_lost_sound, 0);
            switch_to_scene(g_lost);
        }
        
    }

    
    
    
   
}

void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);
    
    glClear(GL_COLOR_BUFFER_BIT);
    
//    glUseProgram(g_shader_program.get_program_id());
    g_current_scene->render(&g_shader_program);
    
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();
    
    delete g_menu;
    delete g_levelA;
    delete g_levelB;
}

// ————— GAME LOOP ————— //
int main(int argc, char* argv[])
{
    initialise();
    
    while (g_app_status == RUNNING)
    {
        process_input();
        update();
        
        if (g_current_scene->get_state().next_scene_id >= 0){ switch_to_scene(g_levels[g_current_scene->get_state().next_scene_id]);
        }
        
        render();
    }
    
    shutdown();
    return 0;
}

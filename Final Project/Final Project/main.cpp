/**
* Author: Carl Shi
* Assignment: Final Project
* Date due: 4/25, 5:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 15
#define LEVEL1_HEIGHT 15
#define LEVEL1_LEFT_EDGE 7.0f
#define LEVEL1_UP_EDGE 7.0f

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
#include "Menu.h"
#include "Win.h"
#include "Lose.h"

// ————— CONSTANTS ————— //
constexpr int WINDOW_WIDTH  = 640 * 2,
          WINDOW_HEIGHT = 480 * 2;

constexpr float BG_RED     = 0.0f,
            BG_BLUE    = 0.0f,
            BG_GREEN   = 0.0f,
            BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr char V_SHADER_PATH_DIM[] = "shaders/vertex_lit.glsl",
            F_SHADER_PATH_DIM[] = "shaders/fragment_lit.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

enum AppStatus { RUNNING, TERMINATED };

// ————— GLOBAL VARIABLES ————— //
Scene *g_current_scene = nullptr;
LevelA *g_LevelA = nullptr;
LevelB *g_LevelB = nullptr;
LevelC *g_LevelC = nullptr;
Menu   *g_Menu = nullptr;
Win    *g_Win = nullptr;
Lose   *g_Lose = nullptr;

SDL_Window* g_display_window = nullptr;

AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program;
ShaderProgram g_dim_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

Scene* g_scenes[6];

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

Mix_Music* g_bgm = nullptr;


void initialise();
void process_input();
void update();
void render();
void shutdown();
void switch_to_scene(Scene* scene);


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
    
    g_dim_shader_program.load(V_SHADER_PATH_DIM, F_SHADER_PATH_DIM);
    g_dim_shader_program.set_projection_matrix(g_projection_matrix);
    g_dim_shader_program.set_view_matrix(g_view_matrix);
    g_dim_shader_program.set_float("ambient", 0.2f);
    
    // ————— LEVEL A SETUP ————— //

    
//    g_menu   = new MenuScene();
    g_LevelA = new LevelA();
    g_LevelB = new LevelB();
    g_LevelC = new LevelC();
    g_Menu   = new Menu();
    g_Win    = new Win();
    g_Lose   = new Lose();
    g_scenes[0] = g_Menu;
    g_scenes[1] = g_LevelA;
    g_scenes[2] = g_LevelB;
    g_scenes[3] = g_LevelC;
    g_scenes[4] = g_Win;
    g_scenes[5] = g_Lose;
    
//    switch_to_scene(g_Menu);
//    switch_to_scene(g_LevelA);
    switch_to_scene(g_LevelB);
//    switch_to_scene(g_LevelC);
//    switch_to_scene(g_Win);
//    switch_to_scene(g_Lose);
    
    
//     ————— BLENDING ————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
        
    g_bgm = Mix_LoadMUS("assets/Graveyard.mp3");
    Mix_PlayMusic(g_bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
    

}

void switch_to_scene(Scene* scene) {
    if (g_current_scene != nullptr) {
        scene->set_life(g_current_scene->get_state().life);
    }
    if (g_current_scene == g_LevelB && scene == g_LevelC) {
        scene->set_wisp(g_current_scene->get_state().wisp_spawned);
    }
    g_current_scene = scene;
    g_current_scene->initialise();
}

void process_input()
{
    if(g_current_scene->get_state().player){
        g_current_scene->get_state().player->set_movement(glm::vec3(0.0f));
        g_current_scene->get_state().player->set_velocity(glm::vec3(0.0f));
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
                    case SDLK_SPACE:{
                        if(g_current_scene != g_Menu && g_current_scene != g_Win && g_current_scene != g_Lose){
                            Entity* player = g_current_scene->get_state().player;
                            player->set_velocity(glm::vec3(0.0f));
                            if(player->get_player_state() != ATTACKING){
                                player->set_player_state(ATTACKING);
                                Mix_PlayChannel(-1,  g_current_scene->get_state().swing_sfx, 0);
                            }
                        }
                        
                        
                        break;
                    }
                    case SDLK_RETURN: {
                        if(g_current_scene == g_Menu){
                            switch_to_scene(g_LevelA);
                        }
                        break;
                    }
                    
                    case SDLK_r:{
                        switch_to_scene(g_Menu);
                        break;
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

    if (g_current_scene != g_Menu && g_current_scene != g_Win && g_current_scene != g_Lose) {
        if (key_state[SDL_SCANCODE_A])  g_current_scene->get_state().player->move_left();
        if (key_state[SDL_SCANCODE_D]) g_current_scene->get_state().player->move_right();
        if (key_state[SDL_SCANCODE_W])    g_current_scene->get_state().player->move_up();
        if (key_state[SDL_SCANCODE_S])  g_current_scene->get_state().player->move_down();
    }
    
    if (g_current_scene != g_Menu && g_current_scene != g_Win && g_current_scene != g_Lose){
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
    
//    if (delta_time < FIXED_TIMESTEP)
//    {
//        g_accumulator = delta_time;
//        return;
//    }
    
    g_accumulator = delta_time;
    
    while (g_accumulator >= FIXED_TIMESTEP) {
        // ————— UPDATING THE SCENE (i.e. map, character, enemies...) ————— //
        g_current_scene->update(FIXED_TIMESTEP);
        g_accumulator -= FIXED_TIMESTEP;
    }
    
    int nextID = g_current_scene->get_state().next_scene_id;

    if(g_current_scene != nullptr && nextID != -1){
        switch_to_scene(g_scenes[nextID]);
    }
    
    
    if(g_current_scene != g_Menu && g_current_scene != g_Win && g_current_scene != g_Lose){
        
        glm::vec3 p = g_current_scene->get_state().player->get_position();
        
        g_view_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(-p.x, -p.y, 0.0f));
        
    }else{
        g_view_matrix = glm::mat4(1.0f);
    }

}

void render()
{
    
    ShaderProgram *active_program =
            (g_current_scene == g_LevelC) ? &g_dim_shader_program : &g_shader_program;
    
    if(g_current_scene == g_LevelC){
        if(g_current_scene->get_state().wisp_spawned){
            glm::vec3 pos = g_current_scene->get_state().wisp->get_position();
            active_program->set_light_position_matrix(pos);
        }
        
    }
    
    active_program->set_view_matrix(g_view_matrix);
    glUseProgram(active_program->get_program_id());

    glClear(GL_COLOR_BUFFER_BIT);

    g_current_scene->render(active_program);

    SDL_GL_SwapWindow(g_display_window);
    

    
//    g_shader_program.set_view_matrix(g_view_matrix);
//    
//    glClear(GL_COLOR_BUFFER_BIT);
//    
//    g_current_scene->render(&g_shader_program);

    
//    SDL_GL_SwapWindow(g_display_window);
    
}

void shutdown()
{
    SDL_Quit();
    
    // ————— DELETING LEVEL A DATA (i.e. map, character, enemies...) ————— //
    delete g_LevelA;
}

// ————— GAME LOOP ————— //
int main(int argc, char* argv[])
{
    initialise();
    
    while (g_app_status == RUNNING)
    {
        process_input();
        update();
        render();
    }
    
    shutdown();
    return 0;
}

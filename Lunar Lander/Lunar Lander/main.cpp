/**
 * Author: Carl Shi
 * Assignment: Lunar Lander
 * Date due: 2025-3-21, 11:59pm
 * I pledge that I have completed this assignment without
 * collaborating with anyone else, in conformance with the
 * NYU School of Engineering Policies and Procedures on
 * Academic Misconduct.
**/




#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define PLATFORM_COUNT 10

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"

struct GameState {
    Entity* rocket;
    Entity* platforms;
    Entity* background;
    Entity* game_win;
    Entity* game_lost;
    Entity* fuel;
};

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

constexpr char ROCKET_FILEPATH[]     = "assets/playerShip1_blue.png";
constexpr char PLATFORM_FILEPATH[]   = "assets/platformPack_tile016.png";
constexpr char TRAP_FILEPATH[]       = "assets/platformPack_tile011.png";
constexpr char TARGET_FILEPATH[]     = "assets/platformPack_tile036.png";
constexpr char BACKGROUND_FILEPATH[] = "assets/blue.png";
constexpr char WIN_FILEPATH[]        = "assets/win.png";
constexpr char LOST_FILEPATH[]       = "assets/game_over.jpg";
constexpr char FUEL_FILEPATH[]       = "assets/buttonBlue.png";


constexpr float MILLISECONDS_IN_SECOND = 1000.0;

constexpr int NUMBER_OF_TEXTURES = 1;
constexpr GLint LEVEL_OF_DETAIL  = 0;
constexpr GLint TEXTURE_BORDER   = 0;

GameState g_state;

SDL_Window* g_display_window;
bool g_game_is_running = true;

ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float         g_previous_ticks = 0.0f;
float         g_accumulator    = 0.0f;
float         g_gravity        = -4.905f;

bool game_win = false;
bool game_end = false;

GLuint load_texture(const char* filepath)
{
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);
    
    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }
    
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    stbi_image_free(image);
    
    return textureID;
};

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Lunar Lander",
                                        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                        640, 480,
                                        SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
//    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    
    //starting initialize
    
    g_state.platforms = new Entity[PLATFORM_COUNT];
    GLuint platform_texture = load_texture(PLATFORM_FILEPATH);
    GLuint target_texture = load_texture(TARGET_FILEPATH);
    GLuint trap_texture = load_texture(TRAP_FILEPATH);
    
    
    srand((unsigned int)time(NULL));
    int trap_idx = rand() % PLATFORM_COUNT;
    int target_idx = rand() % PLATFORM_COUNT;
    
    while(target_idx == trap_idx){
        target_idx = rand() % PLATFORM_COUNT;
    }
    
    
    for (int i = 0; i < PLATFORM_COUNT; i++) {
        int xpos = -4.5 + i;
        g_state.platforms[i].set_position(glm::vec3(xpos, -3.0f, 0.0f));
        if(i == trap_idx){
            g_state.platforms[i].set_texture_id(trap_texture);
            g_state.platforms[i].set_entity_type(TRAP);
        }else if(i == target_idx){
            g_state.platforms[i].set_texture_id(target_texture);
            g_state.platforms[i].set_entity_type(TARGET);
        }else{
//            g_state.platforms[i].set_texture_id(platform_texture);
            g_state.platforms[i].set_texture_id(platform_texture);
            g_state.platforms[i].set_entity_type(PLATFORM);
        }
        g_state.platforms[i].activate();
        g_state.platforms[i].update(0.0f, nullptr, 0.0f);
        
    }
    
    GLuint rocket_texture = load_texture(ROCKET_FILEPATH);
    g_state.rocket = new Entity();
    g_state.rocket->set_texture_id(rocket_texture);
    g_state.rocket->set_position(glm::vec3(-3.0f, 3.5f, 0.0f));
    g_state.rocket->set_movement(glm::vec3(0.0f));
    g_state.rocket->set_acceleration(glm::vec3(0.0f, g_gravity * 0.1, 0.0f));
    g_state.rocket->set_speed(1.0f);
    g_state.rocket->set_width(1.0f);
    g_state.rocket->set_height(1.0f);
    g_state.rocket->set_collisioin_type(NOCOLLISION);
    
    GLuint game_win_texture = load_texture(WIN_FILEPATH);
    g_state.game_win  = new Entity();
    g_state.game_win->set_texture_id(game_win_texture);
    g_state.game_win->set_position(glm::vec3(0.0f));
    g_state.game_win->set_entity_type(UI);
    g_state.game_win->set_width(8.0f);
    g_state.game_win->set_height(6.0f);
    g_state.game_win->deactivate();
    g_state.game_win->update(0.0f, nullptr, 0);
    
    GLuint game_lost_texture = load_texture(LOST_FILEPATH);
    g_state.game_lost = new Entity();
    g_state.game_lost->set_texture_id(game_lost_texture);
    g_state.game_lost->set_position(glm::vec3(0.0f));
    g_state.game_lost->set_entity_type(UI);
    g_state.game_lost->set_width(8.0f);
    g_state.game_lost->set_height(6.0f);
    g_state.game_lost->deactivate();
    g_state.game_lost->update(0.0f, nullptr, 0);
    
    GLuint bg_texture = load_texture(BACKGROUND_FILEPATH);
    g_state.background = new Entity();
    g_state.background->set_texture_id(bg_texture);
    g_state.background->set_entity_type(BACKGROUND);
    g_state.background->set_position(glm::vec3(0.0f, 0.0f, 0.0f));
    g_state.background->set_width(10.0f);
    g_state.background->set_height(7.5f);
    g_state.background->activate();
    g_state.background->update(0.0f, nullptr, 0);
    
    
    GLuint fuel_texture = load_texture(FUEL_FILEPATH);
    g_state.fuel = new Entity();
    g_state.fuel->set_texture_id(fuel_texture);
    g_state.fuel->set_entity_type(UI);
    g_state.fuel->set_position(glm::vec3(3.5f, 3.5f, 0.0f));
    g_state.fuel->set_width(2.0f);
    g_state.fuel->set_height(0.5f);
    g_state.fuel->activate();
    g_state.fuel->update(0.0f, nullptr, 0);
    
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
}

void process_input(){
    g_state.rocket->set_movement(glm::vec3(0.0f));

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                
            case SDL_WINDOWEVENT_CLOSE:
                g_game_is_running = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_q:
                        g_game_is_running = false;
                        break;
                        
                    default: break;
                }
                break;
                
            default: break;
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(nullptr);

    // movement -> acceleration
    glm::vec3 acc(0.0f);

    if(g_state.rocket->get_fuel() > 0.0f){
        if (keys[SDL_SCANCODE_LEFT]) {
            acc.x = -1.0f;
            g_state.rocket->set_rotation(90.0f);
            g_state.rocket->consume_fuel(0.1f);
        }
        else if (keys[SDL_SCANCODE_RIGHT]) {
            acc.x = 1.0f;
            g_state.rocket->set_rotation(270.0f);
            g_state.rocket->consume_fuel(0.1f);
        }

        if (keys[SDL_SCANCODE_UP]) {
            acc.y = 1.0f;
            g_state.rocket->set_rotation(0.0f);
            g_state.rocket->consume_fuel(0.1f);
        }
        else if (keys[SDL_SCANCODE_DOWN]) {
            acc.y = -1.0f;
            g_state.rocket->set_rotation(180.0f);
            g_state.rocket->consume_fuel(0.1f);
        }
        
        acc.y += g_gravity * 0.1f;
        g_state.rocket->set_acceleration(acc);
        
    }else{
        g_state.rocket->set_acceleration(glm::vec3(0.0f, g_gravity * 0.1f, 0.0f));
    }
    

    // If magnitude of movement is > 1, normalize
    if (glm::length(g_state.rocket->get_movement()) > 1.0f) {
        g_state.rocket->normalise_movement();
    }
    
}

void update(){
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    delta_time += g_accumulator;
    if (delta_time < FIXED_TIMESTEP) {
        g_accumulator = delta_time;
        return;
    }
    
    while (delta_time >= FIXED_TIMESTEP) {
        
        if(!game_end){
            CollisionType type = g_state.rocket->update(FIXED_TIMESTEP, g_state.platforms, PLATFORM_COUNT);

            
            if(type == LOST){
                game_end = true;
                game_win = false;
                g_state.rocket->set_movement(glm::vec3(0.0f));
                g_state.rocket->deactivate();
                g_state.game_lost->activate();
                
            }else if(type == WIN){
                game_end = true;
                game_win = true;
                g_state.rocket->set_movement(glm::vec3(0.0f));
                g_state.rocket->deactivate();
                g_state.game_win->activate();
                
            }else if(g_state.rocket->get_fuel() <= 0.0f){
                game_end = true;
                game_win = false;
                g_state.rocket->set_movement(glm::vec3(0.0f));
                g_state.rocket->deactivate();
                g_state.game_lost->activate();
            }
        }
        
        delta_time -= FIXED_TIMESTEP;
    }
    
    g_accumulator = delta_time;
    
    float scaleFactor = g_state.rocket->get_fuel() / 100.0f;
    float fullWidth   = 2.0f;
    g_state.fuel->set_width(fullWidth * scaleFactor);
    g_state.fuel->update_transform();
    
    

    
}


void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    g_state.background->render(&g_shader_program);
    
    g_state.fuel->render(&g_shader_program);

    
    for (int i = 0; i < PLATFORM_COUNT; i++)
    {
        g_state.platforms[i].render(&g_shader_program);
    }
    
    if(game_end){
        if (game_win)
        {
            g_state.game_win->update_transform();
            g_state.game_win->render(&g_shader_program);
        }
        else
        {
            g_state.game_lost->update_transform();
            g_state.game_lost->render(&g_shader_program);
        }
    }else{
        g_state.rocket->render(&g_shader_program);
    }
    
    
    
    
    SDL_GL_SwapWindow(g_display_window);
}



void shutdown() {
    SDL_Quit();
    delete[] g_state.platforms;
    delete g_state.rocket;
    delete g_state.background;
    delete g_state.game_win;
    delete g_state.game_lost;
}

int main(int argc, char* argv[])
{
    initialise();

    while (g_game_is_running)
    {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}

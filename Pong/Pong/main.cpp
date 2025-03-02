/**
* Author: Carl Shi
* Assignment: wave Clone
* Date due: 2025-3-01, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/




#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>

enum AppStatus { RUNNING, TERMINATED };

constexpr float WINDOW_SIZE_MULT = 1.0f;

constexpr int WINDOW_WIDTH  = 1280 * WINDOW_SIZE_MULT,
              WINDOW_HEIGHT = 720 * WINDOW_SIZE_MULT;

constexpr float BG_RED     = 0.9765625f,
                BG_GREEN   = 0.97265625f,
                BG_BLUE    = 0.9609375f,
                BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

constexpr char WAVE_FILEPATH[] = "wave.png",
               RED_SWORD_FILEPATH[]  = "redsword.png",
            BLUE_SWORD_FILEPATH[] = "bluesword.png";

//constexpr float MINIMUM_COLLISION_DISTANCE = 1.0f;

constexpr glm::vec3 INIT_SCALE_REDSWORD = glm::vec3(0.75f, 1.5f, 0.0f),
                    INIT_SCALE_BLUESWORD = glm::vec3(-0.75f,-1.5f, 0.0f),
                    INIT_SCALE_WAVE = glm::vec3(0.5f, 1.0f, 0.0f),
                    INIT_POS_WAVE  = glm::vec3(0.0f, 0.0f, 0.0f),
                    INIT_POS_REDSWORD   = glm::vec3(-4.0f, 0.0f, 0.0f),
                    INIT_POS_BLUESWORD = glm::vec3(4.0f, 0.0f, 0.0f);

SDL_Window* g_display_window;

AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program = ShaderProgram();
glm::mat4 g_view_matrix, g_wave_matrix, g_projection_matrix, g_red_sword_matrix, g_blue_sword_matrix;

float g_previous_ticks = 0.0f;

GLuint g_wave_texture_id;
GLuint g_red_sword_texture_id;
GLuint g_blue_sword_texture_id;

glm::vec3 g_wave_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_wave_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_red_sword_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_red_sword_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_blue_sword_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_blue_sword_movement = glm::vec3(0.0f, 0.0f, 0.0f);


float g_red_sword_speed = 2.0f;  // move 1 unit per second
float g_blue_sword_speed = 2.0f;
float g_wave_speed = 1.0f;

constexpr float TOP_BOUND = 3.75f,
                BOT_BOUND = -3.75f;

bool y_flip = false;
bool x_flip = false;

float wave_height = INIT_SCALE_WAVE.y * 0.5f;
float red_sword_height = INIT_SCALE_REDSWORD.y * 0.5f;
float blue_sword_height = INIT_SCALE_BLUESWORD.y * 0.5f;

bool single_player = false;

int red_points = 0, blue_points = 0;


void initialise();
void process_input();
void sword_wall_collision();
void sword_wave_collision();
void wave_wall_collision();
void update();
void render();
void shutdown();

constexpr GLint NUMBER_OF_TEXTURES = 1;  // to be generated, that is
constexpr GLint LEVEL_OF_DETAIL    = 0;  // base image level; Level n is the nth mipmap reduction image
constexpr GLint TEXTURE_BORDER     = 0;  // this value MUST be zero

GLuint load_texture(const char* filepath)
{
    // STEP 1: Loading the image file
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);
    
    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }
    
    // STEP 2: Generating and binding a texture ID to our image
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    // STEP 3: Setting our texture filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    // STEP 4: Releasing our file from memory and returning our texture id
    stbi_image_free(image);
    
    return textureID;
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("FIGHTTTTTT First to three",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    
    
    if (g_display_window == nullptr)
    {
        shutdown();
    }
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    
    g_wave_matrix = glm::mat4(1.0f);

    
    g_red_sword_matrix = glm::mat4(1.0f);
    
    g_blue_sword_matrix = glm::mat4(1.0f);
    
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);
    
    glUseProgram(g_shader_program.get_program_id());
    
    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);
    
    g_wave_texture_id = load_texture(WAVE_FILEPATH);
    g_red_sword_texture_id = load_texture(RED_SWORD_FILEPATH);
    g_blue_sword_texture_id = load_texture(BLUE_SWORD_FILEPATH);
    
    
    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    
    g_red_sword_movement = glm::vec3(0.0f, 0.0f, 0.0f);
    g_blue_sword_movement = glm::vec3(0.0f, 0.0f, 0.0f);


    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            // End game
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_app_status = TERMINATED;
                break;
                                                                             
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_q:
                        g_app_status = TERMINATED;
                        break;
                    case SDLK_w:
                        // Move the red UP
                        g_red_sword_movement.y = 1.0f;
                        break;
                                                                             
                    case SDLK_s:
                        // Move the red down
                        g_red_sword_movement.y = -1.0f;
                        break;
                                                                             
                    case SDLK_UP:
                        //  Blue up
                        g_blue_sword_movement.y = 1.0f;
                        break;
                    
                    case SDLK_DOWN:
                        //blue down
                        g_blue_sword_movement.y = -1.0f;
                        break;
                        
                    case SDLK_1:
                        //1 wave
                        
                        break;
                        
                    case SDLK_2:
                        //2 wave
                        
                        break;
                        
                    case SDLK_3:
                        //3 wave
                        
                        break;
                        
                    case SDLK_t:
                        //1 player mode
                        single_player = !single_player;
                        break;
                    
                                                                             
                    default:
                        break;
                }
                                                                             
            default:
                break;
        }
    }
                                                                             
                                                                             
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);
                                                                             
    if (key_state[SDL_SCANCODE_W])
    {
        g_red_sword_movement.y = 1.0f;
    }
    else if (key_state[SDL_SCANCODE_S])
    {
        g_red_sword_movement.y = -1.0f;
    }
                                                                             

    
    if(single_player){
        if(g_wave_position.y > g_blue_sword_position.y + fabs(blue_sword_height)){
            g_blue_sword_movement.y = 1.0f;
        }else if(g_wave_position.y < g_blue_sword_position.y - fabs(blue_sword_height)){
            g_blue_sword_movement.y = -1.0f;
        }else{
            g_blue_sword_movement.y = 0.0f;
        }
    }else{
        if (key_state[SDL_SCANCODE_UP])
        {
            g_blue_sword_movement.y = 1.0f;
        }
        else if (key_state[SDL_SCANCODE_DOWN])
        {
            g_blue_sword_movement.y = -1.0f;
        }
    }
    
    if (glm::length(g_red_sword_movement) > 1.0f)
    {
        g_red_sword_movement = glm::normalize(g_red_sword_movement);
    }else if (glm::length(g_blue_sword_movement) > 1.0f)
    {
        g_blue_sword_movement = glm::normalize(g_blue_sword_movement);
    }
    
}

void sword_wall_collision(){
    if(g_red_sword_position.y > TOP_BOUND - red_sword_height){
        
        g_red_sword_position.y = TOP_BOUND - red_sword_height;
        g_red_sword_movement = glm::vec3(0.0f, 0.0f, 0.0f);
        
    }else if(g_red_sword_position.y < BOT_BOUND + red_sword_height){
        g_red_sword_position.y = BOT_BOUND + red_sword_height;
        g_red_sword_movement = glm::vec3(0.0f, 0.0f, 0.0f);
    }
    
    if(g_blue_sword_position.y > TOP_BOUND - fabs(blue_sword_height)){
        
        g_blue_sword_position.y = TOP_BOUND - fabs(blue_sword_height);
        g_blue_sword_movement = glm::vec3(0.0f, 0.0f, 0.0f);
        
    }else if(g_blue_sword_position.y < BOT_BOUND + fabs(blue_sword_height)){
        
        g_blue_sword_position.y = BOT_BOUND + fabs(blue_sword_height);
        g_blue_sword_movement = glm::vec3(0.0f, 0.0f, 0.0f);
    }
    
}


void wave_wall_collision(){

    float wave_top = g_wave_position.y + wave_height;
    float wave_bottom = g_wave_position.y - wave_height;
    
    float wave_left = g_wave_position.x - wave_height;
    float wave_right = g_wave_position.x + wave_height;
    
    
    if (wave_left <= -5.0f)
    {
        blue_points += 1;
        std::cout << red_points << " : " << blue_points << "\n";
        g_wave_position = glm::vec3(0.0f, 0.0f, 0.0f);
        g_wave_movement = glm::vec3(0.0f, 0.0f, 0.0f);
    }
    else if (wave_right >= 5.0f)
    {
        red_points += 1;
        std::cout << red_points << " : " << blue_points << "\n";
        g_wave_position = glm::vec3(0.0f, 0.0f, 0.0f);
        g_wave_movement = glm::vec3(0.0f, 0.0f, 0.0f);
    }
    
    if (wave_top >= TOP_BOUND){
        y_flip = true;
        g_wave_movement.y = -fabs(g_wave_movement.y);
    }else if(wave_bottom <= BOT_BOUND){
        y_flip = false;
//        g_wave_position.y = BOT_BOUND + wave_height;
        g_wave_movement.y = fabs(g_wave_movement.y);
    }
    
    if(x_flip && y_flip){
        g_wave_movement = glm::vec3(-g_wave_speed, -g_wave_speed, 0.0f);
    }else if(y_flip){
        g_wave_movement = glm::vec3(g_wave_speed, -g_wave_speed, 0.0f);
    }else if(x_flip){
        g_wave_movement = glm::vec3(-g_wave_speed, g_wave_speed, 0.0f);
    }else{
        g_wave_movement = glm::vec3(g_wave_speed, g_wave_speed, 0.0f);
    }
}


void sword_wave_collision(){
    

    
    float red_wave_x_distance = fabs((g_red_sword_position.x + INIT_POS_REDSWORD.x)  - (g_wave_position.x + INIT_POS_WAVE.x)) - ((INIT_SCALE_WAVE.x + INIT_SCALE_REDSWORD.x) / 2.0f);
    
    float red_wave_y_distance = fabs((g_red_sword_position.y + INIT_POS_REDSWORD.y)  - (g_wave_position.y + INIT_POS_WAVE.y)) - ((INIT_SCALE_WAVE.y + INIT_SCALE_REDSWORD.y) / 2.0f);
    
    float blue_wave_x_distance = fabs((g_blue_sword_position.x + INIT_POS_BLUESWORD.x)  - (g_wave_position.x + INIT_POS_WAVE.x)) - ((INIT_SCALE_WAVE.x + fabs(INIT_SCALE_BLUESWORD.x)) / 2.0f);
    
    float blue_wave_y_distance = fabs((g_blue_sword_position.y + INIT_POS_BLUESWORD.y)  - (g_wave_position.y + INIT_POS_WAVE.y)) - ((INIT_SCALE_WAVE.y + fabs(INIT_SCALE_BLUESWORD.y)) / 2.0f);
    
    
    if ((red_wave_x_distance < 0.0f && red_wave_y_distance < 0.0f) ||
         (blue_wave_x_distance < 0.0f && blue_wave_y_distance < 0.0f))
    {
        x_flip = !x_flip;
        g_wave_movement.x *= -1;
        
    }
    

    
    
}


void update()
{
    float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND; // get the current number of ticks
    float delta_time = ticks - g_previous_ticks; // the delta time is the difference from the last frame
    g_previous_ticks = ticks;

    // Add direction * units per second * elapsed time
    g_red_sword_position += g_red_sword_movement * g_red_sword_speed * delta_time;
    g_blue_sword_position += g_blue_sword_movement * g_blue_sword_speed * delta_time;
    g_wave_position += g_wave_movement * g_wave_speed * delta_time;
    
    
    g_red_sword_matrix = glm::mat4(1.0f);
    g_red_sword_matrix = glm::translate(g_red_sword_matrix, INIT_POS_REDSWORD);
    g_red_sword_matrix = glm::translate(g_red_sword_matrix, g_red_sword_position);
    
    g_blue_sword_matrix = glm::mat4(1.0f);
    g_blue_sword_matrix = glm::translate(g_blue_sword_matrix, INIT_POS_BLUESWORD);
    g_blue_sword_matrix = glm::translate(g_blue_sword_matrix, g_blue_sword_position);
    
    g_wave_matrix = glm::mat4(1.0f);
    g_wave_matrix = glm::translate(g_wave_matrix, INIT_POS_WAVE);
    g_wave_matrix = glm::translate(g_wave_matrix, g_wave_position);
    
    
    sword_wall_collision();
    
    sword_wave_collision();
    wave_wall_collision();

    if(red_points == 3) {
        std::cout << "Red wins\n";
        g_app_status = TERMINATED;
    }else if(blue_points == 3){
        std::cout << "Blue wins\n";
        g_app_status = TERMINATED;
    }
    

    
    g_red_sword_matrix = glm::scale(g_red_sword_matrix, INIT_SCALE_REDSWORD);
    g_blue_sword_matrix  = glm::scale(g_blue_sword_matrix, INIT_SCALE_BLUESWORD);
    g_wave_matrix = glm::scale(g_wave_matrix, INIT_SCALE_WAVE);
    
    
    
}


void draw_object(glm::mat4 &object_model_matrix, GLuint &object_texture_id)
{
    g_shader_program.set_model_matrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so we use 6 instead of 3
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Vertices
    float vertices[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,  // triangle 1
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f   // triangle 2
    };

    // Textures
    float texture_coordinates[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
    };
    
    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());
    
    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    // Bind texture
    draw_object(g_red_sword_matrix, g_red_sword_texture_id);
    draw_object(g_blue_sword_matrix, g_blue_sword_texture_id);
    draw_object(g_wave_matrix, g_wave_texture_id);
    
    // We disable two attribute arrays now
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown() { SDL_Quit(); }


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

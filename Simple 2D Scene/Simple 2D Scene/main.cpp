/**
* Author: Carl Shi
* Assignment: Simple 2D Scene
* Date due: 2025-02-15, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/


#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'
#define GL_GLEXT_PROTOTYPES 1

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"


enum AppStatus { RUNNING, TERMINATED };

// Our window dimensions
constexpr int WINDOW_WIDTH  = 640 * 2,
              WINDOW_HEIGHT = 480 * 2;

// Background color components
constexpr float BG_RED     = 0.9765625f,
                BG_GREEN   = 0.97265625f,
                BG_BLUE    = 0.9609375f,
                BG_OPACITY = 1.0f;

// Our viewport—or our "camera"'s—position and dimensions
constexpr int VIEWPORT_X      = 0,
              VIEWPORT_Y      = 0,
              VIEWPORT_WIDTH  = WINDOW_WIDTH,
              VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
               F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

constexpr GLint NUMBER_OF_TEXTURES = 1, // to be generated, that is
                LEVEL_OF_DETAIL    = 0, // mipmap reduction image level
                TEXTURE_BORDER     = 0; // this value MUST be zero


constexpr char clove_filepath[] = "clove.png";
constexpr char Mystbloom_filepath[] = "Mystbloom.png";

constexpr glm::vec3 INIT_SCALE_CLOVE      = glm::vec3(2.0f, 2.0f, 0.0f),
                    INIT_SCALE_MYSTBLOOM  = glm::vec3(0.5f, 0.5f, 0.0f),
                    INIT_POS_CLOVE        = glm::vec3(0.0f, 0.0f, 0.0f),
                    INIT_POS_MYSTBLOOM    = glm::vec3(-3.0f, 0.0f, 0.0f);

constexpr float ROT_INCREMENT = 1.0f;

float g_previous_ticks = 0.0f;
float g_clove_x = 0.0f;
float g_clove_y = 0.0f;
float g_mystbloom_x = 0.0f;
float g_mystbloom_y = 0.0f;

glm::vec3 g_clove_position = glm::vec3(0.0f);
glm::vec3 g_mystbloom_position = glm::vec3(0.0f);


constexpr float SCALE_SPEED = 0.5f;
constexpr float MAX_SCALE = 3.0f;
constexpr float MIN_SCALE = -3.0f;

AppStatus g_app_status = RUNNING;
SDL_Window* g_display_window;
ShaderProgram g_shader_program;

glm::mat4 g_view_matrix,
          g_clove_matrix,
          g_mystbloom_matrix,
          g_model_matrix,
          g_projection_matrix;


glm::vec3 g_rotation_clove = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_rotation_mystbloom = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_scale_mystbloom = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_size_mystbloom = glm::vec3(1.0f, 1.0f, 0.0f);

GLuint g_clove_texture_id,
g_mystbloom_texture_id;

GLuint load_texture(const char* filepath){
    
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

        stbi_image_free(image);

        return textureID;
    
    
}

void initialise()
{
    // HARD INITIALISE ———————————————————————————————————————————————————————————————————
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("Hello, Triangle!",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);
    
    if (g_display_window == nullptr)
    {
        std::cerr << "ERROR: SDL Window could not be created.\n";
        g_app_status = TERMINATED;
        
        SDL_Quit();
        exit(1);
    }
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_clove_matrix = glm::mat4(1.0f);
    g_mystbloom_matrix = glm::mat4(1.0f);
    
    g_view_matrix       = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);
    
    glUseProgram(g_shader_program.get_program_id());
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    g_clove_texture_id = load_texture(clove_filepath);
    g_mystbloom_texture_id = load_texture(Mystbloom_filepath);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
        {
            g_app_status = TERMINATED;
        }
    }
}


glm::vec3 clove_rotate = glm::vec3(0.0f);
glm::vec3 mystbloom_rotate = glm::vec3(0.0f);

void update() {
    float ticks = (float) SDL_GetTicks() / 1000.0f;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    
    //clove rotation
    g_clove_x += 1.0 * delta_time;
    g_clove_y += 1.0 * delta_time;
    
    clove_rotate = glm::vec3(cos(g_clove_x), sin(g_clove_y), 0.0f);
    
    g_clove_matrix = glm::mat4(1.0f);
    g_clove_matrix = glm::translate(g_clove_matrix, clove_rotate);
    
    //rotate around clove
    g_mystbloom_x = g_clove_x + 5;
    g_mystbloom_y = g_clove_y + 5;
    
    
    mystbloom_rotate = glm::vec3(cos(g_mystbloom_x),
                                 sin(g_mystbloom_y), 0.0f);
    
    
    g_mystbloom_matrix = glm::mat4(1.0f);
    g_mystbloom_matrix = glm::translate(g_mystbloom_matrix, mystbloom_rotate);

    //rotating clove;
    
    g_rotation_clove.y += ROT_INCREMENT * delta_time;


    g_clove_matrix = glm::translate(g_clove_matrix, INIT_POS_CLOVE);
    g_clove_matrix = glm::rotate(g_clove_matrix,
                              g_rotation_clove.y,
                              glm::vec3(0.0f, 1.0f, 0.0f));

    
    //scaling
    
    
    g_clove_matrix = glm::scale(g_clove_matrix, INIT_SCALE_CLOVE);

    g_mystbloom_matrix = glm::scale(g_mystbloom_matrix, INIT_SCALE_MYSTBLOOM);
    

    g_size_mystbloom +=  SCALE_SPEED * delta_time;
    g_mystbloom_matrix = glm::scale(g_mystbloom_matrix, g_size_mystbloom);

    
    
    if (g_size_mystbloom.x <= MIN_SCALE) {
        g_size_mystbloom = glm::vec3(MIN_SCALE);
        g_scale_mystbloom = glm::vec3(1.0f, 1.0f, 0.0f);
    }
    else if (g_size_mystbloom.x >= MAX_SCALE) {
        g_size_mystbloom = glm::vec3(MAX_SCALE);
        g_scale_mystbloom = glm::vec3(-1.0f, -1.0f, 0.0f);
    }
    
    
}


void shutdown() { SDL_Quit(); }

void draw_object(glm::mat4 &object_g_model_matrix, GLuint &object_texture_id)
{
    g_shader_program.set_model_matrix(object_g_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so use 6, not 3
}


void render(){
    glClear(GL_COLOR_BUFFER_BIT);

        // Vertices
        float vertices[] =
        {
            -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
            -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f
        };

        // Textures
        float texture_coordinates[] =
        {
            0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        };

        glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false,
                              0, vertices);
        glEnableVertexAttribArray(g_shader_program.get_position_attribute());

        glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT,
                              false, 0, texture_coordinates);
        glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

        // Bind texture
        draw_object(g_clove_matrix, g_clove_texture_id);
        draw_object(g_mystbloom_matrix, g_mystbloom_texture_id);

        // We disable two attribute arrays now
        glDisableVertexAttribArray(g_shader_program.get_position_attribute());
        glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

        SDL_GL_SwapWindow(g_display_window);
}

int main(int argc, char* argv[])
{
    // Initialise our program—whatever that means
    initialise();
    
    while (g_app_status == RUNNING)
    {
        process_input();  // If the player did anything—press a button, move the joystick—process it
        update();         // Using the game's previous state, and whatever new input we have, update the game's state
        render();         // Once updated, render those changes onto the screen
    }
    
    shutdown();  // The game is over, so let's perform any shutdown protocols
    return 0;
}

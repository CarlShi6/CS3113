/**
* Author: Carl Shi
* Assignment: Rise of the AI
* Date due: 2025-04-05, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/


#include "Menu.h"
#include "Utility.h"
#include "LevelA.h"

#define FONT_TEXTURE_PATH "assets/font2.png"

void Menu::initialise() {
    m_game_state.next_scene_id = -1;
    m_font_texture = Utility::load_texture(FONT_TEXTURE_PATH);
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    m_game_state.bgm = Mix_LoadMUS("assets/Graveyard.mp3");
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
}

void Menu::update(float delta_time) {
    
}

void Menu::render(ShaderProgram *program) {
    glClear(GL_COLOR_BUFFER_BIT);

    glm::mat4 model_matrix = glm::mat4(1.0f);
    program->set_model_matrix(model_matrix);
    
    Utility::draw_text(program, m_font_texture, "Terade", 0.75f, -0.15f, glm::vec3(-1.5f, 2.0f, 0));

    Utility::draw_text(program, m_font_texture, "Press Enter to Start", 0.3f, -0.15f, glm::vec3(-1.5f, -1.0f, 0));
    
    Utility::draw_text(program, m_font_texture, "w,a,s,d - move", 0.3f, -0.15f, glm::vec3(-1.5f, -2.0f, 0));
    
    Utility::draw_text(program, m_font_texture, "space - attack", 0.3f, -0.15f, glm::vec3(-1.5f, -2.25f, 0));
}

Menu::~Menu(){
    Mix_FreeMusic(m_game_state.bgm);
}

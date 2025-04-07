#include "MenuScene.h"
#include "Utility.h"
#include "LevelA.h"

#define FONT_TEXTURE_PATH "assets/font2.png"

void MenuScene::initialise() {
    m_game_state.next_scene_id = -1;
    m_font_texture = Utility::load_texture(FONT_TEXTURE_PATH);
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    m_game_state.bgm = Mix_LoadMUS("assets/tom_and_jerry_theme.mp3");
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
}

void MenuScene::update(float delta_time) {
    
}

void MenuScene::render(ShaderProgram *program) {
    glClear(GL_COLOR_BUFFER_BIT);

    glm::mat4 model_matrix = glm::mat4(1.0f);
    program->set_model_matrix(model_matrix);
    
   
    Utility::draw_text(program, m_font_texture, "You are Jerry", 0.5f, -0.25f, glm::vec3(-1.5f, 1.0f, 0));
    Utility::draw_text(program, m_font_texture, "Press Enter to Start", 0.5f, -0.15f, glm::vec3(-3.0f, 0.0f, 0));
}

MenuScene::~MenuScene(){
    Mix_FreeMusic(m_game_state.bgm);
}


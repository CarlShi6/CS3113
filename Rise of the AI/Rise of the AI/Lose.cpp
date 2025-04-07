#include "MenuScene.h"
#include "Utility.h"
#include "Lose.h"

#define FONT_TEXTURE_PATH "assets/font2.png"

void Lose::initialise() {
    m_game_state.next_scene_id = -1;
    m_font_texture = Utility::load_texture(FONT_TEXTURE_PATH);
    
}

void Lose::update(float delta_time) {
    
}

void Lose::render(ShaderProgram *program) {
    glClear(GL_COLOR_BUFFER_BIT);

    glm::mat4 model_matrix = glm::mat4(1.0f);
    program->set_model_matrix(model_matrix);
    
   
    Utility::draw_text(program, m_font_texture, "You Lost", 0.5f, -0.25f, glm::vec3(-1.5f, 1.0f, 0));
    
}

Lose::~Lose(){
    Mix_FreeMusic(m_game_state.bgm);
}


/**
* Author: Carl Shi
* Assignment: Final Project
* Date due: 4/25, 5:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "LevelA.h"
#include "Utility.h"
#include <cmath>

#define LEVEL_WIDTH 15
#define LEVEL_HEIGHT 15



static constexpr char PLAYER_IDLE_FILEPATH[] = "assets/Soldier-Idle.png",
                PLAYER_WALK_FILEPATH[] = "assets/Soldier-Walk.png",
                PLAYER_ATTACK_FILEPATH[] = "assets/Soldier-Attack01.png",
                PLAYER_HURT_FILEPATH[] = "assets/Soldier-Hurt.png",
                PLAYER_DEATH_FILEPATH[] = "assets/Soldier-Death.png",
                ENEMY_FILEPATH[]        = "assets/skeleton1_movement.png",
                KEY_FILEPATH[]          = "assets/Key.png";

static unsigned int LEVEL_DATA[] =
{
    10, 10, 10, 10,  1,  1,  1,  1,  1,  1,  1, 10, 10, 10, 10,
    10, 10, 10, 10,  3,  0,  0,  0,  0,  0,  4, 10, 10, 10, 10,
    10, 10, 10, 10,  3,  0,  0,  0,  0,  0,  4, 10, 10, 10, 10,
    10, 10, 10, 10,  3,  0,  0,  0,  0,  0,  4, 10, 10, 10, 10,
     1,  1,  1,  1,  1,  0,  0,  0,  0,  0,  1,  1,  1,  1,  4,
     3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4,
     3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4,
     3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4,
     3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4,
     2,  2,  2,  2,  2,  0,  0,  0,  0,  0,  2,  2,  2,  2,  2,
    10, 10, 10, 10,  3,  0,  0,  0,  0,  0,  4, 10, 10, 10, 10,
    10, 10, 10, 10,  3,  0,  0,  0,  0,  0,  4, 10, 10, 10, 10,
    10, 10, 10, 10,  3,  0,  0,  0,  0,  0,  4, 10, 10, 10, 10,
    10, 10, 10, 10,  3,  0,  0,  0,  0,  0,  4, 10, 10, 10, 10,
    10, 10, 10, 10,  2,  2,  2,  2,  2,  2,  2, 10, 10, 10, 10
};

//1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
//3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4,
//3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4,
//3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4,
//3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4,
//3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4,
//3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4,
//3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4,
//3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4,
//3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4,
//3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4,
//3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4,
//3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4,
//3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4,
//2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2


LevelA::~LevelA()
{
    if (m_game_state.enemies) {
            delete[] m_game_state.enemies;
    }
    delete    m_game_state.player;
    delete    m_game_state.map;

//    Mix_FreeMusic(m_game_state.bgm);
    Mix_FreeChunk(m_game_state.swing_sfx);
    Mix_FreeChunk(m_game_state.enemy_hurt_sfx);
    Mix_FreeChunk(m_game_state.player_hurt_sfx);
}

constexpr float PLAYER_SPEED   = 2.0f;

void LevelA::initialise(){
    enemy_count = ENEMY_COUNT;
    m_game_state.next_scene_id = -1;
    m_game_state.life = 3;
    
    m_keySpawned = false;
    m_keyCollected = false;
    //map part
    
    GLuint map_texture_id = Utility::load_texture("assets/Dungeon_Tileset.png");
    m_game_state.map =new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_DATA, map_texture_id, 1.0f, 10, 10);
    
    //player part
    
    GLuint t_idle   = Utility::load_texture(PLAYER_IDLE_FILEPATH);
    GLuint t_walk   = Utility::load_texture(PLAYER_WALK_FILEPATH);
    GLuint t_attack = Utility::load_texture(PLAYER_ATTACK_FILEPATH);
    GLuint t_hurt   = Utility::load_texture(PLAYER_HURT_FILEPATH);
    GLuint t_death  = Utility::load_texture(PLAYER_DEATH_FILEPATH);
    
    m_game_state.player = new Entity(t_idle, PLAYER_SPEED, 2.5f, 2.5f, PLAYER);
    
    m_game_state.player->set_state_texture(IDLE,     t_idle);
    m_game_state.player->set_state_texture(WALKING,  t_walk);
    m_game_state.player->set_state_texture(ATTACKING,t_attack);
    m_game_state.player->set_state_texture(HURT,     t_hurt);
    m_game_state.player->set_state_texture(DEATH,    t_death);
    
    m_game_state.player->set_player_state(IDLE);
    
    m_game_state.player->set_position(glm::vec3(7.0f,-2.0f,0.0f));
    
    m_game_state.player->set_collision_dimensions(1.0f, 1.0f);
    m_game_state.player->set_collision_offset({0.0f, 0.0f, 0.0f});
    
    //enemy part
    GLuint enemy_tex = Utility::load_texture(ENEMY_FILEPATH);
    m_game_state.enemies = new Entity[ENEMY_COUNT];
    for (int i = 0; i < ENEMY_COUNT; ++i) {
        Entity &e = m_game_state.enemies[i];
        e.set_position(glm::vec3(3.0f, -7.0f, 0.0f));
        e.set_collision_dimensions(0.70f, 0.70f);
        e.set_width(1.0f);
        e.set_height(1.0f);
        e.set_entity_type(ENEMY);
        e.set_ai_type(WALKER);
        e.set_speed(2.0);
        e.set_velocity(glm::vec3(1.0f, 0.0f, 0.0f));
        e.set_texture_id(enemy_tex);
        e.set_animation_cols(10);
        e.set_animation_index(0);
    }
    
    //ui part
    
    m_heart_texture_id = Utility::load_texture("assets/icon_heart.png");
    
    //ITEM PART
    
    m_key_texture_id = Utility::load_texture(KEY_FILEPATH);

    m_key = new Entity(m_key_texture_id, 0.0f, 0.25f, 0.25f, ITEM);
    m_key->set_animation_cols(1);
    m_key->set_collision_dimensions(0.25f, 0.25f);
    m_key->deactivate();
    
    GLuint m_door_tex_id = Utility::load_texture("assets/door.png");
    m_door = new Entity(m_door_tex_id, 0.0f, 1.0f, 1.0f, ITEM);
    m_door->set_animation_cols(1);
    m_door->set_collision_dimensions(0.4f, 0.4f);
    m_door->deactivate();

    
    m_game_state.swing_sfx = Mix_LoadWAV("assets/swing.wav");
    m_game_state.enemy_hurt_sfx = Mix_LoadWAV("assets/enemy_hurt.wav");
    m_game_state.player_hurt_sfx = Mix_LoadWAV("assets/player_hurt.wav");

    
}

void LevelA::update(float deltaTime) {
  
    m_game_state.player->update(deltaTime, m_game_state.player, nullptr, 0, m_game_state.map);

    if (m_game_state.enemies) {        
        
        for(int i = 0; i < ENEMY_COUNT; i++ ){
            Entity &enemy = m_game_state.enemies[i];
            if (!enemy.isActive()) continue;
            float oldV = enemy.get_velocity().x;
            enemy.update(deltaTime, m_game_state.player, nullptr, 0, m_game_state.map);
            float newV = enemy.get_velocity().x;
            
            if(oldV >= 0 && newV == 0){
                enemy.set_velocity(glm::vec3(-2.0f, 0.0f, 0.0f));
                enemy.face_left();
            }else if(oldV < 0 && newV == 0){
                enemy.set_velocity(glm::vec3(2.0f, 0.0f,0.0f));
                enemy.face_right();
            }
            
            if (m_game_state.player->check_collision(&enemy)){
                if(m_game_state.player->get_player_state() == ATTACKING){
                    int frame = m_game_state.player->get_animation_index();
                    if (frame == 3 || frame == 4) {
                        enemy.deactivate();
                        --enemy_count;
                        if (enemy_count == 0 && !m_keySpawned) {
                            glm::vec3 deathPos = enemy.get_position();
                            m_keySpawned = true;
                            m_key->activate();
                            m_key->set_position(glm::vec3(deathPos.x+0.5f, deathPos.y+0.5f, 0.0f));
                            m_door->activate();
                            m_door->set_position(glm::vec3(7.0f, -12.0f, 0.0f));
                        }
                    }
                    Mix_PlayChannel(-1, m_game_state.enemy_hurt_sfx, 0);
                }else if(m_game_state.player->get_player_state() != HURT){
                    m_game_state.life--;
                    m_game_state.player->set_player_state(HURT);
                    Mix_PlayChannel(-1, m_game_state.player_hurt_sfx, 0);
                }
            }
//            m_game_state.enemies->update(deltaTime, m_game_state.player, nullptr, 0, m_game_state.map);
        }
    }
        
    if(m_keySpawned && !m_keyCollected){
        m_key->update(0.0f,nullptr,nullptr,0,m_game_state.map);
        
    }
    
    if (m_game_state.player->check_collision(m_key)) {
        m_keyCollected = true;
        m_key->deactivate();
    }
    
    if(m_door->isActive()){
        m_door->update(0.0f, nullptr, nullptr, 0, m_game_state.map);
    }
    
    if(m_game_state.player->check_collision(m_door)){
        if(m_keyCollected){
            m_game_state.next_scene_id = 2;
            return;
        }
    }
    
    if(m_game_state.life == 0){
        m_game_state.next_scene_id = 5;
        return;
    }
        

}

void LevelA::render(ShaderProgram* program) {
  // map
    m_game_state.map->render(program);

  // player
    m_game_state.player->render(program);
    //enemy
    for (int i = 0; i < ENEMY_COUNT; i++){
        Entity &enemy = m_game_state.enemies[i];
        if(enemy.isActive()){
            enemy.render(program);
        }
    }


    glm::vec3 p = m_game_state.player->get_position();
    float left = p.x - 5.0f + 0.5f;     // 0.5 units in from left edge
    float top  = p.y + 3.75f - 0.5f;    // 0.5 units down from top
    
    if (m_key->isActive()) {
        m_key->render(program);
    }
    if(m_door->isActive()){
        m_door->render(program);
    }

    for (int i = 0; i < m_game_state.life; ++i) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f),
                                         glm::vec3(left + i * 0.6f, top, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 1.0f));
        program->set_model_matrix(model);

        glBindTexture(GL_TEXTURE_2D, m_heart_texture_id);

        // draw a unit quad
        float verts[] = {
            -0.5f, -0.5f,   0.5f, -0.5f,   0.5f,  0.5f,
            -0.5f, -0.5f,   0.5f,  0.5f,  -0.5f,  0.5f
        };
        float uvs[] = {
             0.0f, 1.0f,   1.0f, 1.0f,   1.0f, 0.0f,
             0.0f, 1.0f,   1.0f, 0.0f,   0.0f, 0.0f
        };
        glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, GL_FALSE, 0, verts);
        glEnableVertexAttribArray(program->get_position_attribute());
        glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, GL_FALSE, 0, uvs);
        glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(program->get_position_attribute());
        glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
    }
    
    if (m_keyCollected) {
        glm::vec3 p   = m_game_state.player->get_position();
        float iconX = p.x + 5.0f  - 0.5f;
        float iconY = p.y + 3.75f - 0.5f;
//        float left    = p.x - 5.0f + 0.5f + m_game_state.life * 0.6f;
//        float top     = p.y + 3.75f  - 0.5f;

        glm::mat4 model = glm::translate(glm::mat4(1.0f),
                                 glm::vec3(iconX, iconY,  0.0f));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 1.0f));
        program->set_model_matrix(model);

        glBindTexture(GL_TEXTURE_2D, m_key_texture_id);
        float verts[] = {
            -0.5f, -0.5f,   0.5f, -0.5f,   0.5f,  0.5f,
            -0.5f, -0.5f,   0.5f,  0.5f,  -0.5f,  0.5f
        };
        float uvs[] = {
             0.0f, 1.0f,   1.0f, 1.0f,   1.0f, 0.0f,
             0.0f, 1.0f,   1.0f, 0.0f,   0.0f, 0.0f
        };
        glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, GL_FALSE, 0, verts);
        glEnableVertexAttribArray(program->get_position_attribute());
        glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, GL_FALSE, 0, uvs);
        glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(program->get_position_attribute());
        glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
    }
}

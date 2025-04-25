/**
 * Author: Carl Shi
 * Assignment: Final Project
 * Date due: 4/25, 5:00pm
 * I pledge that I have completed this assignment without
 * collaborating with anyone else, in conformance with the
 * NYU School of Engineering Policies and Procedures on
 * Academic Misconduct.
 **/

#include "LevelB.h"
#include "Utility.h"
#include <cmath>

// Map size
#define LEVEL_WIDTH 15
#define LEVEL_HEIGHT 15

// Filepaths
static constexpr char
  PLAYER_IDLE_FILEPATH[]   = "assets/Soldier-Idle.png",
  PLAYER_WALK_FILEPATH[]   = "assets/Soldier-Walk.png",
  PLAYER_ATTACK_FILEPATH[] = "assets/Soldier-Attack01.png",
  PLAYER_HURT_FILEPATH[]   = "assets/Soldier-Hurt.png",
  PLAYER_DEATH_FILEPATH[]  = "assets/Soldier-Death.png",
  ENEMY_FILEPATH[]         = "assets/skeleton1_movement.png",
  GUARD_FILEPATH[]         = "assets/enemies-skeleton2_idle.png",
  ARROW_FILEPATH[]         = "assets/Arrow.png",
  KEY_FILEPATH[]           = "assets/Key.png",
  BOTTLE_FILEPATH[]        = "assets/Potion.png",
  DOOR_FILEPATH[]          = "assets/door.png",
  WISP_FILEPATH[]          = "assets/WispInABottle.png",
WBCARD_FILEPATH[]        = "assets/WBcard.png";

static unsigned int LEVEL_DATA[] = {
        1,  1,  1,  1,  1, 10, 10, 10, 10, 10,  1,  1,  1,  1,  1,
        3,  0,  0,  0,  6, 10, 10, 10, 10, 10,  8,  0,  0,  0,  4,
        3,  0,  0,  0,  6, 10, 10, 10, 10, 10,  8,  0,  0,  0,  4,
        3,  0,  0,  0,  6, 10, 10, 10, 10, 10,  8,  0,  0,  0,  4,
        3,  0,  0,  0,  6, 10, 10, 10, 10, 10,  8,  0,  0,  0,  4,
        3,  0,  0,  6,  7, 10, 10, 10, 10, 10,  8,  0,  0,  0,  4,
        3,  0,  0,  6, 10, 10, 10, 10, 10, 10,  8,  0,  0,  0,  4,
        3,  0,  0,  6, 10, 10, 10, 10, 10, 10,  8,  0,  0,  0,  4,
        3,  0,  0,  6, 10, 10, 10, 10, 10, 10,  8,  0,  0,  0,  4,
        3,  0,  0,  0,  7,  7,  7,  7,  7,  7,  0,  0,  0,  0,  4,
        3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4,
        3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4,
        3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4,
        3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4,
        2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2
};

constexpr float PLAYER_SPEED = 2.0f;
//constexpr float GUARD_SHOOT_INTERVAL = 2.0f; // seconds between shots

LevelB::~LevelB() {
    delete[]   m_game_state.enemies;
    delete     m_game_state.player;
    delete     m_game_state.map;
    delete     m_key;
    delete     m_bottle;
    delete     m_door;
    delete     m_arrow;
}

void LevelB::initialise(){
    enemy_count      = ENEMY_COUNT;   // say ENEMY_COUNT == 2
    m_game_state.next_scene_id = -1;
//    m_game_state.life          = 3;
    m_keySpawned    = false;
    m_keyCollected  = false;
    m_bottleSpawned= false;
    m_doorSpawned  = false;
    m_wispSpawned = false;
    m_guardLastShot= 0.0f;

    // -- MAP --
    GLuint mapTex = Utility::load_texture("assets/Dungeon_Tileset.png");
    m_game_state.map = new Map(
      LEVEL_WIDTH, LEVEL_HEIGHT,
      LEVEL_DATA, mapTex,
      1.0f, 10, 10
    );

    // -- PLAYER --
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
    m_game_state.player->set_position({3.0f, -2.0f, 0});
    m_game_state.player->set_collision_dimensions(1.0f,1.0f);

    // -- ENEMIES ---
    GLuint eTex1 = Utility::load_texture(ENEMY_FILEPATH);
    GLuint eTex2 = Utility::load_texture(GUARD_FILEPATH);
    m_game_state.enemies = new Entity[ENEMY_COUNT];
    // walker
    {
        Entity &e = m_game_state.enemies[0];
        e.set_entity_type(ENEMY);
        e.set_position({12.0f,-11.0f,0.0f});
        e.set_collision_dimensions(0.75f,0.75f);
        e.set_width(1.0f); e.set_height(1.0f);
        e.set_ai_type(WALKER);
        e.set_speed(2.0f);
        e.set_velocity({2,0,0});
        e.set_texture_id(eTex1);
        e.set_animation_cols(10);
    }
    // guard
    {
        Entity &g = m_game_state.enemies[1];
        g.set_entity_type(ENEMY);
        g.set_position({12.0f,-13.0f,0.0f});
        g.set_collision_dimensions(0.75f,0.75f);
        g.set_width(1.0f);
        g.set_height(1.0f);
        g.set_ai_type(GUARD);
        g.set_speed(0);
        g.set_texture_id(eTex2);
        g.set_animation_cols(6);
    }

    // -- ARROW (single instance reused) --
    GLuint arrowTex = Utility::load_texture(ARROW_FILEPATH);
    m_arrow = new Arrow(arrowTex, 3.0f);
    m_arrow->set_collision_dimensions(0.5f, 0.5f);
    m_arrow->deactivate();

    // -- PICKUPS & DOOR --
    m_key    = new Entity(Utility::load_texture(KEY_FILEPATH),    0,0.5f,0.5f,ITEM);
    m_key->set_width(0.25f);
    m_key->set_height(0.25f);
    m_key->set_animation_cols(1);
    m_key->deactivate();
    
    m_bottle = new Entity(Utility::load_texture(BOTTLE_FILEPATH), 0,0.5f,0.5f,ITEM);
    m_bottle->set_animation_cols(1);
    m_bottle->set_collision_dimensions(0.5f, 0.5f);
    m_bottle->deactivate();
    
    m_door   = new Entity(Utility::load_texture(DOOR_FILEPATH),   0,1.0f,1.0f,ITEM);
    m_door->set_animation_cols(1);
    m_door->deactivate();
    
    
    //Wisp in a bottle
    GLuint wispTex = Utility::load_texture(WISP_FILEPATH);
    m_game_state.wisp = new Entity(wispTex, 1.9f, 0.25f, 0.25f, ENEMY);
    m_game_state.wisp->set_animation_cols(5);
    m_game_state.wisp->set_animation_rows(1);
    m_game_state.wisp->set_animation_frames(5);
    m_game_state.wisp->deactivate();
    
    m_wispCardTexture = Utility::load_texture(WBCARD_FILEPATH);


    // -- HEART ICON --
    m_heart_texture_id = Utility::load_texture("assets/icon_heart.png");
    
    m_game_state.swing_sfx = Mix_LoadWAV("assets/swing.wav");
    m_game_state.enemy_hurt_sfx = Mix_LoadWAV("assets/enemy_hurt.wav");
    m_game_state.player_hurt_sfx = Mix_LoadWAV("assets/player_hurt.wav");
}

void LevelB::update(float deltaTime){
    
//    SDL_PumpEvents();
//
//    if (m_showWispCard) {
//        const Uint8* keys = SDL_GetKeyboardState(NULL);
//        if (keys[SDL_SCANCODE_ESCAPE]) {
//          m_showWispCard = false;
//            
//        }
//        return;
//    }
    
    // player update
    m_game_state.player->update(deltaTime, m_game_state.player, nullptr,0,m_game_state.map);

    // enemies
    if(m_game_state.enemies){
        for(int i=0; i<ENEMY_COUNT; i++){
            
            Entity &e = m_game_state.enemies[i];
            if(!e.isActive()) continue;
            if(e.get_ai_type() == WALKER){
                int oldV = e.get_velocity().x;
                e.update(deltaTime, m_game_state.player, nullptr, 0, m_game_state.map);
                int newV = e.get_velocity().x;
                
                if(oldV >= 0 && newV == 0){
                    e.set_velocity(glm::vec3(-2.0f, 0.0f, 0.0f));
                    e.face_left();
                }else if(oldV < 0 && newV == 0){
                    e.set_velocity(glm::vec3(2.0f, 0.0f,0.0f));
                    e.face_right();
                }
                
            }else if(e.get_ai_type()==GUARD){
                e.update(deltaTime, m_game_state.player, nullptr, 0, m_game_state.map);
                m_guardLastShot+=deltaTime;
                if(m_guardLastShot>=GUARD_SHOOT_INTERVAL && !m_arrow->isActive()){
                  // aim arrow at player
                  glm::vec3 ppos = m_game_state.player->get_position();
                  glm::vec3 gpos = e.get_position();
                  m_arrow->shoot(gpos, ppos);
                  m_guardLastShot=0;
                }
            }
            if(m_game_state.player->check_collision(&e)){
                if(m_game_state.player->get_player_state() == ATTACKING){
                    int frame = m_game_state.player->get_animation_index();
                    if (frame == 3 || frame == 4){
                        enemy_count--;
                        e.deactivate();
                        if(e.get_ai_type() == WALKER){
                            m_keySpawned=true;
                            m_key->activate();
                            glm::vec3 pos = e.get_position();
                            m_key->set_position(glm::vec3(pos.x+0.5f, pos.y+0.5f, 0.0f));
                            m_key->update(0.0f,nullptr,nullptr,0,m_game_state.map);
                        }else if(e.get_ai_type() == GUARD && !m_bottleSpawned){
                            m_bottleSpawned = true;
                            m_bottle->activate();
                            glm::vec3 pos = e.get_position();
                            m_bottle->set_position(glm::vec3(pos.x + 0.5f, pos.y+0.5f, 0.0f));
                            m_bottle->update(0.0f,nullptr,nullptr,0,m_game_state.map);
                        }
                    }
                    Mix_PlayChannel(-1, m_game_state.enemy_hurt_sfx, 0);
                }else if(m_game_state.player->get_player_state() != HURT){
                    m_game_state.life--;
                    m_game_state.player->set_player_state(HURT);
                    Mix_PlayChannel(-1, m_game_state.player_hurt_sfx, 0);
                }
            }

        }
        
    }
    


    // update arrow
    if(m_arrow->isActive()){
      m_arrow->update(deltaTime,m_game_state.map);
        if(m_arrow->isActive() && m_arrow->check_collision(m_game_state.player)){
            if(m_game_state.player->get_player_state()!=HURT){
                  m_game_state.life--;
                  m_game_state.player->set_player_state(HURT);
                  Mix_PlayChannel(-1, m_game_state.player_hurt_sfx, 0);
            }
            m_arrow->deactivate();
        }
    }
    

    // pickups
    if(m_keySpawned && m_key->isActive() && m_game_state.player->check_collision(m_key)){
      m_keyCollected=true;
      m_key->deactivate();
    }
    
    
    if(m_bottleSpawned && m_bottle->isActive() && m_game_state.player->check_collision(m_bottle)){
        
        m_bottle->deactivate();
        if (!m_wispSpawned) {
            m_wispSpawned = true;
            m_game_state.wisp_spawned = true;
            m_game_state.wisp->activate();
            glm::vec3 pp = m_game_state.player->get_position();
            m_game_state.wisp->set_position(glm::vec3(pp.x, pp.y+1.0f, pp.z));
            if (!m_wispCardAlreadyShown) {
              m_showWispCard         = true;
              m_wispCardAlreadyShown = true;
            }
        }
    }
    
    if (m_wispSpawned) {
        m_wispSpawned = true;
        m_game_state.wisp->activate();
        glm::vec3 wp = m_game_state.wisp->get_position();
        glm::vec3 pp = m_game_state.player->get_position();
        glm::vec3 dir = pp - wp;
        if (glm::length(dir) > 0.5f) {
            dir = glm::normalize(dir);
            m_game_state.wisp->set_velocity(dir * m_game_state.wisp->get_speed());
        } else {
            m_game_state.wisp->set_velocity(glm::vec3(0.0f));
        }
        m_game_state.wisp->update(deltaTime, m_game_state.player, nullptr, 0, m_game_state.map);
        
    }

    if(m_keyCollected && !m_doorSpawned){
        m_doorSpawned=true;
        m_door->activate();
        m_door->set_position({12.0f,-2.0f,0});
        m_door->update(0.0f, nullptr, nullptr, 0, m_game_state.map);
    }

    // collision with door
    if(m_game_state.player->check_collision(m_door)){
        if(m_keyCollected){
            m_game_state.next_scene_id = 3;
            return;
        }
    }
    if(m_game_state.life == 0){
        m_game_state.next_scene_id = 5;
        return;
    }
    
}

void LevelB::render(ShaderProgram* program){

    // map
    m_game_state.map->render(program);

    // player
    m_game_state.player->render(program);
    
    if(m_wispSpawned && m_game_state.wisp->isActive()){
        m_game_state.wisp->render(program);
    }

    // enemies
    for(int i=0; i<ENEMY_COUNT; i++){
      if(m_game_state.enemies[i].isActive())
        m_game_state.enemies[i].render(program);
    }

    // arrow
    if(m_arrow->isActive()) m_arrow->render(program);

    // pickups
    if(m_key->isActive())    m_key->render(program);
    if(m_bottle->isActive()) m_bottle->render(program);
    if(m_door->isActive())   m_door->render(program);

    // draw hearts
    glm::vec3 p = m_game_state.player->get_position();
    float left = p.x - 5.0f + 0.5f;
    float top  = p.y + 3.75f - 0.5f;
    for(int i=0;i<m_game_state.life;i++){
      glm::mat4 model = glm::translate(glm::mat4(1.0f),
        glm::vec3(left + i*0.6f, top, 0));
      model = glm::scale(model, glm::vec3(0.5f,0.5f,1));
      program->set_model_matrix(model);
      glBindTexture(GL_TEXTURE_2D, m_heart_texture_id);
      float verts[] = {
        -0.5f,-0.5f,  0.5f,-0.5f,  0.5f,0.5f,
        -0.5f,-0.5f,  0.5f,0.5f,  -0.5f,0.5f
      };
      float uvs[] = {
        0,1,  1,1,  1,0,
        0,1,  1,0,  0,0
      };
      glVertexAttribPointer(program->get_position_attribute(),2,GL_FLOAT,false,0,verts);
      glEnableVertexAttribArray(program->get_position_attribute());
      glVertexAttribPointer(program->get_tex_coordinate_attribute(),2,GL_FLOAT,false,0,uvs);
      glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
      glDrawArrays(GL_TRIANGLES,0,6);
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
        
        m_key_texture_id = Utility::load_texture(KEY_FILEPATH);

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








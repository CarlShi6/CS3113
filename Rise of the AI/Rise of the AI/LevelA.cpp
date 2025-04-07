#include "LevelA.h"
#include "Utility.h"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8

constexpr char SPRITESHEET_FILEPATH[] = "assets/Mouse-Sheet.png",
//           PLATFORM_FILEPATH[]    = "assets/TerrainPart.png",
                ENEMY_SIT_FILEPATH[]       = "assets/Cat-Sit.png",
            ENEMY_WALK_FILEPATH[] = "assets/Cat-Walk.png";

unsigned int LEVELA_DATA[] =
{
      13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      29, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

LevelA::~LevelA()
{
    delete [] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}

void LevelA::initialise()
{
    m_game_state.next_scene_id = -1;
    
    GLuint map_texture_id = Utility::load_texture("assets/tilemap_packed.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELA_DATA, map_texture_id, 1.0f, 16, 7);
    
    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);

    int player_walking_animation[1][4] ={0, 1, 2, 3};

    glm::vec3 acceleration = glm::vec3(0.0f, -4.81f, 0.0f);

    m_game_state.player = new Entity(
        player_texture_id,         // texture id
        5.0f,                      // speed
        acceleration,              // acceleration
        5.0f,                      // jumping power
        player_walking_animation,  // animation index sets
        0.0f,                      // animation time
        4,                         // animation frame amount
        0,                         // current animation index
        4,                         // animation column amount
        1,                         // animation row amount
        1.0f,                      // width
        1.0f,                       // height
        PLAYER
    );
    
    m_game_state.player->set_position(glm::vec3(5.0f, 0.0f, 0.0f));
    m_game_state.player->set_collision_offset(glm::vec3(0.0f, 0.0f, 0.0f));

    // Jumping
    m_game_state.player->set_jumping_power(3.5f);
    
    /**
     Enemies' stuff */
    GLuint enemy_sit_texture_id = Utility::load_texture(ENEMY_SIT_FILEPATH);

    m_game_state.enemies = new Entity[ENEMY_COUNT];

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
    m_game_state.enemies[i] =  Entity(enemy_sit_texture_id, 1.0f, 2.0f, 2.0f, ENEMY, GUARD, IDLE);
    }


    m_game_state.enemies[0].set_position(glm::vec3(10.0f, 0.0f, 0.0f));
    m_game_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    m_game_state.enemies[0].set_collision_offset(glm::vec3(0.0f, 0.0f, 0.0f));
    m_game_state.enemies[0].set_colision_dimension(1.0f, 0.75f);

    //Walking enemies
    

    
    /**
     BGM and SFX
//     */
//    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
//    
//    m_game_state.bgm = Mix_LoadMUS("assets/tom_and_jerry_theme.mp3");
//    Mix_PlayMusic(m_game_state.bgm, -1);
//    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
    
    m_game_state.jump_sfx = Mix_LoadWAV("assets/jump.wav");
}

void LevelA::update(float delta_time)
{
    
    
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
    
    for (int i = 0; i < ENEMY_COUNT; i++)
        {
            float distance = glm::distance(m_game_state.player->get_position(), m_game_state.enemies[i].get_position());
            float chaseThreshold = 3.0f; 

            if (distance < chaseThreshold)
            {
                glm::vec3 direction = glm::normalize(m_game_state.player->get_position() - m_game_state.enemies[i].get_position());
                m_game_state.enemies[i].set_movement(direction);
//                m_game_state.enemies[i].set_ai_state(WALKING);
                    
                if (m_game_state.enemies[i].get_ai_state() != WALKING)
                {
                    m_game_state.enemies[i].set_texture_id(Utility::load_texture(ENEMY_WALK_FILEPATH));
                                
                    int enemy_walking_animation[1][8] = { {0, 1, 2, 3, 4, 5, 6, 7} };
                    m_game_state.enemies[i].set_animation_cols(8);
                    m_game_state.enemies[i].set_animation_rows(1);
                    m_game_state.enemies[i].set_animation_frames(8);
                    m_game_state.enemies[i].set_animation_index(0);
                    m_game_state.enemies[i].set_enemy_walking(enemy_walking_animation);
                    m_game_state.enemies[i].set_ai_state(WALKING);
                 }
            }
            else
            {
                m_game_state.enemies[i].set_movement(glm::vec3(0.0f));
                m_game_state.enemies[i].set_ai_state(IDLE);
                m_game_state.enemies[i].set_texture_id(Utility::load_texture(ENEMY_SIT_FILEPATH));

            }
            
            m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map);
        }
    
    for (int i = 0; i < ENEMY_COUNT; i++){
        if(m_game_state.player->check_collision(&m_game_state.enemies[i])){
            m_game_state.life--;
            if(m_game_state.life <=0){
                m_game_state.next_scene_id = 5;
                return;
            }else
            {
                m_game_state.player->set_position(glm::vec3(5.0f, 0.0f, 0.0f));
            }
        }
        m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map);
        
    }
    
    if (m_game_state.player->get_position().y < -10.0f) m_game_state.next_scene_id += 1;
    

}


void LevelA::render(ShaderProgram *g_shader_program)
{
    m_game_state.map->render(g_shader_program);
    m_game_state.player->render(g_shader_program);
    for (int i = 0; i < m_number_of_enemies; i++)
            m_game_state.enemies[i].render(g_shader_program);
}

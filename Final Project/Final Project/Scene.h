#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Utility.h"
#include "Entity.h"
#include "Map.h"


struct GameState
{
    // ————— GAME OBJECTS ————— //
    Map *map;
    Entity *player;
    Entity *enemies;
    Entity *wisp;
    
    // ————— AUDIO ————— //
    Mix_Chunk* swing_sfx;
    Mix_Chunk* enemy_hurt_sfx;
    Mix_Chunk* player_hurt_sfx;
    Mix_Music* bgm;
    
    // ————— POINTERS TO OTHER SCENES ————— //
    int next_scene_id;
    
    int life = 3;
    
    bool wisp_spawned = false;
};

class Scene {
protected:
    GameState m_game_state;
    
public:
    // ————— ATTRIBUTES ————— //
    int m_number_of_enemies = 1;
    
    // ————— METHODS ————— //
    virtual void initialise() = 0;
    virtual void update(float delta_time) = 0;
    virtual void render(ShaderProgram *program) = 0;
    
    // ————— GETTERS ————— //
    GameState const get_state() const { return m_game_state;             }
    int const get_number_of_enemies() const { return m_number_of_enemies; }
    void set_life(int new_life) {m_game_state.life = new_life;}
    void set_wisp(bool new_wisp) {m_game_state.wisp_spawned = new_wisp;}
};

#ifndef ENTITY_H
#define ENTITY_H


#include "glm/glm.hpp"
#include "ShaderProgram.h"
#include "Map.h"

enum EntityType { PLATFORM, PLAYER, ENEMY, ITEM};
enum PlayerState{ IDLE, WALKING, ATTACKING, DEATH, HURT};
enum AIType     { WALKER, GUARD            };
enum AIState    { AI_WALK, AI_IDLE, AI_ATTACKING };



class Entity
{
private:
    bool m_is_active = true;

    
    EntityType m_entity_type;
    PlayerState m_player_state = IDLE;
    AIType     m_ai_type;
    AIState    m_ai_state;

    // ————— TRANSFORMATIONS ————— //
    glm::vec3 m_movement;
    glm::vec3 m_position;
    glm::vec3 m_scale;
    glm::vec3 m_velocity;
    glm::vec3 m_acceleration;

    glm::mat4 m_model_matrix;

    float     m_speed = 0.0f;
    
    float     m_direction   = 1.0f;
    
    GLuint m_texture_id = 0;
    static constexpr int STATE_COUNT = 5;
    GLuint m_state_texture[STATE_COUNT] = {0};

    //animation frame preset
    static constexpr int IDLE_FRAME_COUNT   = 6;
    static constexpr int WALK_FRAME_COUNT   = 8;
    static constexpr int ATTACK_FRAME_COUNT = 6;
    static constexpr int DEATH_FRAME_COUNT  = 4;
    static constexpr int HURT_FRAME_COUNT   = 4;

    int m_idle[IDLE_FRAME_COUNT];
    int m_walk[WALK_FRAME_COUNT];
    int m_attack[ATTACK_FRAME_COUNT];
    int m_death[DEATH_FRAME_COUNT];
    int m_hurt[HURT_FRAME_COUNT];

    // ————— ANIMATION ————— //
    int   m_animation_cols    = 0;
    int   m_animation_rows    = 1;
    int   m_animation_index   = 0;
    int   m_animation_frames  = 0;
    
    int*  m_animation_indices = nullptr;
    float m_animation_time    = 0.0f;


    float m_width = 1.0f,
          m_height = 1.0f;

    // ————— COLLISIONS ————— //
    
    bool   m_collided_top    = false;
    bool   m_collided_bottom = false;
    bool   m_collided_left   = false;
    bool   m_collided_right  = false;
    glm::vec3 m_collision_offset = glm::vec3(0.0f);
    float     collision_width    = 1.0f;
    float     collision_height   = 1.0f;
    
    

public:
    // ————— STATIC VARIABLES ————— //
    static constexpr int SECONDS_PER_FRAME = 4;

    // ————— METHODS ————— //
    
    Entity();
    Entity(GLuint texture_id, float speed, glm::vec3 acceleration);
    Entity(GLuint texture_id, float speed, float width, float height, EntityType type);
    Entity(GLuint texture_id, float speed, float width, float height, EntityType type,
           AIType aiType, AIState aiState);
    ~Entity();

    //redering & animation
    void draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index);
    void update(float delta_time, Entity *player, Entity *collidable_entities, int collidable_entity_count, Map* map);
    void render(ShaderProgram* program);

    //collision checkking
    void const check_collision_y(Map *map);
    void const check_collision_x(Map *map);
    
    bool const check_collision(Entity* other) const;
//        
//    void const check_collision_y(Entity* collidable_entities, int collidable_entity_count);
//    void const check_collision_x(Entity* collidable_entities, int collidable_entity_count);
    
    
    //state stuff
    void ai_activate(Entity *player);
    void ai_walk();
    void ai_guard(Entity *player);
    
    //movement and facing
    void normalise_movement() { m_velocity = glm::normalize(m_velocity); }
    void face_left()  { m_direction = -1.0f; }
    void face_right() { m_direction = +1.0f; }
    void move_left()  { m_velocity.x = -1.0f * m_speed; face_left(); }
    void move_right() { m_velocity.x = +1.0f * m_speed; face_right(); }
    void move_up()    { m_velocity.y = +1.0f * m_speed; }
    void move_down()  { m_velocity.y = -1.0f * m_speed; }
    

    // ————— GETTERS ————— //
    EntityType const get_entity_type()    const { return m_entity_type;   };
    PlayerState const get_player_state() const {return m_player_state;};
    AIType     const get_ai_type()        const { return m_ai_type;       };
    AIState    const get_ai_state()       const { return m_ai_state;      };
    glm::vec3 const get_position()     const { return m_position; }
    glm::vec3 const get_velocity()     const { return m_velocity; }
    glm::vec3 const get_acceleration() const { return m_acceleration; }
    glm::vec3 const get_movement()     const { return m_movement; }
    glm::vec3 const get_scale()        const { return m_scale; }
    GLuint    const get_texture_id()   const { return m_texture_id; }
    float     const get_speed()        const { return m_speed; }
    bool      const get_collided_top() const { return m_collided_top; }
    bool      const get_collided_bottom() const { return m_collided_bottom; }
    bool      const get_collided_right() const { return m_collided_right; }
    bool      const get_collided_left() const { return m_collided_left; }
    float     const get_width()    const {return m_width;}
    float     const get_height()    const {return m_height;}
    
    int       const get_animation_index() const { return m_animation_index;}
    
    void activate()   { m_is_active = true;  };
    void deactivate() { m_is_active = false; };
    bool const isActive() const {return m_is_active;};
    
    
    // ————— SETTERS ————— //
    void const set_entity_type(EntityType new_entity_type)  { m_entity_type = new_entity_type;};
    void const set_ai_type(AIType new_ai_type){ m_ai_type = new_ai_type;};
    void const set_ai_state(AIState new_state){ m_ai_state = new_state;};
    void const set_position(glm::vec3 new_position) { m_position = new_position; }
    void const set_velocity(glm::vec3 new_velocity) { m_velocity = new_velocity; }
    void const set_acceleration(glm::vec3 new_acceleration) { m_acceleration = new_acceleration; }
    void const set_movement(glm::vec3 new_movement) { m_movement = new_movement; }
    void const set_scale(glm::vec3 new_scale) { m_scale = new_scale; }
    void const set_texture_id(GLuint new_texture_id) { m_texture_id = new_texture_id; }
    void const set_speed(float new_speed) { m_speed = new_speed; }
    void const set_animation_cols(int new_cols) { m_animation_cols = new_cols; }
    void const set_animation_rows(int new_rows) { m_animation_rows = new_rows; }
    void const set_animation_frames(int new_frames) { m_animation_frames = new_frames; }
    void const set_animation_index(int new_index) { m_animation_index = new_index; }
    void const set_animation_time(float new_time) { m_animation_time = new_time; }

    void const set_width(float new_width) {m_width = new_width; }
    void const set_height(float new_height) {m_height = new_height; }
    void set_player_state(PlayerState state);
    void set_state_texture(PlayerState s, GLuint tex);
    
    void set_collision_dimensions(float w, float h) {
      collision_width  = w;
      collision_height = h;
    }
    void set_collision_offset(const glm::vec3 &off) {
      m_collision_offset = off;
    }
    


};

#endif // ENTITY_H


#ifndef ENTITY_H
#define ENTITY_H
#include "glm/glm.hpp"
#include "ShaderProgram.h"


enum CollisionType {WIN, LOST, NOCOLLISION};
enum EntityType {PLATFORM, TRAP, PLAYER, TARGET, BACKGROUND, UI};

class Entity
{
    
public:
    static constexpr int SECONDS_PER_FRAME = 4;
    
private:
    CollisionType m_collision_type = NOCOLLISION;
    EntityType m_entity_type;
    
    bool m_is_active = true;

    // ————— TRANSFORMATIONS ————— //
    glm::vec3 m_movement;
    glm::vec3 m_position;
    glm::vec3 m_velocity;
    glm::vec3 m_acceleration;
    float     m_speed;
    float     m_rotation;
    
    

    glm::mat4 m_model_matrix;

    // ————— TEXTURES ————— //
    GLuint    m_texture_id;

    float m_width = 1.0f,
          m_height = 1.0f;
    // ————— COLLISIONS ————— //
    bool m_collided_top    = false;
    bool m_collided_bottom = false;
    bool m_collided_left   = false;
    bool m_collided_right  = false;
    
    //FUEL
    float     m_fuel = 100.0f;
    
    
   

public:
    // ————— METHODS ————— //
    Entity();
    ~Entity();
    
    bool const check_collision(Entity* other) const;
    CollisionType const check_collision_y(Entity* collidable_entities, int collidable_entity_count);
    CollisionType const check_collision_x(Entity* collidable_entities, int collidable_entity_count);
    
    CollisionType update(float delta_time, Entity* collidable_entities, int collidable_entity_count);
    void render(ShaderProgram* program);

    void normalise_movement() { m_movement = glm::normalize(m_movement); }
    
    void move_left() { m_movement.x = -1.0f;}
    void move_right() { m_movement.x = 1.0f; }
    void move_up() { m_movement.y = 1.0f; }
    void move_down() { m_movement.y = -1.0f; }
    
    void activate() { m_is_active = true; }
    void deactivate() { m_is_active = false; }
    
    void update_transform();
    
    // ————— GETTERS ————— //
    CollisionType const get_collison_type()    const { return m_collision_type; };
    EntityType const get_entity_type()    const { return m_entity_type; };
    glm::vec3 const get_position()     const { return m_position; }
    glm::vec3 const get_velocity()     const { return m_velocity; }
    glm::vec3 const get_acceleration() const { return m_acceleration; }
    glm::vec3 const get_movement()     const { return m_movement; }
    GLuint    const get_texture_id()   const { return m_texture_id; }
    float     const get_speed()        const { return m_speed; }
    float     const get_rotation()           const { return m_rotation; }
    int       const get_width()        const { return m_width; };
    int       const get_height()       const { return m_height; };
    
    
    bool      const get_collided_top() const { return m_collided_top; }
    bool      const get_collided_bottom() const { return m_collided_bottom; }
    bool      const get_collided_right() const { return m_collided_right; }
    bool      const get_collided_left() const { return m_collided_left; }
    
    float     const get_fuel()           const {return m_fuel;}
    
    // ————— SETTERS ————— //
    void const set_collisioin_type(CollisionType new_collision_type)  { m_collision_type = new_collision_type;};
    void set_entity_type(EntityType new_entity_type)  { m_entity_type = new_entity_type;}

    void set_position(glm::vec3 new_position) { m_position = new_position; }
    void set_velocity(glm::vec3 new_velocity) { m_velocity = new_velocity; }
    void set_acceleration(glm::vec3 new_acceleration) { m_acceleration = new_acceleration; }
    void set_movement(glm::vec3 new_movement) { m_movement = new_movement; }
    void set_texture_id(GLuint new_texture_id) { m_texture_id = new_texture_id; }
    void set_speed(float new_speed) { m_speed = new_speed; }

    void set_width(float new_width) {m_width = new_width; }
    void set_height(float new_height) {m_height = new_height; }
    void scale(glm::vec3 new_scale) { m_model_matrix = glm::scale(m_model_matrix, new_scale); }
    void set_rotation(float degrees)     { m_rotation   = degrees; }
    
    void consume_fuel(float amount);
};

#endif // ENTITY_H

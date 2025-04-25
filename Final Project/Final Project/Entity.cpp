/**
* Author: Carl Shi
* Assignment: Final Project
* Date due: 4/25, 5:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/



#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Entity.h"

Entity::Entity() {
    for(int i = 0; i < IDLE_FRAME_COUNT;   ++i) m_idle[i]   = i;
    for(int i = 0; i < WALK_FRAME_COUNT;   ++i) m_walk[i]   = i;
    for(int i = 0; i < ATTACK_FRAME_COUNT; ++i) m_attack[i] = i;
    for(int i = 0; i < HURT_FRAME_COUNT;   ++i) m_hurt[i]   = i;
    for(int i = 0; i < DEATH_FRAME_COUNT;  ++i) m_death[i]  = i;
    set_player_state(IDLE);
}

Entity::Entity(GLuint texture_id, float speed, glm::vec3 acceleration)
  : Entity() {
    m_texture_id   = texture_id;
    m_speed        = speed;
    m_acceleration = acceleration;
}

Entity::Entity(GLuint texture_id, float speed, float width, float height, EntityType type)
  : Entity(texture_id, speed, glm::vec3(0.0f)) {
    m_width       = width;
    m_height      = height;
    m_entity_type = type;
}


Entity::Entity(GLuint texture_id, float speed, float width, float height,
               EntityType type, AIType aiType, AIState aiState)
  : Entity(texture_id, speed, width, height, type) {
    m_ai_type  = aiType;
    m_ai_state = aiState;
}

Entity::~Entity() { }

void Entity::set_state_texture(PlayerState s, GLuint tex) {
    if (s >= 0 && s < STATE_COUNT) {
        m_state_texture[s] = tex;
    }
}

void Entity::set_player_state(PlayerState state) {
    m_player_state    = state;
    m_animation_index = 0;
    m_animation_time  = 0.0f;

    switch(state) {
        case IDLE:
            m_animation_indices = m_idle;
            m_animation_frames  = IDLE_FRAME_COUNT;
            m_animation_cols    = IDLE_FRAME_COUNT;
            m_animation_rows    = 1;
            break;
        case WALKING:
            m_animation_indices = m_walk;
            m_animation_frames  = WALK_FRAME_COUNT;
            m_animation_cols    = WALK_FRAME_COUNT;
            m_animation_rows    = 1;
            break;
        case ATTACKING:
            m_animation_indices = m_attack;
            m_animation_frames  = ATTACK_FRAME_COUNT;
            m_animation_cols    = ATTACK_FRAME_COUNT;
            m_animation_rows    = 1;
            break;
        case HURT:
            m_animation_indices = m_hurt;
            m_animation_frames  = HURT_FRAME_COUNT;
            m_animation_cols    = HURT_FRAME_COUNT;
            m_animation_rows    = 1;
            break;
        case DEATH:
            m_animation_indices = m_death;
            m_animation_frames  = DEATH_FRAME_COUNT;
            m_animation_cols    = DEATH_FRAME_COUNT;
            m_animation_rows    = 1;
            break;
    }
    m_texture_id = m_state_texture[state];
}


void Entity::ai_activate(Entity *player) {
    switch (m_ai_type) {
        case WALKER: ai_walk();  break;
        case GUARD:  ai_guard(player); break;
        default: break;
    }
}

void Entity::ai_walk() {
    m_velocity = glm::vec3(-1,0,0) * m_speed;
}

void Entity::ai_guard(Entity *player) {
    // todo
}

void Entity::update(float delta_time, Entity *player, Entity *collidables, int coll_count, Map* map) {
    
    if (m_entity_type == ENEMY) {
        
        if (m_animation_indices) {
            m_animation_time += delta_time;
            float frameTime = 1.0f / SECONDS_PER_FRAME;
            if (m_animation_time >= frameTime) {
                m_animation_time -= frameTime;
                m_animation_index = (m_animation_index + 1) % m_animation_frames;
            }
        }
    }
    
    if(m_entity_type == PLAYER){
        if(m_player_state == ATTACKING) {
            m_animation_time += delta_time;
            float frameTime = 1.0f / SECONDS_PER_FRAME;
            if (m_animation_time >= frameTime) {
                m_animation_time -= frameTime;
                m_animation_index++;
                // once we've shown all attack frames, go back to idle
                if (m_animation_index >= m_animation_frames) {
                    set_player_state(IDLE);
                }
            }
        }
        else if(m_player_state == HURT){
            m_animation_time += delta_time;
            float frameTime = 1.0f / SECONDS_PER_FRAME;
            if (m_animation_time >= frameTime) {
                m_animation_time -= frameTime;
                m_animation_index++;
                if (m_animation_index >= m_animation_frames) {
                    set_player_state(IDLE);
                }
            }
        }
        else{
            if (m_entity_type == PLAYER) {
                if (glm::length(m_velocity) > 0.0f) set_player_state(WALKING);
                else                               set_player_state(IDLE);
            }
            
            if (m_animation_indices && glm::length(m_velocity) > 0.0f) {
                m_animation_time += delta_time;
                float frameTime = 1.0f / SECONDS_PER_FRAME;
                if (m_animation_time >= frameTime) {
                    m_animation_time -= frameTime;
                    m_animation_index = (m_animation_index + 1) % m_animation_frames;
                }
            }
        }
    }
    


    
    m_position.x += m_velocity.x * delta_time;
    check_collision_x(map);

    m_position.y += m_velocity.y * delta_time;
    check_collision_y(map);



    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    m_model_matrix = glm::scale(
        m_model_matrix,
        glm::vec3(m_width * m_direction, m_height, 1.0f)
    );
}

void Entity::draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index)
{
    // Step 1: Calculate the UV location of the indexed frame
    float u_coord = (float)(index % m_animation_cols) / (float)m_animation_cols;
    float v_coord = (float)(index / m_animation_cols) / (float)m_animation_rows;

    // Step 2: Calculate its UV size
    float width = 1.0f / (float)m_animation_cols;
    float height = 1.0f / (float)m_animation_rows;

    // Step 3: Just as we have done before, match the texture coordinates to the vertices
    float tex_coords[] =
    {
        u_coord, v_coord + height, u_coord + width, v_coord + height, u_coord + width, v_coord,
        u_coord, v_coord + height, u_coord + width, v_coord, u_coord, v_coord
    };

    float vertices[] =
    {
        -0.5, -0.5, 0.5, -0.5,  0.5, 0.5,
        -0.5, -0.5, 0.5,  0.5, -0.5, 0.5
    };

    // Step 4: And render
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());

    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

void Entity::render(ShaderProgram* program) {
    program->set_model_matrix(m_model_matrix);
    if (m_animation_indices) {
        draw_sprite_from_texture_atlas(program, m_texture_id, m_animation_indices[m_animation_index]);
    } else {
        float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
        float tex_coords[] = { 0.0,  1.0, 1.0,  1.0, 1.0, 0.0,  0.0,  1.0, 1.0, 0.0,  0.0, 0.0 };

        glBindTexture(GL_TEXTURE_2D, m_texture_id);

        glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(program->get_position_attribute());
        glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
        glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glDisableVertexAttribArray(program->get_position_attribute());
        glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
    }
}

void const Entity::check_collision_y(Map *map)
{
    
    glm::vec3 effective_position = m_position + m_collision_offset;
    
    // Probes for tiles above
    glm::vec3 top = glm::vec3(effective_position.x, effective_position.y + (collision_height / 2), m_position.z);
    glm::vec3 top_left = glm::vec3(effective_position.x - (collision_width / 2), effective_position.y + (collision_height / 2), effective_position.z);
    glm::vec3 top_right = glm::vec3(effective_position.x + (collision_width / 2), effective_position.y + (collision_height / 2), effective_position.z);
    
    // Probes for tiles below
    glm::vec3 bottom = glm::vec3(effective_position.x, effective_position.y - (collision_height / 2), effective_position.z);
    glm::vec3 bottom_left = glm::vec3(effective_position.x - (collision_width / 2), effective_position.y - (collision_height / 2), effective_position.z);
    glm::vec3 bottom_right = glm::vec3(effective_position.x + (collision_width / 2), effective_position.y - (collision_height / 2), effective_position.z);
    
    float penetration_x = 0;
    float penetration_y = 0;
    
    // If the map is solid, check the top three points
    if (map->is_solid(top, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_left, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_right, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    
    // And the bottom three points
    if (map->is_solid(bottom, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_left, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
            m_position.y += penetration_y;
            m_velocity.y = 0;
            m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_right, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
        
    }
}

void const Entity::check_collision_x(Map *map)
{
    glm::vec3 effective_position = m_position + m_collision_offset;
    
    // Probes for tiles; the x-checking is much simpler
    glm::vec3 left  = glm::vec3(effective_position.x - (collision_width / 2), effective_position.y, effective_position.z);
    glm::vec3 right = glm::vec3(effective_position.x + (collision_width / 2), effective_position.y, effective_position.z);
    
    float penetration_x = 0;
    float penetration_y = 0;
    
    if (map->is_solid(left, &penetration_x, &penetration_y) && m_velocity.x < 0)
    {
        m_position.x += penetration_x;
        m_velocity.x = 0;
        m_collided_left = true;
    }
    if (map->is_solid(right, &penetration_x, &penetration_y) && m_velocity.x > 0)
    {
        m_position.x -= penetration_x;
        m_velocity.x = 0;
        m_collided_right = true;
    }
}

bool const Entity::check_collision(Entity* other) const {
    if(m_entity_type == PLAYER && (other->m_entity_type == ENEMY || other->m_entity_type == ITEM)){
        glm::vec3 this_pos = m_position + m_collision_offset;
           
       float x_distance = fabs(this_pos.x - (other->m_collision_offset.x + other->m_position.x)) - ((collision_width + other->collision_width) / 2.0f);
       float y_distance = fabs(this_pos.y - (other->m_collision_offset.y + other->m_position.y)) - ((collision_height + other->collision_height) / 2.0f);

       return x_distance < 0.0f && y_distance < 0.0f;
    }else{
        return false;
    }
}
//
//void const Entity::check_collision_x(<#Entity *collidable_entities#>, <#int collidable_entity_count#>){
//    glm::vec3 this_pos = m_position + m_collision_offset;
//      
//    for (int i = 0; i < collidable_entity_count; i++)
//    {
//      Entity *collidable_entity = &collidable_entities[i];
//      
//      if (check_collision(collidable_entity))
//      {
//          float x_distance = fabs(this_pos.x - (collidable_entity->m_position.x + collidable_entity->m_collision_offset.x));
//          float x_overlap = fabs(x_distance - (collision_width / 2.0f) - (collidable_entity->collision_width / 2.0f));
//          if (m_velocity.x > 0)
//          {
//              m_position.x     -= x_overlap;
//              m_velocity.x      = 0;
//
//              // Collision!
//              m_collided_right  = true;
//              
//          } else if (m_velocity.x < 0)
//          {
//              m_position.x    += x_overlap;
//              m_velocity.x     = 0;
//
//              // Collision!
//              m_collided_left  = true;
//          }
//      }
//    }
//}
//
//void const Entity::check_collision_y(<#Entity *collidable_entities#>, <#int collidable_entity_count#>){
//    glm::vec3 this_pos = m_position + m_collision_offset;
//    for (int i = 0; i < collidable_entity_count; i++)
//    {
//        Entity *collidable_entity = &collidable_entities[i];
//        
//        if (check_collision(collidable_entity))
//        {
//            float y_distance = fabs(this_pos.y - (collidable_entity->m_position.y + collidable_entity->m_collision_offset.y));
//            float y_overlap = fabs(y_distance - (collision_height / 2.0f) - (collidable_entity->collision_height / 2.0f));
//            if (m_velocity.y > 0)
//            {
//                m_position.y   -= y_overlap;
//                m_velocity.y    = 0;
//
//                // Collision!
//                m_collided_top  = true;
//            } else if (m_velocity.y < 0)
//            {
//                m_position.y      += y_overlap;
//                m_velocity.y       = 0;
//
//                // Collision!
//                m_collided_bottom  = true;
//            }
//        }
//    }
//}


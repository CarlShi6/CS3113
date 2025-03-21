/**
 * Author: Carl Shi
 * Assignment: Lunar Lander
 * Date due: 2025-3-21, 11:59pm
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

Entity::Entity()
{
    m_position = glm::vec3(0.0f);
    m_velocity = glm::vec3(0.0f);
    m_movement = glm::vec3(0.0f);
    m_acceleration = glm::vec3(0.0f);
    m_model_matrix = glm::mat4(1.0f);
    m_speed = 0;
    m_rotation = 0;
}

Entity::~Entity() { }

bool const Entity::check_collision(Entity* other) const
{
    if (!m_is_active || !other->m_is_active) return false;

    float xdist = fabs(m_position.x - other->m_position.x) - ((m_width + other->m_width) / 2.0f);
    float ydist = fabs(m_position.y - other->m_position.y) - ((m_height + other->m_height) / 2.0f);
    
    return (xdist < 0.0f && ydist < 0.0f);
    
}

CollisionType const Entity::check_collision_y(Entity* collidable_entities, int collidable_entity_count)
{
    CollisionType result = NOCOLLISION;
    
    for (int i = 0; i < collidable_entity_count; i++) {
        Entity* obj = &collidable_entities[i];
        if (check_collision(obj)) {
            // If we collide, figure out overlap
            float ydist = fabs(m_position.y - obj->m_position.y);
            float y_overlap = fabs(ydist - ((m_height + obj->m_height) / 2.0f));

            if (m_velocity.y > 0) {
                // Colliding from below
                m_position.y -= y_overlap;
                m_velocity.y = 0;
                m_collided_top = true;
            }
            else if (m_velocity.y < 0) {
                // Colliding from above
                m_position.y += y_overlap;
                m_velocity.y = 0;
                m_collided_bottom = true;
            }

            // Decide if it's WIN or LOST based on type
            if (obj->get_entity_type() == TARGET) {
                result = WIN;
            }
            else if (obj->get_entity_type() == TRAP){
                result = LOST;
            }
        }
    }
    return result;
}

CollisionType const Entity::check_collision_x(Entity* collidable_entities, int collidable_entity_count)
{
    CollisionType result = NOCOLLISION;
    
    for (int i = 0; i < collidable_entity_count; i++) {
        Entity* obj = &collidable_entities[i];
        if (check_collision(obj)) {
            float xdist = fabs(m_position.x - obj->m_position.x);
            float x_overlap = fabs(xdist - ((m_width + obj->m_width) / 2.0f));

            if (m_velocity.x > 0) {
                m_position.x -= x_overlap;
                m_velocity.x = 0;
                m_collided_right = true;
            }
            else if (m_velocity.x < 0) {
                m_position.x += x_overlap;
                m_velocity.x = 0;
                m_collided_left = true;
            }

            // Decide if it's WIN or LOST
            if (obj->get_entity_type() == TARGET) {
                result = WIN;
            }
            else if (obj->get_entity_type() == TRAP) {
                result = LOST;
            }
        }
    }
    return result;
}

CollisionType Entity::update(float delta_time, Entity* collidable_entities, int collidable_entity_count){
   
    if (!m_is_active) {
        return NOCOLLISION;
    }
    
    m_collided_top    = false;
    m_collided_bottom = false;
    m_collided_left   = false;
    m_collided_right  = false;
    
    m_velocity += m_acceleration * delta_time;
    m_position += m_velocity * delta_time;
    
    // Build model matrix
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    m_model_matrix = glm::rotate(m_model_matrix, glm::radians(m_rotation),
                                 glm::vec3(0.0f, 0.0f, 1.0f));
    m_model_matrix = glm::scale(m_model_matrix, glm::vec3(m_width, m_height, 1.0f));
    

    
    //Check Collision
    CollisionType y_collision = check_collision_y(collidable_entities, collidable_entity_count);
    CollisionType x_collision = check_collision_x(collidable_entities, collidable_entity_count);
    
    if (y_collision == LOST || x_collision == LOST) {
        return LOST;
    }
    if (y_collision == WIN || x_collision == WIN) {
        return WIN;
    }
    return NOCOLLISION;
    
}

void Entity::render(ShaderProgram* program)
{
    program->set_model_matrix(m_model_matrix);

    float vertices[] = { -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f };
    float tex_coords[] = { 0.0f,  1.0f, 1.0f,  1.0f, 1.0f, 0.0f,  0.0f,  1.0f, 1.0f, 0.0f,  0.0f, 0.0f };

    glBindTexture(GL_TEXTURE_2D, m_texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

void Entity::update_transform() {
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    m_model_matrix = glm::scale(m_model_matrix, glm::vec3(m_width, m_height, 1.0f));
    m_model_matrix = glm::rotate(m_model_matrix, glm::radians(m_rotation), glm::vec3(0.0f, 0.0f, 1.0f));
}


void Entity::consume_fuel(float amount){
    m_fuel -= amount;
        if(m_fuel < 0.0f) {
            m_fuel = 0.0f;
        }
}




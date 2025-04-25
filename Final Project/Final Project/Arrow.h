// Arrow.h
#pragma once

#include "glm/glm.hpp"
#include "Map.h"
#include "Entity.h"
#include <SDL_opengl.h>

class Arrow : public Entity {
public:
    Arrow(GLuint texture_id, float speed);
    void shoot(const glm::vec3 &start, const glm::vec3 &target);
    void update(float deltaTime, Map *map);
    bool check_collision(Entity* other) const;
    void deactivate() { set_collision_dimensions(0,0); Entity::deactivate(); }
    void render(ShaderProgram *program) const;
    bool isActive() const { return Entity::isActive(); }

private:
    glm::vec3 velocity;
    float     rotation = 0.0f;
    float     speed;
};

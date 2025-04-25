/**
* Author: Carl Shi
* Assignment: Final Project
* Date due: 4/25, 5:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/


#include "Arrow.h"
#include "glm/gtc/matrix_transform.hpp"
#include <cmath>

Arrow::Arrow(GLuint texture, float speed_)
  : Entity(texture, 0.0f, 1.0f, 0.3f, ITEM),
    speed(speed_) {
    set_collision_dimensions(1.0f, 0.3f);
    deactivate();
}

void Arrow::shoot(const glm::vec3 &start, const glm::vec3 &target) {
    set_position(start);
    glm::vec3 dir = glm::normalize(target - start);
    velocity = dir * speed;
    rotation = std::atan2(dir.y, dir.x);
    activate();
}

void Arrow::update(float deltaTime, Map *map) {
    if(!isActive()) return;
    glm::vec3 pos = get_position() + velocity * deltaTime;
    set_position(pos);
    float penX, penY;
    if(map->is_solid(pos, &penX, &penY)) deactivate();
}

bool Arrow::check_collision(Entity* other) const {
    if(!isActive()) return false;
    // AABB vs AABB
    auto ap = get_position();
    float aw = get_width() * 0.1;
    float ah = get_height() * 0.1;
    auto op = other->get_position();
    float ow = other->get_width() * 0.1;
    float oh = other->get_height() * 0.1;
    return  (ap.x + aw > op.x - ow) && (ap.x - aw < op.x + ow) &&
    +            (ap.y + ah > op.y - oh) && (ap.y - ah < op.y + oh);
}

void Arrow::render(ShaderProgram *program) const {
    if(!isActive()) return;
    glm::mat4 model(1.0f);
    model = glm::translate(model, get_position());
    model = glm::rotate(model, rotation, glm::vec3(0,0,1));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f ,1));
    program->set_model_matrix(model);

    float verts[] = { -0.5f,-0.5f,  0.5f,-0.5f,  0.5f,0.5f,
                      -0.5f,-0.5f,  0.5f,0.5f,  -0.5f,0.5f };
    float uvs[]   = { 0,1, 1,1, 1,0,  0,1, 1,0, 0,0 };
    glBindTexture(GL_TEXTURE_2D, get_texture_id());
    glVertexAttribPointer(program->get_position_attribute(),2,GL_FLOAT,false,0,verts);
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(),2,GL_FLOAT,false,0,uvs);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
    glDrawArrays(GL_TRIANGLES,0,6);
    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

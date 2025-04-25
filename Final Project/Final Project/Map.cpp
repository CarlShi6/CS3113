/**
* Author: Carl Shi
* Assignment: Final Project
* Date due: 4/25, 5:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/



#include "Map.h"

Map::Map(int width, int height, unsigned int *level_data, GLuint texture_id, float tile_size, int tile_count_x, int tile_count_y) :
m_width(width), m_height(height), m_level_data(level_data), m_texture_id(texture_id), m_tile_size(tile_size), m_tile_count_x(tile_count_x), m_tile_count_y(tile_count_y)
{
    build();
}

void Map::build()
{
    static const unsigned int CodeToTilesetIndex[] = {
        12, //floor 0
        2, //topwall 1
        41, //bot wall 2
        0, //left wall 3
        5, //right wall 4
        99, //key 5
        48, //inner vert left wall 6
        37, //inner hori wall 7
        47  //inner vert right wall 8
           //nothing 
    };
    
    for(int y=0; y<m_height; ++y) {
      for(int x=0; x<m_width; ++x) {
        unsigned int code = m_level_data[y*m_width + x];
        
        if (code == INNER_HORI ||
            code == INNER_VERT_LEFT ||
            code == INNER_VERT_RIGHT ||
            code == KEY){
            unsigned int tile = CodeToTilesetIndex[0];
            
            float u = (tile % m_tile_count_x) / float(m_tile_count_x);
            float v = (tile / m_tile_count_x) / float(m_tile_count_y);
            float tileW = 1.0f/float(m_tile_count_x);
            float tileH = 1.0f/float(m_tile_count_y);

            float x_offset = -m_tile_size/2, y_offset =  m_tile_size/2;
            float x_pos = x_offset + m_tile_size * x;
            float y_pos = y_offset + -m_tile_size * y;

            // push 6 verts for the quad...
            m_vertices.insert(m_vertices.end(), {
              x_pos,           y_pos,
              x_pos,           y_pos - m_tile_size,
              x_pos + m_tile_size, y_pos - m_tile_size,
              x_pos,           y_pos,
              x_pos + m_tile_size, y_pos - m_tile_size,
              x_pos + m_tile_size, y_pos
            });
            m_texture_coordinates.insert(m_texture_coordinates.end(), {
              u,         v,
              u,         v + tileH,
              u + tileW, v + tileH,
              u,         v,
              u + tileW, v + tileH,
              u + tileW, v
            });
            
        }else if(code == NOTHING){
            continue;
        }

        unsigned int tile = CodeToTilesetIndex[code];

        float u = (tile % m_tile_count_x) / float(m_tile_count_x);
        float v = (tile / m_tile_count_x) / float(m_tile_count_y);
        float tileW = 1.0f/float(m_tile_count_x);
        float tileH = 1.0f/float(m_tile_count_y);

        float x_offset = -m_tile_size/2, y_offset =  m_tile_size/2;
        float x_pos = x_offset + m_tile_size * x;
        float y_pos = y_offset + -m_tile_size * y;

        // push 6 verts for the quad...
        m_vertices.insert(m_vertices.end(), {
          x_pos,           y_pos,
          x_pos,           y_pos - m_tile_size,
          x_pos + m_tile_size, y_pos - m_tile_size,
          x_pos,           y_pos,
          x_pos + m_tile_size, y_pos - m_tile_size,
          x_pos + m_tile_size, y_pos
        });
        m_texture_coordinates.insert(m_texture_coordinates.end(), {
          u,         v,
          u,         v + tileH,
          u + tileW, v + tileH,
          u,         v,
          u + tileW, v + tileH,
          u + tileW, v
        });
      }
    }
    
    
    // The bounds are dependent on the size of the tiles
    m_left_bound   = 0 - (m_tile_size / 2);
    m_right_bound  = (m_tile_size * m_width) - (m_tile_size / 2);
    m_top_bound    = 0 + (m_tile_size / 2);
    m_bottom_bound = -(m_tile_size * m_height) + (m_tile_size / 2);
}

void Map::render(ShaderProgram *program)
{
    glm::mat4 model_matrix = glm::mat4(1.0f);
    program->set_model_matrix(model_matrix);
    
    glUseProgram(program->get_program_id());
    
    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, m_vertices.data());
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, m_texture_coordinates.data());
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
    
    glBindTexture(GL_TEXTURE_2D, m_texture_id);
    
    glDrawArrays(GL_TRIANGLES, 0, (int) m_vertices.size() / 2);
    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

bool Map::is_floor_tile(unsigned int tileIndex) {
    return tileIndex == 12;
}

bool Map::is_solid(glm::vec3 position, float *penetration_x, float *penetration_y)
{
    // The penetration between the map and the object
    // The reason why these are pointers is because we want to reassign values
    // to them in case that we are colliding. That way the object that originally
    // passed them as values will keep track of these distances
    // inb4: we're passing by reference
    
    *penetration_x = 0;
    *penetration_y = 0;
    
    // If we are out of bounds, it is not solid
    if (position.x < m_left_bound || position.x > m_right_bound)  return false;
    if (position.y > m_top_bound  || position.y < m_bottom_bound) return false;
    
    int tile_x = floor((position.x + (m_tile_size / 2))  / m_tile_size);
    int tile_y = -(ceil(position.y - (m_tile_size / 2))) / m_tile_size; // Our array counts up as Y goes down.
    
    // If the tile index is negative or greater than the dimensions, it is not solid
    if (tile_x < 0 || tile_x >= m_width)  return false;
    if (tile_y < 0 || tile_y >= m_height) return false;
    
    // If the tile index is 0 i.e. an open space, it is not solid
    unsigned int code = m_level_data[tile_y*m_width + tile_x];
    
    if (code == INNER_VERT_LEFT || code == INNER_VERT_RIGHT) {
        float tile_center_x = tile_x * m_tile_size;
        float dx = position.x - tile_center_x;

        float half = m_tile_size/2;
        float localX = dx + half;

        const float strip = m_tile_size * 0.3f;
        if (code == INNER_VERT_LEFT) {
            if (localX <= strip) {
                *penetration_x = strip - localX;
                return true;
            }
        } else {
            if (localX >= (m_tile_size - strip)) {
                *penetration_x = localX - (m_tile_size - strip);
                return true;
            }
        }
        return false;
    }
    
    switch (code) {
        case TOP_WALL:
        case BOT_WALL:
        case LEFT_WALL:
        case RIGHT_WALL:
        case INNER_VERT_LEFT:
        case INNER_VERT_RIGHT:
        case INNER_HORI:
            break;
        default:
            return false;
    }
    
    
    
    // And we likely have some overlap
    float tile_center_x = (tile_x  * m_tile_size);
    float tile_center_y = -(tile_y * m_tile_size);
    
    // And because we likely have some overlap, we adjust for that
    *penetration_x = (m_tile_size / 2) - fabs(position.x - tile_center_x);
    *penetration_y = (m_tile_size / 2) - fabs(position.y - tile_center_y);
    
    return true;
}



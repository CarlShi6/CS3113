#include "Scene.h"

class LevelA : public Scene {
    
private:
    GLuint    m_heart_texture_id;
    GLuint    m_key_texture_id;
    Entity*   m_key;
    Entity*   m_door;
    bool      m_keySpawned;
    bool      m_keyCollected;
    int       enemy_count;
public:
    // ————— STATIC ATTRIBUTES ————— //
    int ENEMY_COUNT = 1;
    
    // ————— DESTRUCTOR ————— //
    ~LevelA();
    
    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
};

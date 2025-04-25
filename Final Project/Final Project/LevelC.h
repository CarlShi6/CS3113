

#define GL_SILENCE_DEPRECATION
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#define GL_GLEXT_PROTOTYPES 1

#include "Scene.h"
#include "Entity.h"
#include "Arrow.h"
#include "ShaderProgram.h"

class LevelC : public Scene {
private:
    // ——— Textures ———
    GLuint m_heart_texture_id;
    GLuint m_key_texture_id;

    // ——— Pickups & Door ———
    Entity* m_key;
    Entity* m_bottle;
    Entity* m_door;

    // ——— Arrow ———
    Arrow* m_guardArrows[4];
    float   m_guardShotTimers[4];

    // ——— State flags ———
    bool m_keySpawned = false;
    bool m_keyCollected = false;
    bool m_bottleSpawned = false;
    bool m_doorSpawned = false;

    // ——— Enemy & shooting timer ———
    int   enemy_count;
    float m_guardLastShot;

public:
    // ——— Constants ———
    static constexpr int   ENEMY_COUNT = 6;
    static constexpr float GUARD_SHOOT_INTERVAL = 2.0f; // seconds
    
    ShaderProgram* m_spot_prog;

    // ——— Lifecycle ———
    ~LevelC();

    // ——— Scene interface ———
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
    

};

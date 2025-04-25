

#define GL_SILENCE_DEPRECATION
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#define GL_GLEXT_PROTOTYPES 1

#include "Scene.h"
#include "Entity.h"
#include "Arrow.h"

class LevelB : public Scene {
private:
    // ——— Textures ———
    GLuint m_heart_texture_id;
    GLuint m_key_texture_id;

    // ——— Pickups & Door ———
    Entity* m_key;
    Entity* m_bottle;
    Entity* m_door;

    // ——— Arrow ———
    Arrow* m_arrow;

    // ——— State flags ———
    bool m_keySpawned;
    bool m_keyCollected;
    bool m_bottleSpawned;
    bool m_doorSpawned;
    bool m_wispSpawned;

    // ——— Enemy & shooting timer ———
    int   enemy_count;
    float m_guardLastShot;

public:
    // ——— Constants ———
    static constexpr int   ENEMY_COUNT = 2;
    static constexpr float GUARD_SHOOT_INTERVAL = 2.0f; // seconds

    // ——— Lifecycle ———
    ~LevelB();

    // ——— Scene interface ———
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
    
    bool      m_showWispCard     = false;
    GLuint    m_wispCardTexture;
    bool m_wispCardAlreadyShown = false;
    
    int    m_screenWidth  = 1280;   // <- replace with your real width
    int    m_screenHeight =  720;
};

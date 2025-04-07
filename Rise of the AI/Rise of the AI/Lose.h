#include "Scene.h"

class Lose : public Scene {
public:
    GLuint m_font_texture;
    
    ~Lose();

    virtual void initialise() override;
    virtual void update(float delta_time) override;
    virtual void render(ShaderProgram *program) override;
};

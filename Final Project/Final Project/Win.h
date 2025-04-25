#include "Scene.h"

class Win : public Scene {
public:
    GLuint m_font_texture;
    
    ~Win();

    virtual void initialise() override;
    virtual void update(float delta_time) override;
    virtual void render(ShaderProgram *program) override;
};


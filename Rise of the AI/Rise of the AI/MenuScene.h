#include "Scene.h"

class MenuScene : public Scene {
public:
    GLuint m_font_texture;
    
    ~MenuScene();

    virtual void initialise() override;
    virtual void update(float delta_time) override;
    virtual void render(ShaderProgram *program) override;
};

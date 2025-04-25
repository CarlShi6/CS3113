#include "Scene.h"

class Menu : public Scene {
public:
    GLuint m_font_texture;
    
    ~Menu();

    virtual void initialise() override;
    virtual void update(float delta_time) override;
    virtual void render(ShaderProgram *program) override;

    
    GLuint  m_bg_texture     = 0;
    GLuint  m_vao            = 0;

    void    draw_background(ShaderProgram *program);
};

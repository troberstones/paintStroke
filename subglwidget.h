#ifndef SUBGLWIDGET_H
#define SUBGLWIDGET_H
#include "glwidget.h"

class QOpenGLShaderProgram;

class subGLWidget : public GLWidget
{
public:
    subGLWidget(QScreen *screen);

    virtual void initialize();
    virtual void render();

private:
    GLuint loadShader(GLenum type, const char *source);

    GLuint m_posAttr;
    GLuint m_uvTiltAttr;
    GLuint m_pressureAttr;
    GLuint m_colorAttr;
    GLuint m_angleAttr;
    GLuint m_speedAttr;

    GLuint m_matrixUniform;
    GLuint m_textureUniform;

    QOpenGLShaderProgram *m_program;
    int m_frame;
};

#endif // SUBGLWIDGET_H

#include "subglwidget.h"
#include <QOpenGLShaderProgram>
#include "strokedb.h"


#if 0
static const char *vertexShaderSource =
    "#version 330 core\n"
    "attribute highp vec4 posAttr;\n"
    "attribute lowp vec2 colAttr;\n"
    "varying lowp vec4 col;\n"
    "uniform highp mat4 matrix;\n"
    "void main() {\n"
    "   col = vec4(colAttr.x, colAttr.y, 0.f, 1.f);\n"
    //"   col = vec4(1.f,0.f,0.f,1.f);\n"
    "   gl_Position = matrix * posAttr;\n"
    "}\n";

static const char *fragmentShaderSource =
    "#version 330 core\n"
    "varying lowp vec4 col;\n"
    "uniform sampler2D topMap;\n"
    "void main() {\n"
    "   gl_FragColor = col;\n"
    "   gl_FragColor = vec3(texture(topMap, vec2(col.r,col.g)).r);\n"
    "   gl_FragColor.a = 1.f;\n"
    "}\n";
#endif


static const char *vertexShaderSource =
    "#version 330 core\n"
    "attribute highp vec4 posAttr;\n"
    "attribute lowp vec4 uvTiltAttr;\n"
    "varying lowp vec4 uvTilt;\n"
    "uniform highp mat4 matrix;\n"
    "void main() {\n"
    "   uvTilt = uvTiltAttr;\n"
    "   gl_Position = matrix * posAttr;\n"
    "}\n";

static const char *fragmentShaderSource =
        "#version 330 core\n"
        "varying lowp vec4 uvTilt;\n"
        "uniform sampler2D topMap;\n"
        "void main() {\n"
        "    vec4 tex = texture(topMap, vec2(uvTilt.r,uvTilt.g));"
        "    float res = tex.r * uvTilt.b + tex.b * (1.f - uvTilt.b);"
        "    gl_FragColor = vec4(0.f);\n"
        //"   gl_FragColor = vec4(texture(topMap, vec2(uvTilt.r,uvTilt.g)).r);\n"
        //"  gl_FragColor = vec4(uvTilt.b,uvTilt.a,0.f,0.f);"
        "    gl_FragColor.a = 1.f-res;\n"
        "}\n";

subGLWidget::subGLWidget(QScreen *screen)
    : GLWidget(screen)
    , m_frame(0)

{
    qWarning() << "new subGLWidget";
}

GLuint subGLWidget::loadShader(GLenum type, const char *source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, 0);
    glCompileShader(shader);
    return shader;
}

void subGLWidget::initialize()
{
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();
    m_posAttr = m_program->attributeLocation("posAttr");
    m_uvTiltAttr = m_program->attributeLocation("uvTiltAttr");
    m_matrixUniform = m_program->uniformLocation("matrix");
    m_textureUniform = m_program->attributeLocation("topMap");

    //Load texture
    //m_strokes->initTextures("C:\\Users\\Chris\\Google Drive\\qtdev\\paintStroke\\verticalPenStrokeLitePressure_1.tif");
    //m_strokes->initTextures("C:\\Users\\Chris\\Google Drive\\qtdev\\paintStroke\\testimage.png");
    //m_strokes->initTextures("C:\\Users\\Chris\\Google Drive\\qtdev\\paintStroke\\testimage2.png");
    m_strokes->initTextures("C:\\Users\\Chris\\Google Drive\\qtdev\\paintStroke\\wrapableStrokes_RGBWidened_v01.png");
   // m_shaderProgram->setUniformValue("topMap", m_strokes->m_textures[0]);
}

void subGLWidget::render()
{
    glViewport(0, 0, width(), height());
    glClearColor(.8f,.8f,.8f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    // XXX test this 2 lines
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#define DRAWLINES 0
#if DRAWLINES
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#else
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
    m_program->bind();

    QMatrix4x4 matrix;
    matrix.ortho(-.5f,.5f,-.5f,.5f,0.f,10.f);
    matrix.translate(0, 0, -2);

    m_program->setUniformValue(m_matrixUniform, matrix);

    glActiveTexture(GL_TEXTURE0);
    checkGL();
    glBindTexture(GL_TEXTURE_2D, m_strokes->m_textures[0]);
    checkGL();
    m_program->setUniformValue(m_textureUniform, m_strokes->m_textures[0]);
    checkGL();

    for(int i = 0; i < m_strokes->m_strokes.length(); i++)
    {
#ifdef RENDERCENTERLINE
        const Strips *curStrip = m_strokes->GetStripAt(i);
        int strokeLen = (*curStrip).vertices.length();
        glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, (*curStrip).vertices.constData());
        //glVertexAttribPointer(m_posAttr, 2, GL_FLOAT, GL_FALSE, 0, vertices);
        //glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, colors);
        checkGL();
        glEnableVertexAttribArray(0);
        //glEnableVertexAttribArray(1);

        checkGL();
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawArrays(GL_LINE_STRIP, 0, strokeLen);
        checkGL();

        glDisableVertexAttribArray(1);
        //glDisableVertexAttribArray(0);
        checkGL();
#else
        Strips *curStrip = m_strokes->GetStripAt(i);
        curStrip->processStroke();
        int strokeLen = (*curStrip).processedVerts.length();
        glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, (*curStrip).processedVerts.constData());
        glVertexAttribPointer(m_uvTiltAttr, 4, GL_FLOAT, GL_FALSE, 0, (*curStrip).processedUVs.constData());
        checkGL();
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        checkGL();
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawArrays(GL_QUAD_STRIP, 0, strokeLen);
        checkGL();

        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);
        checkGL();
#endif
    }

    m_program->release();
    checkGL();

    ++m_frame;
}

#include "testglwidget.h"
#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLFunctions>
#include <QOpenGLContext>


static const GLfloat g_vertex_buffer_data[] = {
   -1.0f, -1.0f, 0.0f,
   1.0f, -1.0f, 0.0f,
   0.0f,  1.0f, 0.0f,
};

TestGlWidget::TestGlWidget(QScreen *parent) :
    QWindow(parent)
{
    setSurfaceType(OpenGLSurface);

   QSurfaceFormat format;

   format.setDepthBufferSize(24);
   format.setMajorVersion(4);
   format.setMinorVersion(3);
   format.setSamples(4);
   format.setProfile(QSurfaceFormat::CoreProfile);

   setFormat(format);
   create();
   m_glctx = new QOpenGLContext;
   m_glctx->setFormat(format);
   m_glctx->create();
#if 0
   m_funcs = static_cast<QOpenGLFunctions>(m_glctx->versionFunctions());
   if(!m_funcs)
   {
       qWarning("Couldn't optain OpenGL version object");
       exit(1);
   }
   m_funcs->initializeOpenGLFunctions();
#endif
   initializeGL();
   paintGL();
}

void TestGlWidget::initializeGL()
{
   m_glctx->makeCurrent(this);

#if 0
   GLuint vertexbuffer;
   glGenBuffers(1, &vertexbuffer);
   glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
   glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
#endif
   m_vao1 = new QOpenGLVertexArrayObject(this);
   m_vao1->create();
   m_vao1->bind();

}

void TestGlWidget::paintGL()
{
    m_glctx->makeCurrent(this);

#if 0
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
       0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
       3,                  // size
       GL_FLOAT,           // type
       GL_FALSE,           // normalized?
       0,                  // stride
       (void*)0            // array buffer offset
    );

    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle

    glDisableVertexAttribArray(0);
#endif

    m_glctx->swapBuffers(this);
    m_glctx->doneCurrent();
}


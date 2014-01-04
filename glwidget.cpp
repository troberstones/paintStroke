#include "glwidget.h"
#include "strokedb.h"
#include <QtOpenGL>
#include <cstring>
#include <iostream>
#include <QSurfaceFormat>

GLWidget::GLWidget(QScreen *screen)
    : QWindow(screen)
    , m_brushBufferCount(0)
    , m_brushBufferAmt(3)
    , m_brushCacheX(0)
    , m_brushCacheY(0)
    , m_brushCachePressure(0)
    //, m_vertexBuffer(QOpenGLBuffer::VertexBuffer)
    , m_vao(0)

    , m_update_pending(0)
    , m_animating(0)
    , m_context(0)
    , m_device(0)


{
    setSurfaceType(QWindow::OpenGLSurface);
    printf("GL widget ctor");
#if 1
    QSurfaceFormat glFormat;
    glFormat.setDepthBufferSize(24);

    // Causing the frag and vert shaders to fail to compile
    //glFormat.setMajorVersion(3);
    //glFormat.setMinorVersion(3);

    glFormat.setSamples(4);
    glFormat.setProfile(QSurfaceFormat::CoreProfile);
    setFormat(glFormat);
    create();
#endif

    std::cout << "Init width:" << this->width() << "Height:" << this->height() << std::endl;

    setStrokeMapping(500, 500);

    myTabletDevice = QTabletEvent::Stylus;

    m_shaderProgram = NULL;
    m_vertShader = NULL;
    m_fragShader = NULL;
}

GLWidget::~GLWidget()
{
    printf("GL widget dtor");
    if(m_strokes)
    {
        delete m_strokes;
        m_strokes = NULL;
    }
    if(m_shaderProgram)
    {
        delete m_shaderProgram;
        m_shaderProgram = NULL;
    }

    if(m_fragShader)
    {
        delete m_fragShader;
        m_fragShader = NULL;
    }

    if(m_vertShader)
    {
        delete m_vertShader;
        m_vertShader = NULL;
    }
    if(m_vao)
    {
        delete m_vao;
        m_vao = NULL;
    }
    if(m_context)
    {
        delete m_context;
        m_context = NULL;
    }
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(500, 500);
}

void GLWidget::initialize()
{
    initializeGL();
}

void GLWidget::render(QPainter *painter)
{
    Q_UNUSED(painter);
    qWarning() << "render";
    paintGL();
}

void GLWidget::render()
{
    paintGL();
#if 0
    if (!m_device)
        m_device = new QOpenGLPaintDevice;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    m_device->setSize(size());

    QPainter painter(m_device);
    render(&painter);
#endif
}

void GLWidget::setStrokeMapping(int width, int height)
{
   m_xmapping = 1.f / (float)width;
   m_ymapping = 1.f / (float)height;
}

// Accumulate the stroke positions and average them, when enough
// data is accumulated, append the point to the stroke
void GLWidget::appendPositionToStroke(int x, int y, float pressure, float tiltX, float tiltY)
{
    int distX = x - m_lastX;
    int distY = y - m_lastY;

    //std::cout << "IntDist:" <<  distX * distX + distY * distY << std::endl;
    if((distX * distX + distY * distY) == 0 )
        return;

    m_brushBufferCount++;
    m_lastX = x;
    m_lastY = y;

    float xpos = x * m_xmapping;
    float ypos = y * m_ymapping;
    xpos -= .5f;
    ypos = 1.f - ypos;
    ypos -= .5f;
    float xdist = xpos - m_brushCacheX;
    float ydist = ypos - m_brushCacheY;
    // If the brush point is to close to the previous point
    // average it
    m_brushCacheX += xpos;
    m_brushCacheY += ypos;
    m_brushCachePressure += pressure;
    m_brushCacheTiltX += (float)tiltX/60.f;
    m_brushCacheTiltY += (float)tiltX/60.f;
    //float dist = xdist* xdist + ydist * ydist;
    //std::cout << "Dist: " << dist << std::endl;
    if( xdist* xdist + ydist * ydist > .2)
    {
        //std::cout << "xpos:" << xpos << " ypos:" << ypos << std::endl;
        flushCaches();
    }

#if 0
    if(m_brushBufferCount >= m_brushBufferAmt)
    {
        flushCaches();
    }
#endif
}

// Flush any remaining stroke positions and start a new stroke
void GLWidget::flushAndStartNewStroke()
{
    flushCaches();
    m_strokes->startNewStroke();
}

// reset the cache buffers and append the point to the stroke
void GLWidget::flushCaches()
{
    // m_brushBufferCount was for point averaging.
    float inv = 1.f/m_brushBufferCount;
    m_strokes->appendPositionToStroke(m_brushCacheX * inv,
                                      m_brushCacheY * inv,
                                      m_brushCachePressure * inv,
                                      m_brushCacheTiltX * inv,
                                      m_brushCacheTiltY * inv);
    m_brushCacheX = 0;
    m_brushCacheY = 0;
    m_brushCacheTiltX = 0;
    m_brushCacheTiltY = 0;
    m_brushCachePressure = 0;
    m_brushBufferCount = 0;
    renderLater();

}

void GLWidget::SetStrokeDB(StrokeDB *strokeDB)
{
    m_strokes = strokeDB;
}

bool GLWidget::LoadShader(QString vshader, QString fshader)
{
    bool success = false;
    if(m_shaderProgram)
    {
        m_shaderProgram->release();
        m_shaderProgram->removeAllShaders();
    }
    else
        m_shaderProgram = new QGLShaderProgram;

    if(m_vertShader)
    {
        delete m_vertShader;
        m_vertShader = NULL;
    }
    if(m_fragShader)
    {
        delete m_fragShader;
        m_fragShader = NULL;
    }
    QFileInfo vsh(vshader);
    if(vsh.exists())
    {
        m_vertShader = new QGLShader(QGLShader::Vertex);
        if(m_vertShader->compileSourceFile(vshader))
            m_shaderProgram->addShader(m_vertShader);

        success = true;
    }
    else
        qWarning() << "vertex shader source file " << vshader << "not found";
    QFileInfo fsh(fshader);
    if(fsh.exists())
    {
        m_fragShader = new QGLShader(QGLShader::Fragment);
        if(m_fragShader->compileSourceFile(fshader))
            m_shaderProgram->addShader(m_fragShader);

        success = true;
    }
    else
        qWarning() << "fragment shader source file " << fshader << "not found";
    if(!m_shaderProgram->link())
    {
        success = false;
        qWarning() << "Shader program link error" << m_shaderProgram->log();
    }
    else
        m_shaderProgram->bind();

    return success;
}

void GLWidget::renderLater()
{
    if (!m_update_pending) {
        m_update_pending = true;
        QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
    }
}

void GLWidget::renderNow()
{
    if (!isExposed())
        return;

    m_update_pending = false;

    bool needsInitialize = false;

    if (!m_context) {
#if DEBUGGL
        qWarning() << "setup debug gl context";
        QSurfaceFormat format;
        // asks for a OpenGL 3.2 debug context using the Core profile
        format.setMajorVersion(3);
        format.setMinorVersion(2);
        format.setProfile(QSurfaceFormat::CoreProfile);
        format.setOption(QSurfaceFormat::DebugContext);

        m_context= new QOpenGLContext;
        m_context->setFormat(format);
        m_context->create();
        m_context->makeCurrent(this);
#else

        qWarning() << "setup gl context";
        m_context = new QOpenGLContext(this);
        m_context->setFormat(requestedFormat());
        m_context->create();
        m_context->makeCurrent(this);
#endif
        needsInitialize = true;
    }

    m_context->makeCurrent(this);

    if (needsInitialize) {
        initializeOpenGLFunctions();
        std::cout <<"A:" << glGetString(GL_VERSION) << std::endl;
        initializeGL();
        initialize();
    }

    render();

    m_context->swapBuffers(this);

    if (m_animating)
        renderLater();
}



void GLWidget::onMessageLogged(QOpenGLDebugMessage msg)
{
    qDebug() << msg;
}

void GLWidget::setPointAveraging(int count)
{
   m_brushBufferAmt = count;
}

void GLWidget::clearStrokes()
{
    m_strokes->clearStrokes();
    renderLater();
}
void GLWidget::checkGL()
{
#if DEBUGGL
    QList<QOpenGLDebugMessage> messages = m_logger->loggedMessages();
    foreach (const QOpenGLDebugMessage &message, messages)
        qDebug() << message;
#endif
}

void GLWidget::initializeGL()
{
    qWarning() <<"initializeGL";

#if DEBUGGL
    m_logger = new QOpenGLDebugLogger( this );
    connect( m_logger, SIGNAL( messageLogged( QOpenGLDebugMessage ) ),
             this, SLOT( onMessageLogged( QOpenGLDebugMessage ) ),
             Qt::DirectConnection );

    if ( m_logger->initialize() ) {
        //m_logger->startLogging( QOpenGLDebugLogger::SynchronousLogging );
        m_logger->enableMessages();
    }
    checkGL();

#endif
    return;

    glClearColor(.8f,.8f,.8f, 1.f);

    if(!LoadShader("C:\\Users\\Chris\\Google Drive\\qtdev\\paintStroke\\basic.vert",
                   "C:\\Users\\Chris\\Google Drive\\qtdev\\paintStroke\\basic.frag"))
    {
        qWarning() << "Couldn't load shaders";
        return;
    }
    checkGL();

    // Create a new vertex array object
    // This acts as a gl state storage for fast sw
    m_vao = new QOpenGLVertexArrayObject( this );
    m_vao->create();
    m_vao->bind();

    float points[] = {-1.f, 0.f, 0.f, 1.f, 0.f, 0.f,
                      0.f, 1.f, 0.f, 1.f, .5f, 1.f,
                      1.f, 0.f, 0.f, 1.f, 1.f, 0.f ,
                      -1.f, -.01f, 0.f, 1.f, 0.f, 0.f,
                       0.f, -1.f, 0.f, 1.f, .5f, 1.f,
                       1.f, -.01f, 0.f, 1.f, 1.f, 0.f};
//    float points[] = { -0.5f, -0.5f, 0.0f, 1.0f,
//                            0.5f, -0.5f, 0.0f, 1.0f,
//                            0.0f,  0.5f, 0.0f, 1.0f };

    //m_strokes->SetupBuffers();

    QOpenGLBuffer *tmp = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    tmp->create();
    tmp->setUsagePattern(QOpenGLBuffer::StaticDraw);
    if(!tmp->bind())
    {
        qWarning() << "Couldn't bind vertex buffer to content";
        return;
    }
    GLuint bufferOffset = 4 * sizeof( float );
    GLuint bufferStride = 6 * sizeof( float );

    tmp->allocate( points, 6 * bufferStride);
    m_vertexBuffer.push_back(tmp);

    checkGL();

    m_strokes->initTextures("C:\\Users\\Chris\\Google Drive\\qtdev\\paintStroke\\verticalPenStrokeLitePressure_1.tif");
    checkGL();


    //Call strokes to set up VBO array for each stroke

    m_shaderProgram->setAttributeBuffer( "vertex", GL_FLOAT, 0, 4, bufferStride );
    m_shaderProgram->enableAttributeArray( "vertex" );

    m_shaderProgram->setAttributeBuffer( "UV", GL_FLOAT, bufferOffset, 2, bufferStride );
    m_shaderProgram->enableAttributeArray( "UV" );
    m_shaderProgram->setUniformValue("topMap", m_strokes->m_textures[0]);
    //LoadShader("C:\\Users\\Chris\\Google Drive\\qtdev\\paintStroke\\basic.vert",
    //           "C:\\Users\\Chris\\Google Drive\\qtdev\\paintStroke\\basic.frag");

    checkGL();
#if 0

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, m_strokes->m_textures[0]);
    m_shaderProgram->setUniformValue("topMap", m_strokes->m_textures[0]);

#if 0
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_MULTISAMPLE);
    static GLfloat lightPosition[4] = { 0.5, 5.0, 7.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
#endif

#endif
}

void GLWidget::paintGL()
{
    qWarning() << "PaintGL called";
    return;
    checkGL();
    glViewport(0,0,width(),height());

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, m_strokes->m_textures[0]);
    m_shaderProgram->setUniformValue("topMap", m_strokes->m_textures[0]);

    std::cout << "numb of vertex buffers!" << m_vertexBuffer.size() << std::endl;
    for(int i = 0; i < m_vertexBuffer.size(); ++i)
    {
        std::cout << "Hello!" << i << std::endl;
        m_vertexBuffer[i]->bind();
        std::cout << "glwidget buffer id:" << m_vertexBuffer[i]->bufferId() << std::endl;
        glDrawArrays(GL_TRIANGLES, 0, 6);
        //std::cout << glDebug()
        checkGL();
    }

    //m_strokes->TestDrawBuffers();
//    glDrawArrays(GL_TRIANGLES, 0, 6);


    // Call strokes db to draw the arrays
    //m_strokes->DrawStrokes();

    //glDrawElements(GL_TRIANGLES,1);
    return;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -12.0);
    //glRotatef(xRot / 16.0, 1.0, 0.0, 0.0);
    //glRotatef(yRot / 16.0, 0.0, 1.0, 0.0);
    //glRotatef(zRot / 16.0, 0.0, 0.0, 1.0);

    //glRotatef(45.f, 0,0,1.f);

        checkGL();
#ifdef DEBUG
    glColor3f(0.0f, 1.0f, 0.0f);
    glRectf(-0.2f,0.1f, 0.1f, -0.1f);

    glColor3f(1.0f, 1.0f, 0.0f);
    glRectf(-0.1f,0.1f, 0.1f, -0.1f);
#endif

#if 0
    glPointSize(2.f);
    glBegin(GL_POINTS);
    for(float y = -5; y < 5; y+=.1f)
        for(float x = -5; x < 5; x+=.1f)
            glVertex2d(x*.1f,y*.1f);
    glEnd();
#endif

#if 0
    glPointSize(5.f);
    glBegin(GL_POINTS);
        glColor3f(1.f,0.f,0.f);
        glVertex3f((m_hovX*m_xmapping)-.5f, (1-(m_hovY * m_ymapping))-.5f, 0.f);
    glEnd();
#endif

    m_strokes->DrawStrokes();
        checkGL();
}

void GLWidget::resizeGL(int width, int height)
{
    glViewport(0,0,width,height);
# if 0
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#ifdef QT_OPENGL_ES_1
    glOrthof(-0.5, +0.5, -0.5, +0.5, 4.0, 15.0);
#else
    glOrtho(-0.5, +0.5, -0.5, +0.5, 4.0, 15.0);
#endif
    glMatrixMode(GL_MODELVIEW);
#endif
}



void GLWidget::keyPressEvent(QKeyEvent *e)
{
   switch (e->key())
   {
   case Qt::Key_Escape:
       QCoreApplication::instance()->quit();
       break;
   default:
       QWindow::keyPressEvent(e);
   }
}

bool GLWidget::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::UpdateRequest:
        renderNow();
        return true;
    default:
        return QWindow::event(event);
    }
}

void GLWidget::exposeEvent(QExposeEvent *event)
{
    Q_UNUSED(event);

    if (isExposed())
        renderNow();
}

void GLWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    if (isExposed())
    {
        renderNow();
    }
}

void GLWidget::tabletEvent(QTabletEvent *event)
{

    switch (event->type()) {
        case QEvent::TabletPress:
            if (!m_deviceDown) {
                m_deviceDown = true;
                flushAndStartNewStroke();
                //std::cout << event->pos().x() << " " << event->pos().y() << std::endl;
            }
            break;
        case QEvent::TabletRelease:
            if (m_deviceDown)
                m_deviceDown = false;
                flushAndStartNewStroke();
            break;
        case QEvent::TabletMove:
        if(event->pressure() != 0)
        {
            //std::cout << event->pos().x() << " , " << event->pos().y() << " p:" << event->pressure()<< std::endl;
            m_hovX = event->pos().x();
            m_hovY = event->pos().y();
            if(m_deviceDown)
            {
                appendPositionToStroke(  event->pos().x()
                                       , event->pos().y()
                                       , event->pressure()
                                       , event->xTilt()
                                       , event->yTilt()
                                         );
                renderLater();
            }

        }
        break;
        default:
            break;
    }
    //update();
}




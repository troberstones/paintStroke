#ifndef GLWIDGET_H
#define GLWIDGET_H

//#include <QtWidgets>
//#include <QtOpenGL>
#include <QWindow>
#include <QOpenGLContext>

//#include <QGLWidget>
//#include <QGLBuffer>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QGLShaderProgram>

#include <QTabletEvent>
#include <QOpenGLDebugLogger>
#include <QOpenGLFunctions>
#include <QOpenGLPaintDevice>
#include <QAccessibleWidget>

#define DEBUGGL 0
class StrokeDB;
//class QGLShaderProgram;
//class QGLShader;

class GLWidget : public QWindow, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit GLWidget( QScreen *screen = 0);
    ~GLWidget();
    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    virtual void initialize();
    virtual void render(QPainter *painter);
    virtual void render();

    void setTabletDevice(QTabletEvent::TabletDevice device)
        { myTabletDevice = device; }

    void setStrokeMapping(int width, int height);
    void appendPositionToStroke(int x, int y, float pressure, float tiltX, float tiltY);
    void flushAndStartNewStroke();
    void flushCaches();

    void SetStrokeDB(StrokeDB *strokeDB);
    bool LoadShader(QString vshader, QString fshader);

signals:

public slots:
    void renderLater();
    void renderNow();

    void onMessageLogged(QOpenGLDebugMessage msg);

    void setPointAveraging(int);
    void clearStrokes();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    virtual void keyPressEvent(QKeyEvent *e);
    virtual bool event(QEvent *event);
    void exposeEvent(QExposeEvent *event);
    void resizeEvent(QResizeEvent *event);
    void tabletEvent(QTabletEvent *event);

    void checkGL();

protected:
    StrokeDB *m_strokes;
    int m_brushBufferCount;
    int m_brushBufferAmt;

private:
    QTabletEvent::PointerType pointerType;
    QTabletEvent::TabletDevice myTabletDevice;
    bool m_deviceDown;


    float m_brushCacheX;
    float m_brushCacheY;
    float m_brushCacheTiltX;
    float m_brushCacheTiltY;
    float m_brushCachePressure;
    float m_xmapping;
    float m_ymapping;
    int m_lastX, m_lastY;
    int m_hovX, m_hovY;

    bool m_update_pending;
    bool m_animating;

    QOpenGLContext *m_context;
    QOpenGLPaintDevice *m_device;
    QOpenGLDebugLogger *m_logger;

    QGLShaderProgram *m_shaderProgram;
    QGLShader *m_vertShader, *m_fragShader;
    std::vector<QOpenGLBuffer*> m_vertexBuffer;
    GLfloat *m_testVbo;
    QOpenGLVertexArrayObject *m_vao;
};

#endif // GLWIDGET_H

#ifndef TESTGLWIDGET_H
#define TESTGLWIDGET_H
#include <QWindow>
#include <QOpenGLVertexArrayObject>

class QOpenGLFunctions;
class QOpenGLContext;
class StrokeDB;
class TestGlWidget : public QWindow
{
    Q_OBJECT
public:
    explicit TestGlWidget(QScreen *parent);

    void initializeGL();
    void paintGL();

signals:

public slots:

private:
    QOpenGLContext *m_glctx;
    QOpenGLVertexArrayObject *m_vao1;

    void *m_funcs;
    StrokeDB *m_strokes;
};

#endif // TESTGLWIDGET_H

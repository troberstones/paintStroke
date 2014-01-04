#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class TestGlWidget;
class GLWidget;
class StrokeDB;
class subGLWidget;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool event(QEvent *event);
private:
    Ui::MainWindow *ui;
    //GLWidget *m_glwidget;
    subGLWidget *m_glwidget;
    TestGlWidget *m_tglwidget;
    StrokeDB *m_strokes;
};

#endif // MAINWINDOW_H

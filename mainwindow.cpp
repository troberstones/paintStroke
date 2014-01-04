#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include "glwidget.h"
//#include "testglwidget.h"
#include "subglwidget.h"
#include "strokedb.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);



    //m_glwidget = new GLWidget(0);
    m_glwidget = new subGLWidget(0);

    m_glwidget->setGeometry(1900,100,500,500);
    m_glwidget->show();
    //ui->glLayout->addWidget(m_glwidget);
    m_strokes = new StrokeDB();
    m_glwidget->SetStrokeDB(m_strokes);
#if 0
    // Qwindow version of gl canvas
    m_tglwidget = new TestGlWidget(NULL);
    m_tglwidget->show();
    m_tglwidget->raise();
    m_tglwidget->setPosition(1800,500);
    m_tglwidget->setGeometry(1800,500,500,500);
    //ui->testGlFrame->addWidget(m_tglwidget);
#endif

    // Connect the UI buttons
    // Clear the canvas
    connect(ui->pushButton, SIGNAL(released()), m_glwidget, SLOT(clearStrokes()));
    // Set the point averaging
    connect(ui->PointAveraging, SIGNAL(valueChanged(int)), m_glwidget, SLOT(setPointAveraging(int)));


}

bool MainWindow::event(QEvent *event)
{
    if (event->type() == QEvent::TabletEnterProximity ||
        event->type() == QEvent::TabletLeaveProximity) {
        m_glwidget->setTabletDevice(
            static_cast<QTabletEvent *>(event)->device());
        return true;
    }
    return false;
    //return QApplication::event(event);
}
MainWindow::~MainWindow()
{
    delete m_glwidget;
    delete ui;
}

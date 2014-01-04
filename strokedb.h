#ifndef STROKEDB_H
#define STROKEDB_H

#include <QList>
#include <QOpenGLFunctions>
#include <QVector>
#include <QVector3D>
#include <QVector2D>
#include <QVector4D>
#include <QOpenGLBuffer>

class Strips
{
public:
    Strips();
    ~Strips();
    void Finalize();
    void loadArrays() const;
    void processStroke();
    //void loadVertexAttributePointer(GLuint) const;
    int len() const;
    QVector<QVector3D> vertices;
    QVector<QVector2D> tilt;
    QVector<QVector3D> processedVerts;
    QVector<QVector4D> processedUVs;
    QVector4D color;

    std::ostream& operator<<(std::ostream &os);
    void writeData(std::ofstream &outputFile);

    private:
    bool m_final;
    //void appendFaceted(const QVector3D &a, const QVector3D &n);
};


class StrokeDB
{
public:
    StrokeDB();
    ~StrokeDB();

    void initTextures(QString fileName);
    void DrawStrokes() const;
    void startNewStroke();
    void appendPositionToStroke(float x, float y, float pressure, float tiltX, float tiltY);
    void clearStrokes();
    void SetupBuffers();
    void TestDrawBuffers();
    void SaveBuffer(std::string outputName);
    void LoadBuffer(std::string filename);
    Strips *GetStripAt(int index);
    QList<Strips*> m_strokes;
    int m_currentStroke;
private:

public:
    // Texture handle
    GLuint m_textures[1];
    bool m_justCreated;
    std::vector<QOpenGLBuffer*> m_vertexBuffer;
};
#endif // STROKEDB_H

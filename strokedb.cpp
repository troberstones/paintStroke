#include "strokedb.h"

#include <fstream>
#include <iostream>
#include <QList>
#include <QImage>
#include <QVector3D>
#include <QVector2D>
#include <QGLWidget>
#include <QOpenGLFunctions>

void Strips::writeData(std::ofstream &outputFile)
{
    outputFile << "<stroke verts=\"" << len() << "\">" << std::endl;
    for(int i = 0, end = len(); i < end; ++i)
    {
        outputFile << "v " << vertices[i].x() << " " << vertices[i].y() << " " << vertices[i].z()
           << " t " << tilt[i].x() <<  " " << tilt[i].y() << std::endl;
    }
    outputFile << "</stroke>" << std::endl;
}


int Strips::len() const
{
#ifdef DEBUG
    int len = vertices.length();
    std::cout << "Len request:" << len << std::endl;
#endif
    return vertices.length();
}

Strips::Strips()
    : m_final(false)
{

}
Strips::~Strips()
{
}

void Strips::Finalize()
{
   processStroke();
   m_final = true;
}

void Strips::loadArrays() const
{
    glVertexPointer(3, GL_FLOAT, 0, vertices.constData());
}

void Strips::processStroke()
{
    // if final, nothing to do.
    if(m_final)
        return;

    processedVerts.clear();
    processedUVs.clear();

    // Double the length of verts
    processedVerts.reserve(vertices.length()*2);
    processedUVs.reserve(vertices.length()*2);

    float strokeLength = 0;
    float seglen = 0;
    QVector2D side,side2;
// don't walk off the end of the array, drop the last point
    // so we can compute stroke tangents
    for(int j = 0, end = len()-1; j < end; ++j)
    {
        float tt = (float)j/(float)end-1;
        // Generate the perpendicular vector to the stroke
        side[0]  = -(vertices[j+1][1] - vertices[j][1]);
        side[1]  = vertices[j+1][0] - vertices[j][0];

        // length of a stroke segment
        seglen = side.length();
        seglen *= .5;
        strokeLength += seglen;
        tt = strokeLength;
        // Wrap the texture coords
        //if(strokeLength > 1) strokeLength = 0;
        side.normalize();

        float verticalNess = tilt[j].lengthSquared();
        float brushStrokeAngle = QVector2D::dotProduct(tilt[j],side);
        // Scale the perp edge by
        // small number and the distance along the stroke
        //side *= .025f * tt + .01;

        // Set stroke width
        //side *= .025f;
        side *= .1f * vertices[j][2];

        // Make two new verts on each side of the stroke
        side2[0] = vertices[j][0] - side[0];
        side2[1] = vertices[j][1] - side[1];

        side[0] += vertices[j][0];
        side[1] += vertices[j][1];

#if 0
        // Make this 2d, flatten out the zvector
        side[2] = 0.f;
        side2[2] = 0.f;
#endif

        // Create quad strip
        // Side and side2 are Vector2D, need to add the pressure component back in
        processedVerts.append( QVector3D(side2,vertices[j][2]) );
        processedVerts.append( QVector3D(side, vertices[j][2]) );
        // Create UVs and add verticalness and the brush to stroke cos theta
        processedUVs.append(QVector4D(0.f,strokeLength, verticalNess, brushStrokeAngle));
        processedUVs.append(QVector4D(1.f,strokeLength, verticalNess, brushStrokeAngle));

    }
}

#if 0
void Strips::loadVertexAttributePointer(GLuint pos) const
{
    //glVertexAttributePointer(pos, GL_FLOAT, GL_FALSE, 0, vertices.constData());
}
#endif

//void Strips::appendFaceted()
#define RNDPOS (rand() * invRndMx - .5f)
StrokeDB::StrokeDB()
{
    //printf("StrokeDB ctor");
    for(int j = 0; j < 10; j++)
    {
        Strips *tmpStrip = new Strips();
        float invRndMx= 1.f/RAND_MAX;
        float x,y,z;
        x = y = z = 0.f;

        x = RNDPOS;
        y = RNDPOS;
        z = RNDPOS;

        float sx = RNDPOS;
        float sy = RNDPOS;
        float sz = RNDPOS;
#define SPDSCALE .051f;
        sx *= SPDSCALE;
        sy *= SPDSCALE;
        sz *= SPDSCALE;

        for(int i = 0; i < 50; i++)
        {
            float A = RNDPOS;
            float B = RNDPOS;
            float C = RNDPOS;
            // AMT is the amount of deviation applied to the current speed.
#define AMT .001
            sx += A * AMT;
            sy += B * AMT;
            sz += C * AMT;
            x += sx;
            y += sy;
            z += sz;
            QVector3D tmpVector(x,y,z);
            tmpStrip->vertices.push_back(tmpVector);
            tmpStrip->tilt.push_back(QVector2D(0.f,0.f));
        }
        m_strokes.push_back(tmpStrip);
    }

#ifdef DEBUG
    // Print out the contents of the stroke buffer
    for(int j = 0; j < m_strokes.length(); j++)
    {
        std::cout << "Printing m_strokes:" << j << std::endl;
        int len = m_strokes[j]->len();
        for(int k = 0; k < len; k++ )
        {
            std::cout << "Vert: " << m_strokes[j]->vertices[k][0]
                      << " " << m_strokes[j]->vertices[k][1]
                      << " " << m_strokes[j]->vertices[k][2] << std::endl;
        }
    }
#endif
    // Set the stroke index that will be used for new strokes.
    m_currentStroke = m_strokes.length();
}
StrokeDB::~StrokeDB()
{
    //m_strokes->clear();
    SaveBuffer(std::string("c:\\tmp\\test.txt"));
    for(int i = 0, end = m_strokes.length(); i < end; ++i)
    {
        delete m_strokes[i];
    }
}

void StrokeDB::appendPositionToStroke(float x, float y, float pressure, float tiltX, float tiltY)
{
   // This isn't performant
   // std::cout << "Stroke index: " << m_currentStroke << std::endl;
   //m_strokes[m_currentStroke]->vertices.push_back(QVector3D(x,y,pressure));
    m_strokes.back()->vertices.push_back(QVector3D(x,y,pressure));
    m_strokes.back()->tilt.push_back(QVector2D(tiltX,tiltY));
    m_justCreated = false;
}

void StrokeDB::clearStrokes()
{
   m_strokes.clear();
   startNewStroke();
}

void StrokeDB::SaveBuffer(std::string outputName)
{
    std::ofstream outputFile;
    outputFile.open(outputName.c_str());
    for(int i = 0; i < m_strokes.count(); ++i)
    {
        //m_strokes[i]->Serialize(outputFile);
        outputFile << "<strokeIndex index=\"" << i << "\"/>" << std::endl;
        //outputFile << m_strokes[i] << std::endl;
        m_strokes[i]->writeData(outputFile);
    }
    outputFile.close();
}

void StrokeDB::LoadBuffer(std::string filename)
{
}

Strips* StrokeDB::GetStripAt(int index)
{
    return m_strokes[index];
}

void StrokeDB::startNewStroke()
{
    int curLen = m_strokes.length();
    if(m_strokes.length() > 0 && m_justCreated == true)
        return;
    m_currentStroke = m_strokes.count()-1;
    if(m_currentStroke > 0)
        m_strokes[m_currentStroke]->Finalize();
    m_strokes.append(new Strips());
    m_currentStroke = m_strokes.count()-1;
    m_justCreated = true;
}

void StrokeDB::initTextures(QString fileName)
{
    QImage texture;
    if(!texture.load(fileName))
    {
        std::cout << "File not loaded" << std::endl;
        return;
    }
    int w = texture.width();
    int h = texture.height();
#define USETEXTURE 1
#if USETEXTURE
    std::cout <<" loading texture" << std::endl;
    texture = QGLWidget::convertToGLFormat(texture);
    glGenTextures(1, &m_textures[0]);
    //std::cout <<GLError:" << glGetError() << std::endl;
    glBindTexture(GL_TEXTURE_2D, m_textures[0]);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 2);
    //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)texture.bits());
#endif
}


void StrokeDB::DrawStrokes() const
{
    return;
    //printf("StrokeDB DrawStrokes\n");

#ifdef DEBUG
    glColor3f(0.0f, 0.0f, 1.0f);
    glRectf(-0.4f,0.4f, 0.4f, -0.4f);
    glColor3f(1,1,1);
    glBegin(GL_LINE_LOOP);
        glVertex3f(-.5, 0, 0);
        glVertex3f( 0, .5, 0);
        glVertex3f( .5, 0, 0);
    glEnd();
#endif
    glColor3f(0.0f,0.0f,1.0f); //blue color

    //Draw strokes
    glShadeModel(GL_SMOOTH);

    //glActiveTexture(GL_TEXTURE0);
#if USETEXTURE
    glBindTexture(GL_TEXTURE_2D, m_textures[0]);
#endif
    glColor3f(1.f,1.f,1.f);
    for(int i = 0; i < m_strokes.count(); ++i)
    {
        int len = m_strokes[i]->len();
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        glBegin(GL_QUAD_STRIP);
        const QVector3D *vData = m_strokes[i]->vertices.constData();

        float strokeLength = 0;
        float seglen = 0;
        QVector3D side,side2;
        for(int j = 0; j < len-1; ++j)
        {
            float tt = (float)j/(float)(len-1);

            // Generate the perpendicular vector to the stroke
            side[0]  = -(vData[j+1][1] - vData[j][1]);
            side[1]  = vData[j+1][0] - vData[j][0];
            side[2]  = 0.f;

            // length of a stroke segment
            seglen = side.length();
            seglen *= .5;
            strokeLength += seglen;
            tt = strokeLength;
            // Wrap the texture coords
            //if(strokeLength > 1) strokeLength = 0;

            side.normalize();
            // Scale the perp edge by
            // small number and the distance along the stroke
            //side *= .025f * tt + .01;

            // Set stroke width
            //side *= .025f;
            side *= .1f * vData[j][2];

            // Make two new verts on each side of the stroke
            side2 = vData[j] - side;
            side += vData[j];

            // Make this 2d, flatten out the zvector
            side[2] = 0.f;
            side2[2] = 0.f;


            // provide a texture coord along the stroke
            //glTexCoord2f(0.f, tt);
            glTexCoord2f(0.f, strokeLength);
            glColor3f(0.f,tt,0.f);
#if 0
            glVertex3f( vData[j][0]
                       ,vData[j][1]
                       ,vData[j][2]);
#endif
            glVertex3f( side2[0]
                       ,side2[1]
                       ,side2[2]);
            glTexCoord2f(1.f, strokeLength);


            glColor3f(1.f,tt, 0.f);

            glVertex3f(  side[0]
                       , side[1]
                       , side[2]);
#if 0
            glVertex3f( vData[j][0]
                       ,vData[j][1]
                       ,vData[j][2]);
#endif
        }
        glEnd();
        //std::cout << "error:" << glGetError() << std::endl;
    }
}



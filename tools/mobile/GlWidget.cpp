/*
 * This file is part of Maemo 5 Office UI for Calligra
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Rahul Das H < h.rahuldas@gmail.com >
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */
#include <QtGui>
#include <QtOpenGL/QtOpenGL>
#include <QShortcut>
#include "GlWidget.h"

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{
    change = false;
    pos1 = pos2 = 0;
    xrot = 0;
    yrot = 0;
    zrot = 0;
    /*
     * cube positions
     */
    ax = bx = ex = fx = ey = fy = gy = hy = fz = gz = bz = cz = -1;
    cx = dx = gx = hx = ay = by = cy = dy = az = dz = ez = hz = 1;
    size = 0;
    xpos = 0.0;
    ypos = 0.0;
    zpos = 0.0;
    hkey_paused = new QShortcut(Qt::CTRL + Qt::Key_P, this);
    Q_CHECK_PTR(hkey_paused);
    hkey_end = new QShortcut(Qt::CTRL + Qt::Key_Q, this);
    Q_CHECK_PTR(hkey_end);
    hkey_next = new QShortcut(Qt::CTRL + Qt::Key_Left, this);
    Q_CHECK_PTR(hkey_next);
    hkey_prev = new QShortcut(Qt::CTRL + Qt::Key_Right, this);
    Q_CHECK_PTR(hkey_prev);
    connect(hkey_paused, SIGNAL(activated()), this, SIGNAL(paused()));
    connect(hkey_end, SIGNAL(activated()), this, SLOT(endshow()));
    connect(hkey_next, SIGNAL(activated()), this, SIGNAL(next()));
    connect(hkey_prev, SIGNAL(activated()), this, SIGNAL(prev()));
#ifdef QT_OPENGL_ES_2
    program = 0;
#endif
}

GLWidget::~GLWidget()
{
    destroy(true, true);
}

void GLWidget::initializeGL()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

#ifndef QT_OPENGL_ES_2
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
#endif

#ifdef QT_OPENGL_ES_2
#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

    QGLShader *vshader = new QGLShader(QGLShader::Vertex, this);    //Here starts openGL shader language
    Q_CHECK_PTR(vshader);
    const char *vsrc =
        "attribute highp vec4 vertex;\n"
        "attribute mediump vec4 texCoord;\n"
        "varying mediump vec4 texc;\n"
        "uniform mediump mat4 matrix;\n"
        "void main(void)\n"
        "{\n"
        "    gl_Position = matrix * vertex;\n"
        "    texc = texCoord;\n"
        "}\n";
    vshader->compileSourceCode(vsrc);

    QGLShader *fshader = new QGLShader(QGLShader::Fragment, this);   // TODO : Lighting, shadowing effects,reflection and material properties
    Q_CHECK_PTR(fshader);
    const char *fsrc =
        "uniform sampler2D texture;\n"
        "varying mediump vec4 texc;\n"
        "void main(void)\n"
        "{\n"
        "    gl_FragColor = texture2D(texture, texc.st);\n"
        "}\n";
    fshader->compileSourceCode(fsrc);

    program = new QGLShaderProgram(this);
    Q_CHECK_PTR(program);
    program->addShader(vshader);
    program->addShader(fshader);
    program->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
    program->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
    program->link();
    program->bind();
    program->setUniformValue("texture", 0);
#endif
}

void GLWidget::paintGL()
{
    qglClearColor(Qt::black);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#if !defined(QT_OPENGL_ES_2)
    glLoadIdentity();
    glTranslatef(xpos, ypos, zpos);
    glRotatef(xrot, 1.0f, 0.0f, 0.0f);
    glRotatef(yrot, 0.0f, 1.0f, 0.0f);
    glRotatef(zrot, 0.0f, 0.0f, 1.0f);
    glScalef(size, size, size);
    glVertexPointer(3, GL_FLOAT, 0, vertices.constData());
    glTexCoordPointer(2, GL_FLOAT, 0, texCoords.constData());
#else
    QMatrix4x4 m;
    m.ortho(-5, +5, +5, -5, 0.0, 15.0);
    //m.frustum(-5, +5, +5, -5, 0.0, 15.0);
    m.translate(xpos, ypos, zpos);
    m.rotate(xrot, 1.0f, 0.0f, 0.0f);
    m.rotate(yrot, 0.0f, 1.0f, 0.0f);
    m.rotate(zrot, 0.0f, 0.0f, 1.0f);
    m.scale(size, size, size);
    program->setUniformValue("matrix", m);
    program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
    program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
    program->setAttributeArray
    (PROGRAM_VERTEX_ATTRIBUTE, vertices.constData());
    program->setAttributeArray
    (PROGRAM_TEXCOORD_ATTRIBUTE, texCoords.constData());
#endif
    for (int i = 0; i < 6; ++i) {
        glBindTexture(GL_TEXTURE_2D, slidetexture);
        glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);//0-4 4-8 8-12 12-16 16-20 20-24
    }
}

void GLWidget::resizeGL(int /*width*/, int /*height*/)
{
    glViewport(0, -160, 800, 800);                   //TODO : Adjust for a better viewport than a fixed one.

#if !defined(QT_OPENGL_ES_2)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#ifndef QT_OPENGL_ES
    glOrtho(-5, +5, +5, -5, 0.0, 15.0);
#else
    glOrthof(-5, +5, +5, -5, 0.0, 15.0);
#endif
    glMatrixMode(GL_MODELVIEW);
#endif
}

void GLWidget::mousePressEvent(QMouseEvent */*event*/)
{
    emit paused();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!change) {
        pos1 = event->x();
        change = true;
    }
}

void GLWidget::mouseDoubleClickEvent(QMouseEvent * /*event*/)
{
    close();
    emit end();
}

void GLWidget::mouseReleaseEvent(QMouseEvent * event)
{
    if (change) {
        emit paused();
        pos2 = event->x();
        /*
         * calculates flicks with movements along x axis.correction for finger movements. if movement is less than 50 pix,
         * consider it as tap.
         */
        int a = pos2 - pos1;
        if (a < -100) emit next();
        if (a > 100) emit prev();
        if (a < 50 && a > -50) emit paused();
        change = false;
    }
}

void GLWidget::endshow()
{
    close();
    emit end();
}

void GLWidget::createSlide()
{
    /*
     * 6 faces of the cube
     */
    float coords[6][4][3] = {
        { { gx, gy, gz }, { fx, fy, fz }, { bx, by, bz }, { cx, cy, cz } },  //GFBC
        { { cx, cy, cz }, { bx, by, bz }, { ax, ay, az }, { dx, dy, dz } },  //CBAD
        { { hx, hy, hz }, { gx, gy, gz }, { cx, cy, cz }, { dx, dy, dz } },  //HGCD
        { { fx, fy, fz }, { ex, ey, ez }, { ax, ay, az }, { bx, by, bz } },  //FEAB
        { { hx, hy, hz }, { ex, ey, ez }, { fx, fy, fz }, { gx, gy, gz } },  //HEFG
        { { ex, ey, ez }, { hx, hy, hz }, { dx, dy, dz }, { ax, ay, az } },  //EHDA
    };

    slidetexture = bindTexture(px);

    /*
     * mapping of the texture
     */
    for (int i = 0; i < 6; ++i) {
        for (int j = 0; j < 4; ++j) {
            texCoords.append(QVector2D(j == 0 || j == 3, j == 0 || j == 1));
            vertices.append(QVector3D(coords[i][j][0], coords[i][j][1], coords[i][j][2]));
        }
    }
}

void GLWidget::setPixmapSlide(QPixmap p)
{
    deleteTexture(slidetexture);
    px = p;
    updateGL();
}

void GLWidget::setSlideSize(float frac)
{
    size = frac;
}

void GLWidget::setSlidePosition(float x, float y, float z)
{
    xpos = x;
    ypos = y;
    zpos = z;
}

void GLWidget::setSlideAngles(float x, float y, float z)
{
    xrot = x;
    yrot = y;
    zrot = z;
}

void GLWidget::setSlideShape(float a1, float a2, float a3, float b1, float b2, float b3, float c1, float c2,
                             float c3, float d1, float d2, float d3, float e1, float e2, float e3, float f1,
                             float f2, float f3, float g1, float g2, float g3, float h1, float h2, float h3)
{
    ax = a1; ay = a2; az = a3; bx = b1; by = b2; bz = b3; cx = c1; cy = c2; cz = c3; dx = d1; dy = d2; dz = d3; ex = e1; ey = e2; ez = e3; fx = f1; fy = f2; fz = f3; gx = g1; gy = g2; gz = g3; hx = h1; hy = h2; hz = h3;
}

void GLWidget::resizeSlide(float m)
{
    size = size + m;
}

void GLWidget::moveSlide(float x, float y, float z)
{
    xpos = xpos + x;
    ypos = ypos + y;
    zpos = zpos + z;
}

void GLWidget::rotateSlide(float x, float y, float z)
{
    xrot = xrot + x;
    yrot = yrot + y;
    zrot = zrot + z;
}

QList<float> GLWidget::getSlideSize()
{
    QList<float> list;
    list << size;
    return list;
}

QList<float> GLWidget::getSlidePosition()
{
    QList<float> list;
    list << xpos << ypos << zpos;
    return list;
}

QList<float> GLWidget::getSlideShape()
{
    QList<float> list;
    list << ax << bx << ex << fx << ey << fy << gy << hy << fz << gz << bz << cz
    << cx << dx << gx << hx << ay << by << cy << dy << az << dz << ez << hz ;
    return list;
}

QList<float> GLWidget::getSlideRotation()
{
    QList<float> list;
    list << xrot << yrot << zrot;
    return list;
}

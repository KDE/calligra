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
#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtGui/QtGui>
#include <QtOpenGL/QGLWidget>

class QGLShaderProgram;

class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();
    /*
     * These functions are to control the openGL scene.Create Slide defines the shape of the slide
     */
    void createSlide();
    /*
     * updates the pixmap
     */
    void setPixmapSlide(QPixmap);
    /*
     * Rotation of the slide.Increments the angle of rotation
     */
    void rotateSlide(float, float, float);
    /*
     * Increments the size of the slide.
     */
    void resizeSlide(float);
    /*
     * updates location by incrementing x,y,z cordinate values
     */
    void moveSlide(float, float, float);
    /*
     * Sets a particular size value
     */
    void setSlideSize(float);
    /*
     * set the location of the slide
     */
    void setSlidePosition(float, float, float);
    /*
     * set angles along x,y,z axis respectively
     */
    void setSlideAngles(float, float, float);
    /*
     * updates the shape of the slide. could be useful for extended animations in future.
     */
    void setSlideShape(float, float, float, float, float, float, float, float,
                       float, float, float, float, float, float, float, float,
                       float, float, float, float, float, float, float, float);
    /*
     * functions to get the values such as size,position,and rotation.
     * Slide shape could be useful in future when each cordinate value is to be verified
     */
    QList<float> getSlideSize();
    QList<float> getSlidePosition();
    QList<float> getSlideShape();
    QList<float> getSlideRotation();
public slots:
    void endshow();
signals:
    void paused();
    void next();
    void prev();
    void end();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent * event);

private:
    /*
     * holds current texture
     */
    GLuint slidetexture;
    bool change;
    /*
     * Mouse positions
     */
    int pos1;
    int pos2;
    /*
     * angles
     */
    int xrot;
    int yrot;
    int zrot;
    /*
     * position (x,y,z)
     */
    float xpos, ypos, zpos;
    /*
     * size of the slide
     */
    float size;
    float ax, ay, az, bx, by, bz, cx, cy, cz, dx, dy, dz, ex, ey, ez, fx, fy, fz, gx, gy, gz, hx, hy, hz;  // For adjusting shapes in future.
    QPixmap px;
    /*
     * vertices and texture cordinates
     */
    QVector<QVector3D> vertices;
    QVector<QVector2D> texCoords;
    /*
     * Hot keys
     */
    QShortcut * hkey_paused;
    QShortcut * hkey_prev;
    QShortcut * hkey_next;
    QShortcut * hkey_end;
    /*
     * open GL shader program
     */
#ifdef QT_OPENGL_ES_2
    QGLShaderProgram *program;
#endif
};
#endif

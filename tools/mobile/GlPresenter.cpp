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
#include "GlPresenter.h"
#include <math.h>

#ifdef Q_WS_MAEMO_5
#include <mce/dbus-names.h>
#endif

#ifndef Q_OS_ANDROID
#include <QDBusMessage>
#include <QDebug>
#endif

GLPresenter::GLPresenter(QWidget *parent, int stl, int st, QList <QPixmap> p)
    : QWidget(parent)
{
    keepDisplayOn();
    glbase = new GLWidget(this);
    Q_CHECK_PTR(glbase);
    paused = set = false;
    style = stl;
    s_time = st;
    /*
     * default animation time is 2 seconds ie, 2000 milli seconds
     */
    a_time = 2000;
    c_time = 0;
    delta = 0;
    current_slide = -1;
    pixm = p;
    totalslides = p.count();
    glbase->showFullScreen();
    reverse = done = false;
    forward = true;
    connect(glbase, SIGNAL(paused()), this , SLOT(showpaused()));
    connect(glbase, SIGNAL(next()), this , SLOT(shownext()));
    connect(glbase, SIGNAL(prev()), this , SLOT(showprevious()));
    connect(glbase, SIGNAL(end()), this , SLOT(exitshow()));
    timeline = new QTimeLine(s_time, this);
    Q_CHECK_PTR(timeline);
    timeline->setLoopCount(0);
    timeline->setFrameRange(0, s_time);
    timeline->setUpdateInterval(20); // nearly 50 updates per second
    connect(timeline, SIGNAL(frameChanged(int)), this , SLOT(slideshow()));
    timeline->start();
}

GLPresenter::~GLPresenter()
{
    destroy(true, true);

}

void GLPresenter::keepDisplayOn()
{
#ifdef Q_WS_MAEMO_5
    interface = new QDBusInterface(MCE_SERVICE, MCE_REQUEST_PATH, MCE_REQUEST_IF, QDBusConnection::systemBus(), this);
    Q_CHECK_PTR(interface);
    QDBusMessage reply = interface->call(MCE_PREVENT_BLANK_REQ);
#endif
}
void GLPresenter::showpaused()
{
    if (!paused) {
        paused = true;
        timeline->setPaused(true);
    } else if (paused) {
        paused = false;
        timeline->resume();
    }
}
void GLPresenter::shownext()
{
    if (current_slide < totalslides - 1) {
        timeline->stop();
        /*
         * reset values for next slide
         */
        glbase->setSlideAngles(0, 0, 0);
        glbase->setSlidePosition(0, 0, 0);
        glbase->setSlideSize(0);
        done = true; forward = true; reverse = false;
        timeline->start();
    }
}
void GLPresenter::showprevious()
{
    if (current_slide > 0) {
        timeline->stop();
        /*
         * reset before previous slide
         */
        glbase->setSlideAngles(0, 0, 0);
        glbase->setSlidePosition(0, 0, 0);
        glbase->setSlideSize(0);
        done = true; forward = true; reverse = false;
        current_slide = current_slide - 2;
        timeline->start();
    }
}
void GLPresenter::exitshow()
{
    close();
}

void GLPresenter::slideshow()
{
    int t = timeline->currentTime();
    if (!done)
        if (current_slide < totalslides - 1) {
            current_slide++;
            glbase->setPixmapSlide(pixm[current_slide]);
            glbase->createSlide();                         //TODO : Adjust for multiple bodies,than for present single object,ie a cube.
            done = true;
        }
    if (t < a_time) animation();
    if (t > a_time) {
        forward = false; reverse = true;
    }
    if (t > (s_time - a_time)) animation();
    /*
     * The animations is reset 100 ms before the actual show time. This is to avoid the visibility of pixmap change from the viewer.
     */
    if (t > (s_time - 100)) {
        forward = true; reverse = false;
        if (current_slide == totalslides - 1)timeline->stop();
    }
    /*
     * watch for the time line end.
     */
    if (t > c_time)
        c_time = t;
    else {
        done = false; c_time = 0;
    }
}

void GLPresenter::animation()
{
    switch (style) {
    case 0 : simpleStyle(); break;
    case 1 : cubicalMoves(); break;
    case 2 : waveMotion(); break;
    case 3 : jumpingSlides(); break;
    }
    glbase->updateGL();
}
//////////////////Transition styles///////////////////

// TODO : Bring faster animation.
void GLPresenter::simpleStyle()
{
    QList<float> temp1;
    QList<float> temp2;
    temp1 = glbase->getSlidePosition();
    temp2 = glbase->getSlideSize();
    if (!set) {
        delta = 0;
        /*
         * set slide size to half and locates out side the scree range.
         */
        glbase->setSlideSize(0.5);
        glbase->setSlidePosition(-6, 0, 0);
        set = true;
    }
    if (forward) {
        if (temp1[0] < -0.1)
            glbase->moveSlide(0.4, 0.0, 0.0);
        if (temp1[0] > -0.1 && temp2[0] < 3.95)  // 3.9 is the best fit size for a slide
            glbase->resizeSlide(0.3);
    }
    if (reverse) {
        if (temp2[0] < 1.2)
            glbase->moveSlide(-0.4, 0.0, 0.0);
        if (temp2[0] > 1)
            glbase->resizeSlide(-0.3);
    }
}

void GLPresenter::cubicalMoves()
{
    QList<float> temp1;
    QList<float> temp2;
    temp1 = glbase->getSlideRotation();
    temp2 = glbase->getSlideSize();

    if (forward) {
        if (temp2[0] < 3.9)
            glbase->resizeSlide(0.1);
        if (temp1[0] < 180 || temp1[1] < 180 || temp1[2] < 180) // 180 degree rotation
            glbase->rotateSlide(3, 3, 3);
    }
    if (reverse) {
        if (temp2[0] > 0)
            glbase->resizeSlide(-0.1);
        if (temp1[0] > 0)
            glbase->rotateSlide(-3, -3, -3);
        if (timeline->currentTime() > s_time - 100) {
            set = false;
        }
    }
}

void GLPresenter::waveMotion()
{
    float y;
    float pi = 22 / 7;
    delta = delta + (pi / 10);  //frequency
    y = sin(delta);
    QList<float> temp1;
    QList<float> temp2;
    temp1 = glbase->getSlidePosition();
    temp2 = glbase->getSlideSize();
    if (!set) {
        delta = 0;
        glbase->setSlideSize(0.5);
        glbase->setSlidePosition(-6, -1.5, 0);
        set = true;
    }
    if (forward) {
        if (temp1[0] < -1)
            glbase->moveSlide(0.1, y / 2, 0);   // amplitude
        if (temp1[0] > -1 && temp1[0] != 0) {
            if (temp1[0] < 0.1)glbase->moveSlide(0.09, 0, 0);
            if (temp1[0] > 0.1)glbase->setSlidePosition(0, 0, 0);
        }
        if (temp2[0] < 3.9 && temp1[0] == 0)glbase->resizeSlide(0.5);
    }
    if (reverse) {
        if (temp2[0] > 0.5)glbase->resizeSlide(-0.5);
        if (temp2[0] == 0.5)glbase->moveSlide(0.1, y / 2, 0);
        if (timeline->currentTime() > s_time - 100) {
            set = false; delta = 0;
        }
    }
}

void GLPresenter::jumpingSlides()
{
    float y;
    float pi = 22 / 7;
    delta = delta + (pi / 30);  //frequency
    y = sin(delta);
    if (y > 0)y = (-y);    // half of the cycle is shifted.
    QList<float> temp1;
    QList<float> temp2;
    QList<float> temp12;
    if (!set) {
        delta = 0;
        glbase->setSlideSize(0);
        glbase->setSlidePosition(-5, 3.5, 0);
        glbase->setSlideAngles(10, 30, 0);
        set = true;
    }
    temp1 = glbase->getSlidePosition();
    temp2 = glbase->getSlideSize();
    temp12 = glbase->getSlideRotation();
    if (forward) {
        if (temp1[0] <= 0) {
            glbase->setSlidePosition(temp1[0], 2*y, temp1[2]);
            glbase->resizeSlide(0.01);
            glbase->moveSlide(0.07, -0.01, 0);
        }
        if (temp1[0] > -0.5) {
            if (temp2[0] < 3.9)
                glbase->resizeSlide(0.3);
            glbase->setSlidePosition(0, 0, 0);
            glbase->setSlideAngles(0, 0, 0);
        }
    }
    if (reverse) {
        if (temp2[0] >= 1) glbase->resizeSlide(-0.3);
        if (temp2[0] <= 1) {
            glbase->setSlidePosition(temp1[0], 2*y, temp1[2]);
            glbase->resizeSlide(-0.01);
            glbase->moveSlide(0.07, 0.01, 0);
        }
        if (timeline->currentTime() > s_time - 100) {
            set = false; delta = 0;
        }
    }
}

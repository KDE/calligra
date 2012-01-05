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

#ifndef GLPRESENTER_H
#define GLPRESENTER_H

#include "GlWidget.h"

namespace Ui
{
class GLPresenter;
}
class QDBusInterface;

class GLPresenter : public QWidget
{
    Q_OBJECT

public:

    GLPresenter(QWidget *parent, int, int, QList <QPixmap>);
    ~GLPresenter();

public slots:
    /*
     * selects the animation type
     */
    void animation();
    /*
     * slideshow control the whole animations.
     */
    void slideshow();
    /*
     * show controls
     */
    void showpaused();
    void shownext();
    void showprevious();
    void exitshow();

private :
    GLWidget * glbase;
    int totalslides;
    int current_slide;
    /*
     * animation style
     */
    int style;
    /*
     * animation time
     */
    int a_time;
    /*
     * total show time
     */
    int s_time;
    /*
     * a watcher for time line
     */
    int c_time;
    QDBusInterface *interface;
    QList <QPixmap> pixm;
    QTimeLine * timeline;
    bool set, done, forward, reverse, paused;
    /*
     * need to keep the display on while presentation (maemo specific)
     */
    void keepDisplayOn();
    /*
     * PRESENTATIOn STYLES AND VALUES
     */
    float delta;
    void simpleStyle();
    void cubicalMoves();
    void waveMotion();
    void jumpingSlides();
};
#endif // GLPRESENTER_H

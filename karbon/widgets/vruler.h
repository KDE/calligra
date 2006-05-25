/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
 * Copyright (C) 2002 Patrick Julien <freak@codepimps.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef VRULER_H_
#define VRULER_H_

#include <q3frame.h>
#include <qpixmap.h>
//Added by qt3to4:
#include <QPaintEvent>
#include <QResizeEvent>
#include <KoUnit.h>

// XXX: Make this look more like the KOffice ruler -- the KOffice
// ruler is not quite suited to Krita. Also: start units with 0,
// print every 100 units.

class QPainter;

class VRuler : public Q3Frame {
    Q_OBJECT
    typedef Q3Frame super;

public:
    VRuler(Qt::Orientation, QWidget *parent = 0, const char *name = 0);
    virtual ~VRuler();

public:
    KoUnit::Unit unit() const;

public slots:
    void setZoom(double zoom);
    void updatePointer(qint32 x, qint32 y);
    void updateVisibleArea(qint32 xpos, qint32 ypos);
    void setUnit(KoUnit::Unit u);
    void hide();
    void show();

public:
    virtual void paintEvent(QPaintEvent *e);
    virtual void resizeEvent(QResizeEvent *e);

protected:
    void recalculateSize();
    void drawRuler();
    void initMarker(qint32 w, qint32 h);
    void drawNums(QPainter *gc, qint32 x, qint32 y, QString& num, bool orientationHoriz);

private:
    KoUnit::Unit m_unit;
    Qt::Orientation m_orientation;
    qint32 m_firstVisible;
    qint32 m_currentPosition;
    QPixmap *m_pixmapBuffer;
    QPixmap m_pixmapMarker;
    QPixmap m_pixmapNums;
    double m_zoom;

private:
    static const char *m_nums[];
};

#endif // VRULER_H_


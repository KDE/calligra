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
#include <qpainter.h>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QPixmap>
#include <QPalette>
#include <QBrush>

#include "kdebug.h"

#include "vruler.h"

#define MARKER_WIDTH 1
#define MARKER_HEIGHT 20
#define RULER_SIZE 20

const char *VRuler::m_nums[] = {
    "70 7 2 1",
    "  c Black",
    "X c None",
    "XX   XXXXXX XXXX   XXXX   XXXXXX XXX     XXXX  XXX     XXX   XXXX   XX",
    "X XXX XXXX  XXX XXX XX XXX XXXX  XXX XXXXXXX XXXXXXXXX XX XXX XX XXX X",
    "X XXX XXXXX XXXXXXX XXXXXX XXX X XXX XXXXXX XXXXXXXXX XXX XXX XX XXX X",
    "X XXX XXXXX XXXXX  XXXXX  XXX XX XXX    XXX    XXXXXX XXXX   XXXX    X",
    "X XXX XXXXX XXXX XXXXXXXXX XX     XXXXXX XX XXX XXXX XXXX XXX XXXXXX X",
    "X XXX XXXXX XXX XXXXXX XXX XXXXX XXXXXXX XX XXX XXXX XXXX XXX XXXXX XX",
    "XX   XXXXXX XXX     XXX   XXXXXX XXX    XXXX   XXXXX XXXXX   XXXX  XXX"
};

VRuler::VRuler(Qt::Orientation o, QWidget *parent, const char *name) : super(parent, name, Qt::WNoAutoErase | Qt::WResizeNoErase), m_pixmapNums(m_nums)
{
    QPalette p = palette();
    p.setBrush(QPalette::Window, QBrush(Qt::NoBrush));
    // TODO: Check if this is equivalent with the line below
    // setBackgroundMode(NoBackground);
    setFrameStyle(Box | Sunken);
    setLineWidth(1);
    setMidLineWidth(0);
    m_orientation = o;
    m_unit = KoUnit::U_PT;
    m_zoom = 1.0;
    m_firstVisible = 0;
    m_pixmapBuffer = 0;
    m_currentPosition = -1;

    if (m_orientation == Qt::Horizontal) {
        setFixedHeight(RULER_SIZE);
        initMarker(MARKER_WIDTH, MARKER_HEIGHT);
    } else {
        setFixedWidth(RULER_SIZE);
        initMarker(MARKER_HEIGHT, MARKER_WIDTH);
    }
}

VRuler::~VRuler()
{
    delete m_pixmapBuffer;
}

void VRuler::initMarker(qint32 w, qint32 h)
{
    QPainter p;

    m_pixmapMarker = QPixmap(w, h);
    p.begin(&m_pixmapMarker);
    p.setPen( QColor( "blue" ) );
    p.eraseRect(0, 0, w, h);
    p.drawLine(0, 0, w - 1, h - 1);
    p.end();
}

void VRuler::recalculateSize()
{
    qint32 w;
    qint32 h;

    if (m_pixmapBuffer) {
        delete m_pixmapBuffer;
        m_pixmapBuffer = 0;
    }

    if (m_orientation == Qt::Horizontal) {
        w = width();
        h = RULER_SIZE;
    } else {
        w = RULER_SIZE;
        h = height();
    }

    m_pixmapBuffer = new QPixmap(w, h);
    Q_CHECK_PTR(m_pixmapBuffer);

    drawRuler();
    updatePointer(m_currentPosition, m_currentPosition);
}

KoUnit::Unit VRuler::unit() const
{
    return  m_unit;
}

void VRuler::setUnit(KoUnit::Unit u)
{
    m_unit = u;
    drawRuler();
    updatePointer(m_currentPosition, m_currentPosition);
    repaint();
}

void VRuler::setZoom(double zoom)
{
    m_zoom = zoom;
    recalculateSize();
    drawRuler();
    updatePointer(m_currentPosition, m_currentPosition);
    repaint();
}

void VRuler::updatePointer(qint32 x, qint32 y)
{
    if (m_pixmapBuffer) {
        if (m_orientation == Qt::Horizontal) {
            if (m_currentPosition != -1)
                repaint(m_currentPosition, 1, MARKER_WIDTH, MARKER_HEIGHT);

            if (x != -1) {
                QPainter p(this);
                p.drawPixmap(QPoint(x, 1), m_pixmapMarker, QRect(0, 0, MARKER_WIDTH, MARKER_HEIGHT));
                m_currentPosition = x;
            }
        } else {
            if (m_currentPosition != -1)
                repaint(1, m_currentPosition, MARKER_HEIGHT, MARKER_WIDTH);

            if (y != -1) {
                QPainter p(this);
                p.drawPixmap(QPoint(1, y), m_pixmapMarker, QRect(0, 0, MARKER_HEIGHT, MARKER_WIDTH));
                m_currentPosition = y;
            }
        }
    }
}

void VRuler::updateVisibleArea(qint32 xpos, qint32 ypos)
{
    if (m_orientation == Qt::Horizontal)
        m_firstVisible = xpos;
    else
        m_firstVisible = ypos;

	//kDebug() << "--###-- VRuler::updateVisibleArea(" << xpos << ", " << ypos << ")" << endl;
    drawRuler();
    repaint();
    updatePointer(m_currentPosition, m_currentPosition);
	//kDebug() << "--###-- VRuler::updatePointer(" << m_currentPosition << ", " << m_currentPosition << ")" << endl;
}

void VRuler::paintEvent(QPaintEvent *e)
{
    if (m_pixmapBuffer) {
        const QRect& rect = e -> rect();

        QPainter p(this);
        p.drawPixmap(rect.topLeft(), *m_pixmapBuffer, rect);
        super::paintEvent(e);
    }
}

void VRuler::drawRuler()
{
    QPainter p;
    QString buf;
    qint32 st1 = 0;
    qint32 st2 = 0;
    qint32 st3 = 0;
    qint32 st4 = 0;
    qint32 stt = 0;

    if (!m_pixmapBuffer)
        return;

    p.begin(m_pixmapBuffer);
    p.setPen(QColor(0x70, 0x70, 0x70));
    p.setBackground(palette().window());
    p.eraseRect(0, 0, m_pixmapBuffer -> width(), m_pixmapBuffer -> height());

    switch (m_unit) {
        case KoUnit::U_PT:
        case KoUnit::U_MM:
        case KoUnit::U_DD:
        case KoUnit::U_CC:
            st1 = 1;
            st2 = 5;
            st3 = 10;
            st4 = 25;
            stt = 100;
            break;
        case KoUnit::U_CM:
        case KoUnit::U_PI:
        case KoUnit::U_INCH:
            st1 = 1;
            st2 = 2;
            st3 = 5;
            st4 = 10;
            stt = 1;
            break;
        default:
            break;
    }

    qint32 pos = 0;
    bool s1 = KoUnit::fromUserValue(st1, m_unit) * m_zoom > 3.0;
    bool s2 = KoUnit::fromUserValue(st2, m_unit) * m_zoom > 3.0;
    bool s3 = KoUnit::fromUserValue(st3, m_unit) * m_zoom > 3.0;
    bool s4 = KoUnit::fromUserValue(st4, m_unit) * m_zoom > 3.0;

    if (m_orientation == Qt::Horizontal) {
        // XXX: This was 7 * 4 -- why? what was the idea about having 30 point intervals?
        float cx = KoUnit::fromUserValue(100, m_unit) / m_zoom;
        qint32 step = qRound(cx);//((qint32)(cx / (float)stt) + 1) * stt;
        qint32 start = (qint32)(KoUnit::toUserValue(m_firstVisible, m_unit) / m_zoom);

        do {
            pos = (qint32)(KoUnit::fromUserValue(start, m_unit) * m_zoom - m_firstVisible);

            if (!s3 && s4 && start % st4 == 0)
                p.drawLine(pos, RULER_SIZE - 9, pos, RULER_SIZE);

            if (s3 && start % st3 == 0)
                p.drawLine(pos, RULER_SIZE - 9, pos, RULER_SIZE);

            if (s2 && start % st2 == 0)
                p.drawLine(pos, RULER_SIZE - 7, pos, RULER_SIZE);

            if (s1 && start % st1 == 0)
                p.drawLine(pos, RULER_SIZE - 5, pos, RULER_SIZE);

            if (step && start % step == 0) {
                buf.setNum(QABS(start));
                drawNums(&p, pos, 4, buf, true);
            }

            start++;
        } while (pos < m_pixmapBuffer -> width());
    } else {
        m_firstVisible = 0;
        float cx = KoUnit::fromUserValue(100, m_unit) / m_zoom;
        qint32 height = m_pixmapBuffer -> height() - 1;
        qint32 step = qRound(cx);
        qint32 start = (qint32)(KoUnit::toUserValue(m_firstVisible, m_unit) / m_zoom);

        do {
            pos = height - (qint32)(KoUnit::fromUserValue(start, m_unit) * m_zoom - m_firstVisible);

            if (!s3 && s4 && start % st4 == 0)
                p.drawLine(RULER_SIZE - 9, pos, RULER_SIZE, pos);

            if (s3 && start % st3 == 0)
                p.drawLine(RULER_SIZE - 9, pos, RULER_SIZE, pos);

            if (s2 && start % st2 == 0)
                p.drawLine(RULER_SIZE - 7, pos, RULER_SIZE, pos);

            if (s1 && start % st1 == 0)
                p.drawLine(RULER_SIZE - 5, pos, RULER_SIZE, pos);

            if (step && start % step == 0) {
                buf.setNum(QABS(start));
                drawNums(&p, 4, pos, buf, false);
            }

            start++;
        } while (pos > 0);
    }

    p.end();
}

void VRuler::resizeEvent(QResizeEvent *)
{
    recalculateSize();
}

void VRuler::show()
{
    if (m_orientation == Qt::Horizontal) {
        setFixedHeight(RULER_SIZE);
        initMarker(MARKER_WIDTH, MARKER_HEIGHT);
    } else {
        setFixedWidth(RULER_SIZE);
        initMarker(MARKER_HEIGHT, MARKER_WIDTH);
    }

    super::show();
}

void VRuler::hide()
{
    if (m_orientation == Qt::Horizontal)
        setFixedHeight(1);
    else
        setFixedWidth(1);
}

void VRuler::drawNums(QPainter *p, qint32 x, qint32 y, QString& num, bool orientationHoriz)
{
    if (orientationHoriz)
        x -= 7;
    else
        y -= 8;

    for (quint32 k = 0; k < (quint32)num.length(); k++) {
        qint32 st = num.at(k).digitValue() * 7;

        p -> drawPixmap(x, y, m_pixmapNums, st, 0, 7, 7);

        if (orientationHoriz)
            x += 7;
        else
            y += 8;
    }
}

#include "vruler.moc"


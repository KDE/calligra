/* This file is part of the KDE project
 * Copyright (C) 2010 Adam Celarek <kdedev at xibo dot at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "kis_curve_widget_base.h"

#include <QPainter>
#include <QPainterPath>
#include <QVector2D>
#include <QMouseEvent>
#include <QDebug>

/// delete later, it was moved to kis_curve.h
bool pointCompare (const QPointF &p1, const QPointF &p2)
{
    if(p1.x()<p2.x()) return true;
    if(p1.x()==p2.x() && p1.y()<p2.y()) return true;
    return false;
}

KisCurveWidgetBase::KisCurveWidgetBase(QWidget *parent)
    : QWidget(parent), /*CURVE_RANGE(1000), */m_currentPoint(-1)
{
    KisCurveWidgetBase::reset();
    setMinimumSize(50, 50);
}

KisCurveWidgetBase::~KisCurveWidgetBase()
{}

QList<QPointF> KisCurveWidgetBase::controlPoints() const
{
    return m_points;
}

void KisCurveWidgetBase::setControlPoints(const QList<QPointF> &points)
{
    m_points = points;
}

void KisCurveWidgetBase::reset()
{
    m_points.clear();

    m_points.append(QPointF(0, 0));
    m_points.append(QPoint(CURVE_RANGE, CURVE_RANGE));

    update();
}

void KisCurveWidgetBase::mousePressEvent(QMouseEvent *event)
{
    QVector2D mousePos(m_converterMatrix.inverted().map(event->posF()));
    if(event->button()==Qt::LeftButton) {
        bool movingButton=false;
        for(int i=0; i<m_points.size(); i++) {
            QVector2D pointPos(m_points.at(i));
            QVector2D mouseDistance = (mousePos-pointPos);
            mouseDistance*=QVector2D(m_converterMatrix.m11(), m_converterMatrix.m22());
            if(mouseDistance.lengthSquared()<BLIB_RADIUS*BLIB_RADIUS) {
                m_currentPoint=i;
                movingButton=true;
                break;
            }
        }

        if(movingButton==false) {
            addPoint(mousePos);
            // start moving the new point
            mousePressEvent(event);
        }
    }
    else if(event->button()==Qt::RightButton) {
        removePoint(mousePos);
    }
}

void KisCurveWidgetBase::mouseMoveEvent(QMouseEvent *event)
{
    qDebug() << "KisCurveWidgetBase::mouseMoveEvent(QMouseEvent): " << event->pos();
    if(m_currentPoint!=-1) {
        QPointF mousePos = m_converterMatrix.inverted().map(event->posF());
        mousePos.setX(qBound(0., mousePos.x(), CURVE_RANGE));
        mousePos.setY(qBound(0., mousePos.y(), CURVE_RANGE));
        qDebug() << "  mapped mousePos: " << mousePos;
        qDebug() << "  m_currentPoint:  " << m_currentPoint;
        qDebug() << "  m_points.size:   " << m_points.size();

        if(m_currentPoint!=0 && m_currentPoint!=m_points.size()-1) {
            if(!(rect().adjusted(-20, -20, 20, 20).contains(event->pos()))) {
                m_points.removeAt(m_currentPoint);
                m_currentPoint = -1;
                update();
                qDebug() << "  first Branch";
                return;
            }
            else if(mousePos.x()<m_points.at(m_currentPoint-1).x()+SMALLEST_STEP) {
                m_points[m_currentPoint].setX(m_points.at(m_currentPoint-1).x()+SMALLEST_STEP);
                qDebug() << "  second Branch";
            }
            else if (mousePos.x()>m_points.at(m_currentPoint+1).x()-SMALLEST_STEP) {
                m_points[m_currentPoint].setX(m_points.at(m_currentPoint+1).x()-SMALLEST_STEP);
                qDebug() << "  third Branch";
            }
            else {
                m_points[m_currentPoint].setX(mousePos.x());
                qDebug() << "  fourth Branch";
            }
        }
        m_points[m_currentPoint].setY(mousePos.y());
        update();
    }
}

void KisCurveWidgetBase::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    m_currentPoint=-1;
}

void KisCurveWidgetBase::mouseDoubleClickEvent(QMouseEvent *event)
{
    QVector2D mousePos(m_converterMatrix.inverted().map(event->posF()));
    if(!removePoint(mousePos))
        addPoint(mousePos);
}

void KisCurveWidgetBase::resizeEvent(QResizeEvent *)
{
    m_converterMatrix = QMatrix();
    m_converterMatrix.scale(width()/CURVE_RANGE, height()/(-1*CURVE_RANGE));
    m_converterMatrix.translate(0, -1*CURVE_RANGE);
}

void KisCurveWidgetBase::paintBlips(QPainter *painter)
{
    for(int i=0; i<m_points.size(); i++) {
        painter->drawEllipse(m_points.at(i), BLIB_RADIUS/m_converterMatrix.m11(), BLIB_RADIUS/m_converterMatrix.m22());
    }
}

void KisCurveWidgetBase::paintBackground(QPainter *painter)
{
    QImage bg(width(), height(), QImage::Format_ARGB32_Premultiplied);
    bg.fill(qRgb(250, 250, 250));

    painter->drawImage(0, 0, bg);
}

void KisCurveWidgetBase::addPoint(const QVector2D& pos)
{
    m_points.append(pos.toPointF());
    qSort(m_points.begin(), m_points.end(), pointCompare);
    update();
}

bool KisCurveWidgetBase::removePoint(const QVector2D& pos)
{
    qDebug() << "KisCurveWidgetBase::removePoint:";
    for(int i=0; i<m_points.size(); i++) {
        qDebug() << "  distance to point nr " << i << ": " << (QVector2D(m_points.at(i))-pos).lengthSquared();

        QVector2D pointPos(m_points.at(i));
        QVector2D mouseDistance = (pos-pointPos);
        mouseDistance*=QVector2D(m_converterMatrix.m11(), m_converterMatrix.m22());

        if(mouseDistance.lengthSquared()<BLIB_RADIUS*BLIB_RADIUS) {
            if(i>0 && i<m_points.size()-1) { // don't remove a point, if on start or end
                m_points.removeAt(i);
                update();
            }
            return true;
        }
    }

    return false;
}


/* This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003 The Karbon Developers
   Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef _VGRADIENTWIDGET_H_
#define _VGRADIENTWIDGET_H_

#include <QWidget>

//Added by qt3to4:
#include <QPaintEvent>
#include <QMouseEvent>
#include <karbon_export.h>

class QGradient;
class QPainter;
class QColor;

class KARBONBASE_EXPORT VGradientWidget : public QWidget
{
    Q_OBJECT

public:
    VGradientWidget( const QGradientStops & stops, QWidget* parent = 0L, const char* name = 0L );
    ~VGradientWidget();

    virtual void paintEvent( QPaintEvent* );
    void setStops( const QGradientStops & stops );
    QGradientStops stops() const;
signals:
    void changed();

private:
    /** mouse events... For color stops manipulation */
    void mousePressEvent( QMouseEvent* );
    void mouseReleaseEvent( QMouseEvent* );
    void mouseDoubleClickEvent( QMouseEvent* );
    void mouseMoveEvent( QMouseEvent* );

    void paintColorStop( QPainter& p, int x, QColor& color );
    void paintMidPoint( QPainter& p, int x );

    QLinearGradient m_gradient; ///< the gradient with the stops to modify
    int m_currentStop; ///< the stop to modify.

    QRect m_pntArea;
}; // VGradientWidget

#endif /* _VGRADIENTWIDGET_H_ */

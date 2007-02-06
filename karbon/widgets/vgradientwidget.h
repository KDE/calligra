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

#include <karbon_export.h>

class QGradient;
class QPainter;
class QPaintEvent;
class QMouseEvent;
class QColor;

/**
 * A widget for editing a gradients color stops.
 */
class KARBONBASE_EXPORT VGradientWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructs a new gradient widget with the given parent
     * @param parent the gradient widgets parent
     */
    explicit VGradientWidget( QWidget* parent = 0L );

    /// Destroys the gradient widget
    ~VGradientWidget();

    /**
     * Sets the gradient color stops to be edited.
     * @param stops the gradient color stops to edit
     */
    void setStops( const QGradientStops & stops );

    /**
     * Returns the edited gradient color stops.
     * @return the edited gradient color stops
     */
    QGradientStops stops() const;

signals:
    /// Is emitted as soon as a color stop was changed/added/removed
    void changed();

protected:
    virtual void paintEvent( QPaintEvent* );

private:
    /** mouse events... For color stops manipulation */
    void mousePressEvent( QMouseEvent* );
    void mouseReleaseEvent( QMouseEvent* );
    void mouseDoubleClickEvent( QMouseEvent* );
    void mouseMoveEvent( QMouseEvent* );

    /// paints a color stop handle
    void paintColorStop( QPainter& p, int x, const QColor& color );
    /// paints a midpoint handle
    void paintMidPoint( QPainter& p, int x );

    QLinearGradient m_gradient; ///< the gradient with the stops to modify
    int m_currentStop; ///< the stop to modify.

    QRect m_pntArea; ///< the area where the gradient is painted
};

#endif /* _VGRADIENTWIDGET_H_ */

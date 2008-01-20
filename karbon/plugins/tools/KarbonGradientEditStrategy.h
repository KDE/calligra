/* This file is part of the KDE project
 * Copyright (C) 2007-2008 Jan Hambrecht <jaham@gmx.net>
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

#ifndef _KARBONGRADIENTEDITSTRATEGY_H_
#define _KARBONGRADIENTEDITSTRATEGY_H_

#include <QRectF>
#include <QBrush>
#include <KoLineBorder.h>

class QPainter;
class QUndoCommand;
class QLinearGradient;
class QRadialGradient;
class QConicalGradient;
class KoShape;
class KoViewConverter;

/// The base class for gradient editing strategies
class GradientStrategy
{
public:
    /// The different targets of the gradients
    enum Target { Fill, Stroke };

    /// constructs new strategy on the specified shape and target
    explicit GradientStrategy( KoShape *shape, const QGradient * gradient, Target target );

    virtual ~GradientStrategy() {}

    /// painting of the gradient editing handles
    void paint( QPainter &painter, const KoViewConverter &converter );

    /// selects handle at the given position
    bool selectHandle( const QPointF &mousePos, const KoViewConverter &converter );

    /// selects the the gradient line at the given position
    bool selectLine( const QPointF &mousePos, const KoViewConverter &converter );

    /// selects the gradient stop at the given position
    bool selectStop( const QPointF &mousePos, const KoViewConverter &converter );

    /// mouse position handling for moving handles
    void handleMouseMove(const QPointF &mouseLocation, Qt::KeyboardModifiers modifiers);

    /// sets the strategy into editing mode
    void setEditing( bool on );

    /// checks if strategy is in editing mode
    bool isEditing() { return m_editing; }

    /// create the command for changing the shapes background
    QUndoCommand * createCommand( QUndoCommand * parent );

    /// schedules a repaint of the shape and gradient handles
    void repaint() const;

    /// sets the handle radius used for painting the handles
    static void setHandleRadius( int radius ) { m_handleRadius = radius; }

    /// returns the actual handle radius
    static int handleRadius() { return m_handleRadius; }

    /// returns the gradient handles bounding rect
    QRectF boundingRect() const;

    /// returns the actual gradient
    const QGradient * gradient();

    /// Returns the gradient target
    Target target() const;

    /// Starts drawing the gradient at the given mouse position
    void startDrawing( const QPointF &mousePos );

protected:
    enum SelectionType { None, Handle, Line, Stop };

    /// Sets the actual selection
    void setSelection( SelectionType selection, int index = 0 );

    /// paints a handle at the given position
    void paintHandle( QPainter &painter, const KoViewConverter &converter, const QPointF &position );

    /// paints the 
    void paintStops( QPainter &painter, const KoViewConverter &converter, const QPointF &start, const QPointF &stop );

    /// checks if given mouse position is on specified line segment
    bool mouseAtLineSegment( const QPointF &mousePos, const QPointF &segStart, const QPointF &segStop, double maxDistance );

    /// Sets the handle indices defining the gradient line
    void setGradientLine( int start, int stop );

    /// Returns the handle rect
    QRectF handleRect( const KoViewConverter &converter ) const;

    /// creates an updated brush from the actual data
    virtual QBrush brush() = 0;

    KoShape *m_shape;         ///< the shape we are working on
    SelectionType m_selection; ///< the actual selection type
    int m_selectionIndex;      ///< the actual selection index
    QBrush m_oldBrush;   ///< the old background brush
    QBrush m_newBrush;   ///< the new background brush
    QList<QPointF> m_handles; ///< the list of handles
    QGradientStops m_stops;   ///< the gradient stops
    QMatrix m_matrix;         ///< matrix to map handle into document coordinate system
    QPointF m_lastMousePos;   ///< last mouse position
    KoLineBorder m_oldStroke; ///< the old stroke
private:
    static int m_handleRadius; ///< the handle radius for all gradient strategies
    bool m_editing; /// the edit mode flag
    Target m_target; ///< the gradient target
    QPair<int,int> m_gradientLine; ///< the handle indices defining the gradient line
};

/// Strategy for editing a linear gradient
class LinearGradientStrategy : public GradientStrategy
{
public:
    LinearGradientStrategy( KoShape *shape, const QLinearGradient *gradient, Target target );
private:
    virtual QBrush brush();
    enum Handles { start, stop };
};

/// Strategy for editing a radial gradient
class RadialGradientStrategy : public GradientStrategy
{
public:
    RadialGradientStrategy( KoShape *shape, const QRadialGradient *gradient, Target target );
private:
    virtual QBrush brush();
    enum Handles { center, focal, radius };
};

/// Strategy for editing a conical gradient
class ConicalGradientStrategy : public GradientStrategy
{
public:
    ConicalGradientStrategy( KoShape *shape, const QConicalGradient *gradient, Target target );
private:
    virtual QBrush brush();
    enum Handles { center, direction };
};

#endif // _KARBONGRADIENTEDITSTRATEGY_H_


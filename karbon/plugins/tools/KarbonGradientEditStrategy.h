/* This file is part of the KDE project
 * Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
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
    /// constructs new strategy on the specified shape
    explicit GradientStrategy( KoShape *shape );

    virtual ~GradientStrategy() {}

    /// painting of the gradient editing handles
    virtual void paint( QPainter &painter, KoViewConverter &converter ) = 0;

    /// selects handle at the given position
    virtual bool selectHandle( const QPointF &mousePos );

    /// mouse position handling for moving handles
    void handleMouseMove(const QPointF &mouseLocation, Qt::KeyboardModifiers modifiers);

    /// sets the strategy into editing mode
    void setEditing( bool on );

    /// checks if strategy is in editing mode
    bool isEditing() { return m_editing; }

    /// create the command for changing the shapes background
    QUndoCommand * createCommand();

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
protected:
    /// paints a handle at the given position
    void paintHandle( QPainter &painter, KoViewConverter &converter, const QPointF &position );
    /// checks if given mouse position is inside the handle rect at the specified position
    bool mouseInsideHandle( const QPointF &mousePos, const QPointF &handlePos );
    /// creates an updated background brush from the actual data
    virtual QBrush background() = 0;
    KoShape *m_shape;         ///< the shape we are working on
    int m_selectedHandle;     ///< index of currently deleted handle or -1 if none selected
    QBrush m_oldBackground;   ///< the old background brush
    QBrush m_newBackground;   ///< the new background brush
    QList<QPointF> m_handles; ///< the list of handles
    QMatrix m_matrix;         ///< matrix to map handle into document coordinate system
private:
    static int m_handleRadius; ///< the handle radius for all gradient strategies
    bool m_editing; /// the edit mode flag
};

/// Strategy for editing a linear gradient
class LinearGradientStrategy : public GradientStrategy
{
public:
    LinearGradientStrategy( KoShape *shape, const QLinearGradient *gradient );
    virtual void paint( QPainter &painter, KoViewConverter &converter );
private:
    virtual QBrush background();
    enum Handles { start, stop };
};

/// Strategy for editing a radial gradient
class RadialGradientStrategy : public GradientStrategy
{
public:
    RadialGradientStrategy( KoShape *shape, const QRadialGradient *gradient );
    virtual void paint( QPainter &painter, KoViewConverter &converter );
private:
    virtual QBrush background();
    enum Handles { center, focal, radius };
};

/// Strategy for editing a conical gradient
class ConicalGradientStrategy : public GradientStrategy
{
public:
    ConicalGradientStrategy( KoShape *shape, const QConicalGradient *gradient );
    virtual void paint( QPainter &painter, KoViewConverter &converter );
private:
    virtual QBrush background();
    enum Handles { center, direction };
};

#endif // _KARBONGRADIENTEDITSTRATEGY_H_


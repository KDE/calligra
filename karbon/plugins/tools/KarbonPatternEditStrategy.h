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

#ifndef _KARBONPATTERNEDITSTRATEGY_H_
#define _KARBONPATTERNEDITSTRATEGY_H_

#include <QBrush>

class KoShape;
class KoViewConverter;

class QPainter;
class QUndoCommand;

/// The class used for editing a shapes pattern
class KarbonPatternEditStrategy
{
public:
    /// constructs an edit strategy working on the given shape
    KarbonPatternEditStrategy( KoShape * shape );

    /// destroy the edit strategy
    ~KarbonPatternEditStrategy();

    /// painting of the pattern editing handles
    void paint( QPainter &painter, KoViewConverter &converter );

    /// paints a singale handle
    void paintHandle( QPainter &painter, KoViewConverter &converter, const QPointF &position );

    /// checks if mouse position is inside handle rect
    bool mouseInsideHandle( const QPointF &mousePos, const QPointF &handlePos );

    /// selects handle at the given position
    bool selectHandle( const QPointF &mousePos );

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

    /// returns the pattern handles bounding rect
    QRectF boundingRect();

    /// returns the actual background brush
    QBrush background();

    /// sets the handle radius used for painting the handles
    static void setHandleRadius( int radius ) { m_handleRadius = radius; }

    /// returns the actual handle radius
    static int handleRadius() { return m_handleRadius; }

private:
    enum Handles { center, direction };

    KoShape *m_shape;          ///< the shape we are working on
    int m_selectedHandle;      ///< index of currently deleted handle or -1 if none selected
    QBrush m_oldBackground;    ///< the old background brush
    QBrush m_newBackground;    ///< the new background brush
    QList<QPointF> m_handles;  ///< the list of handles
    QMatrix m_matrix;          ///< matrix to map handle into document coordinate system
    static int m_handleRadius; ///< the handle radius for all gradient strategies
    bool m_editing;            ///< the edit mode flag
    double m_normalizedLength; ///< the normalized direction vector length
    QPointF m_origin;          ///< the pattern handle origin
};

#endif // _KARBONPATTERNEDITSTRATEGY_H_

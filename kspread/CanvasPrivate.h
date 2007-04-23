/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2006 Robert Knight <robertknight@gmail.com>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef CANVAS_PRIVATE_H
#define CANVAS_PRIVATE_H

#include <QPen>
#include <QPoint>

#include "Canvas.h"

class QLabel;

class KoShapeManager;
class KoToolProxy;

namespace KSpread
{
class CellEditor;
class CellWindow;
class ComboboxLocationEditWidget;
class EditWidget;
class View;

class Canvas::Private
{
public:
    ComboboxLocationEditWidget *posWidget;
    KSpread::EditWidget *editWidget;
    KSpread::CellEditor *cellEditor;

    View *view;
// FIXME Stefan: Still needed?
//     QTimer* scrollTimer;

    // Non visible range left from current screen
    // Example: If the first visible column is 'E', then xOffset stores
    // the width of the invisible columns 'A' to 'D'.
    double xOffset;

    // Non visible range on top of the current screen
    // Example: If the first visible row is '5', then yOffset stores
    // the height of the invisible rows '1' to '4'.
    double yOffset;

    // Used to draw the grey grid that is usually only visible on the
    // screen, but not by printing on paper.
    QPen defaultGridPen;

    // see setLastEditorWithFocus, lastEditorWithFocus
    Canvas::EditorType focusEditorType;

    QLabel *validationInfo;

    // true if the user is to choose a cell.
    bool chooseCell;

    // True when the mouse button is pressed
    bool mousePressed;
    bool dragging;

    bool mouseSelectedObject;
    bool drawContour;
    ModifyType modType;

    //---- stuff needed for resizing ----
    /// object which gets resized
    EmbeddedObject *m_resizeObject;
    /// ratio of the object ( width / height )
    double m_ratio;
    bool m_isResizing;
    /// The last position of the mouse during moving
    QPointF m_origMousePos;

    //---- stuff needed for moving ----
    bool m_isMoving;
    QPointF m_moveStartPoint;

    /// size of the object at when resizing is started
    QRectF m_rectBeforeResize;
    /// Start position for move with mouse
    QPointF m_moveStartPosMouse;

    /// object which is selected and should be shown above all the other objects
    EmbeddedObject * m_objectDisplayAbove;

   // bool mouseOverHighlightRangeSizeGrip;

    // The row and column of 1) the last cell under mouse pointer, 2) the last focused cell, and
    // the last spoken cell.
    int prevSpokenPointerRow;
    int prevSpokenPointerCol;
    int prevSpokenFocusRow;
    int prevSpokenFocusCol;
    int prevSpokenRow;
    int prevSpokenCol;

    // flake
    KoShapeManager* shapeManager;
    KoToolProxy* toolProxy;
};

} //namespace KSpread
#endif

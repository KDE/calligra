// This file is part of the KDE project
// SPDX-FileCopyrightText: 2006-2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
// SPDX-FileCopyrightText: 2006 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
// SPDX-FileCopyrightText: 2002-2004 Ariya Hidayat <ariya@kde.org>
// SPDX-FileCopyrightText: 1999-2003 Laurent Montel <montel@kde.org>
// SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
// SPDX-FileCopyrightText: 2002-2003 Philipp Mueller <philipp.mueller@gmx.de>
// SPDX-FileCopyrightText: 2002-2003 John Dailey <dailey@vt.edu>
// SPDX-FileCopyrightText: 1999-2003 David Faure <faure@kde.org>
// SPDX-FileCopyrightText: 1999-2001 Simon Hausmann <hausmann@kde.org>
// SPDX-FileCopyrightText: 1998-2000 Torben Weis <weis@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef CALLIGRA_SHEETS_CELL_TOOL_BASE_P
#define CALLIGRA_SHEETS_CELL_TOOL_BASE_P

// Sheets
#include "CellToolBase.h"

class KUndo2Command;

namespace Calligra
{
namespace Sheets
{
class Actions;
class CellEditor;
class Sheet;

class Q_DECL_HIDDEN CellToolBase::Private
{
public:
    Private(CellToolBase *parent)
        : q(parent)
    {
    }

    // Inline editor
    CellEditor *cellEditor;
    // External editor
    Calligra::Sheets::ExternalEditor *externalEditor;
    // Actions with extended names for the popup menu
    QHash<QString, QAction *> popupMenuActions;
    // Initialization flag.
    bool initialized;
    // For "Selection List..." action
    Editor lastEditorWithFocus;

    QMultiHash<int, QString> wordCollection;
    Actions *actions;

public:
    void updateEditor(const Cell &cell);
    /** Directly triggers an action. */
    void triggerAction(const QString &name);

    QPoint moveBy(const QPoint &point, int dx, int dy);
    QPoint destinationForKey(QKeyEvent *event);
    void moveToDestination(QPoint destination, bool extendSelection);
    bool handleMovementKeys(QKeyEvent *event);

    bool formatKeyPress(QKeyEvent *event);

    /** Which cell to move to when moving in this direction? */
    QPoint visibleCellInDirection(QPoint point, Sheet *sheet, Calligra::Sheets::MoveTo direction);
    Cell nextMarginCellInDirection(const Cell &cell, Calligra::Sheets::MoveTo direction);

    void paintSelection(QPainter &painter, const QRectF &viewRect);

    /**
     * Paint the highlighted ranges of cells.  When the user is editing a formula in a text box,
     * cells and ranges referenced in the formula are highlighted on the canvas.
     * @param painter The painter on which to draw the highlighted ranges
     * @param viewRect The area currently visible on the canvas
     */
    void paintReferenceSelection(QPainter &painter, const QRectF &viewRect);

    /**
     * helper function in drawing the marker and choose marker.
     * @param marker the rectangle that represents the marker being drawn
     *               (cell coordinates)
     * @param viewRect the visible area on the canvas
     * @param positions output parameter where the viewable left, top, right, and
     *                  bottom of the marker will be.  They are stored in the array
     *                  in that order, and take into account cropping due to part
     *                  of the marker being off screen.  This array should have
     *                  at least a size of 4 pre-allocated.
     * @param paintSides booleans indicating whether a particular side is visible.
     *                   Again, these are in the order left, top, right, bottom.
     *                   This should be preallocated with a size of at least 4.
     */
    void retrieveMarkerInfo(const QRect &marker, const QRectF &viewRect, double positions[], bool paintSides[]);

    QList<QAction *> popupActionList() const;
    void createPopupMenuActions();

private:
    CellToolBase *q;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_CELL_TOOL_BASE

/* This file is part of the KDE project
   Copyright 2006-2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2006 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
   Copyright 2002-2004 Ariya Hidayat <ariya@kde.org>
   Copyright 1999-2003 Laurent Montel <montel@kde.org>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2002-2003 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 2002-2003 John Dailey <dailey@vt.edu>
   Copyright 1999-2003 David Faure <faure@kde.org>
   Copyright 1999-2001 Simon Hausmann <hausmann@kde.org>
   Copyright 1998-2000 Torben Weis <weis@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or(at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_CELL_TOOL_BASE_P
#define KSPREAD_CELL_TOOL_BASE_P

// KSpread
#include "CellToolBase.h"

// dialogs
#include "dialogs/FindDialog.h"
#include "dialogs/FormulaDialog.h"

// KDE

// Qt
#include <QAction>
#include <QHash>

class QGridLayout;
class QToolButton;
class QUndoCommand;
class KFind;
class KReplace;

class CharSelectDia;

namespace KSpread
{
class CellEditor;
class CellToolOptionWidget;
class Sheet;

class CellToolBase::Private
{
public:
    Private(CellToolBase* parent) : q(parent) {}

    // Inline editor
    CellEditor* cellEditor;
    // Insert special character dialog
    CharSelectDia* specialCharDialog;
    // Option widget elements
    CellToolOptionWidget *optionWidget;
    QPointer<FormulaDialog> formulaDialog;
    // Actions with extended names for the popup menu
    QHash<QString, QAction*> popupMenuActions;
    // Initialization flag.
    bool initialized;
    // For "Selection List..." action
    QMenu* popupListChoose;
    Editor lastEditorWithFocus;

    // Find and Replace context. We remember the options and
    // the strings used previously.
    long findOptions;
    QStringList findStrings;
    QStringList replaceStrings;
    FindOption::searchTypeValue typeValue;
    FindOption::searchDirectionValue directionValue;
    // Current "find" operation
    KFind* find;
    KReplace* replace;
    QUndoCommand* replaceCommand;
    int findLeftColumn;
    int findRightColumn;
    int findTopRow;
    int findBottomRow;
    QPoint findStart;
    QPoint findPos;
    QPoint findEnd;

    struct {
        Sheet * currentSheet;
        Sheet * firstSheet;
    } searchInSheets;

public:
    void updateEditor(const Cell& cell);
    void updateActions(const Cell& cell);
    void setProtectedActionsEnabled(bool enable);

    void processEnterKey(QKeyEvent *event);
    void processArrowKey(QKeyEvent *event);
    void processEscapeKey(QKeyEvent *event);
    bool processHomeKey(QKeyEvent *event);
    bool processEndKey(QKeyEvent *event);
    bool processPriorKey(QKeyEvent *event);
    bool processNextKey(QKeyEvent *event);
    void processOtherKey(QKeyEvent *event);
    bool processControlArrowKey(QKeyEvent *event);
    bool formatKeyPress(QKeyEvent *event);

    /**
     * returns the rect that needs to be redrawn
     */
    QRect moveDirection(KSpread::MoveTo direction, bool extendSelection);

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
    void retrieveMarkerInfo(const QRect &marker, const QRectF &viewRect,
                            double positions[], bool paintSides[]);

    QList<QAction*> popupActionList() const;
    void createPopupMenuActions();


    /**
     * \ingroup UI
     * Tests for text values in the selection cursor's column, that are
     * different to the one at the selection cursor.
     * Used to create a popup menu consisting of text values to choose from.
     * \param selection the selection of cells to work on
     * \return \c true if there are text values in the selection cursor's column
     */
    bool testListChoose(Selection *selection) const;

private:
    CellToolBase* q;
};

} // namespace KSpread

#endif // KSPREAD_CELL_TOOL_BASE

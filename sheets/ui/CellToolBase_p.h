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

// dialogs
#include "dialogs/FindDialog.h"
#include "dialogs/FormulaDialog.h"

// Qt
#include <QAction>
#include <QHash>

class KUndo2Command;
class KFind;
class KReplace;


namespace Calligra
{
namespace Sheets
{
class CharacterSelectDialog;
class CellEditor;
class Sheet;

class Q_DECL_HIDDEN CellToolBase::Private
{
public:
    Private(CellToolBase* parent) : q(parent) {}

    // Inline editor
    CellEditor* cellEditor;
    // External editor
    Calligra::Sheets::ExternalEditor* externalEditor;
    // Insert special character dialog
    CharacterSelectDialog* specialCharDialog;
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
    KUndo2Command* replaceCommand;
    int findLeftColumn;
    int findRightColumn;
    int findTopRow;
    int findBottomRow;
    QPoint findStart;
    QPoint findPos;
    QPoint findEnd;
    QHash<int, QString> wordCollection;

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
    QRect moveDirection(Calligra::Sheets::MoveTo direction, bool extendSelection);

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

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_CELL_TOOL_BASE

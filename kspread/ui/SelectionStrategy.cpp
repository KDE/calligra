/* This file is part of the KDE project
   Copyright 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#include "SelectionStrategy.h"

#include "Limits.h"
#include "Selection.h"
#include "Sheet.h"

#include <KoCanvasBase.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoToolBase.h>

using namespace KSpread;

class SelectionStrategy::Private
{
public:
};

SelectionStrategy::SelectionStrategy(KoToolBase *parent, Selection *selection,
                                     const QPointF documentPos, Qt::KeyboardModifiers modifiers)
        : AbstractSelectionStrategy(parent, selection, documentPos, modifiers)
        , d(new Private)
{
    const KoShape *shape = tool()->canvas()->shapeManager()->selection()->firstSelectedShape();
    const QPointF position = documentPos - (shape ? shape->position() : QPointF(0.0, 0.0));

    // Extend selection, if control modifier is pressed.
    // In which cell did the user click?
    double xpos;
    double ypos;
    int col = this->selection()->activeSheet()->leftColumn(position.x(), xpos);
    int row = this->selection()->activeSheet()->topRow(position.y(), ypos);
    // Check boundaries.
    if (col > KS_colMax || row > KS_rowMax) {
        kDebug(36005) << "col or row is out of range:" << "col:" << col << " row:" << row;
    } else {
        if (selection->referenceSelectionMode()) {
            selection->emitRequestFocusEditor();
        } else {
            selection->emitCloseEditor(true);
        }
        if (modifiers & Qt::ControlModifier) {
            this->selection()->extend(QPoint(col, row), this->selection()->activeSheet());
        } else {
            this->selection()->initialize(QPoint(col, row), this->selection()->activeSheet());
        }
    }
    tool()->repaintDecorations();
}

SelectionStrategy::~SelectionStrategy()
{
    delete d;
}

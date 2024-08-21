/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DragAndDropStrategy.h"

#include <KoCanvasBase.h>
#include <KoToolBase.h>

#include <QDrag>
#include <QMimeData>
#include <QWidget>

#include "core/Cell.h"
#include "core/Sheet.h"
#include "engine/calligra_sheets_limits.h"

#include "../CellToolBase.h"
#include "../Selection.h"
#include "../commands/CopyCommand.h"

using namespace Calligra::Sheets;

class Q_DECL_HIDDEN DragAndDropStrategy::Private
{
public:
    Private()
        : started(false)
    {
    }

    Cell cell;
    QPointF lastPoint;
    bool started;
};

DragAndDropStrategy::DragAndDropStrategy(CellToolBase *cellTool, const QPointF &documentPos, Qt::KeyboardModifiers modifiers)
    : AbstractSelectionStrategy(cellTool, documentPos, modifiers)
    , d(new Private)
{
    d->lastPoint = documentPos;
    Selection *const selection = this->selection();
    // const KoShape *shape = tool()->canvas()->shapeManager()->selection()->firstSelectedShape();
    const QPointF position = documentPos /*- (shape ? shape->position() : QPointF(0.0, 0.0))*/;

    // In which cell did the user click?
    qreal xpos;
    qreal ypos;
    int col = selection->activeSheet()->leftColumn(position.x(), xpos);
    int row = selection->activeSheet()->topRow(position.y(), ypos);
    // Check boundaries.
    if (col > KS_colMax || row > KS_rowMax) {
        debugSheetsUI << "col or row is out of range:"
                      << "col:" << col << " row:" << row;
    } else {
        d->cell = Cell(selection->activeSheet(), col, row);
    }
}

DragAndDropStrategy::~DragAndDropStrategy()
{
    delete d;
}

void DragAndDropStrategy::handleMouseMove(const QPointF &documentPos, Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(modifiers);
    if (d->started)
        return;
    const QPointF position = documentPos - cellTool()->offset();
    d->lastPoint = position;

    // In which cell did the user click?
    qreal xpos;
    qreal ypos;
    int col = selection()->activeSheet()->leftColumn(position.x(), xpos);
    int row = selection()->activeSheet()->topRow(position.y(), ypos);
    // Check boundaries.
    if (col > KS_colMax || row > KS_rowMax) {
        debugSheetsUI << "col or row is out of range:"
                      << "col:" << col << " row:" << row;
    } else if (d->cell == Cell(selection()->activeSheet(), col, row)) {
    } else {
        QMimeData *mimeData = new QMimeData();
        mimeData->setText(CopyCommand::saveAsPlainText(*selection()));
        QString snippet = CopyCommand::saveAsSnippet(*selection());
        mimeData->setData("application/x-calligra-sheets-snippet", snippet.toUtf8());

        QDrag *drag = new QDrag(tool()->canvas()->canvasWidget());
        drag->setMimeData(mimeData);
        drag->exec();
        d->started = true;
    }
}

KUndo2Command *DragAndDropStrategy::createCommand()
{
    // const KoShape *shape = tool()->canvas()->shapeManager()->selection()->firstSelectedShape();
    const QPointF position = d->lastPoint /*- (shape ? shape->position() : QPointF(0.0, 0.0))*/;

    // In which cell did the user click?
    qreal xpos;
    qreal ypos;
    int col = selection()->activeSheet()->leftColumn(position.x(), xpos);
    int row = selection()->activeSheet()->topRow(position.y(), ypos);
    // Check boundaries.
    if (col > KS_colMax || row > KS_rowMax) {
        debugSheetsUI << "col or row is out of range:"
                      << "col:" << col << " row:" << row;
    } else if (d->cell == Cell(selection()->activeSheet(), col, row)) {
        selection()->initialize(QPoint(col, row), selection()->activeSheet());
    }
    return nullptr;
}

bool DragAndDropStrategy::dragStarted() const
{
    return d->started;
}

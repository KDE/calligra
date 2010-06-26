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

#include "DragAndDropStrategy.h"

#include "Cell.h"
#include "kspread_limits.h"
#include "Selection.h"
#include "Sheet.h"

#include <KoCanvasBase.h>
#include <KoToolBase.h>
#include <KoShape.h>
#include <KoSelection.h>
#include <KoShapeManager.h>

#include <QBuffer>
#include <QDomDocument>
#include <QDrag>
#include <QMimeData>
#include <QTextStream>

#include "commands/CopyCommand.h"

using namespace KSpread;

class DragAndDropStrategy::Private
{
public:
    Private() : started(false) { }

    Cell cell;
    QPointF lastPoint;
    bool started;
};

DragAndDropStrategy::DragAndDropStrategy(CellToolBase *cellTool,
        const QPointF documentPos, Qt::KeyboardModifiers modifiers)
        : AbstractSelectionStrategy(cellTool, documentPos, modifiers)
        , d(new Private)
{
    d->lastPoint = documentPos;
    Selection *const selection = this->selection();
    const KoShape *shape = tool()->canvas()->shapeManager()->selection()->firstSelectedShape();
    const QPointF position = documentPos - (shape ? shape->position() : QPointF(0.0, 0.0));

    // In which cell did the user click?
    double xpos;
    double ypos;
    int col = selection->activeSheet()->leftColumn(position.x(), xpos);
    int row = selection->activeSheet()->topRow(position.y(), ypos);
    // Check boundaries.
    if (col > KS_colMax || row > KS_rowMax) {
        kDebug(36005) << "col or row is out of range:" << "col:" << col << " row:" << row;
    } else {
        d->cell = Cell(selection->activeSheet(), col, row);
    }
}

DragAndDropStrategy::~DragAndDropStrategy()
{
    delete d;
}

void DragAndDropStrategy::handleMouseMove(const QPointF& documentPos, Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(modifiers);
    if (d->started)
        return;
    d->lastPoint = documentPos;
    const KoShape *shape = tool()->canvas()->shapeManager()->selection()->firstSelectedShape();
    const QPointF position = documentPos - (shape ? shape->position() : QPointF(0.0, 0.0));

    // In which cell did the user click?
    double xpos;
    double ypos;
    int col = selection()->activeSheet()->leftColumn(position.x(), xpos);
    int row = selection()->activeSheet()->topRow(position.y(), ypos);
    // Check boundaries.
    if (col > KS_colMax || row > KS_rowMax) {
        kDebug(36005) << "col or row is out of range:" << "col:" << col << " row:" << row;
    } else if (d->cell == Cell(selection()->activeSheet(), col, row)) {
    } else {
        QDomDocument doc = CopyCommand::saveAsXml(*selection(), true);

        // Save to buffer
        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly);
        QTextStream str(&buffer);
        str.setCodec("UTF-8");
        str << doc;
        buffer.close();

        QMimeData* mimeData = new QMimeData();
        mimeData->setText(CopyCommand::saveAsPlainText(*selection()));
        mimeData->setData("application/x-kspread-snippet", buffer.buffer());

        QDrag *drag = new QDrag(tool()->canvas()->canvasWidget());
        drag->setMimeData(mimeData);
        drag->exec();
        d->started = true;
    }
}

QUndoCommand* DragAndDropStrategy::createCommand()
{
    const KoShape *shape = tool()->canvas()->shapeManager()->selection()->firstSelectedShape();
    const QPointF position = d->lastPoint - (shape ? shape->position() : QPointF(0.0, 0.0));

    // In which cell did the user click?
    double xpos;
    double ypos;
    int col = selection()->activeSheet()->leftColumn(position.x(), xpos);
    int row = selection()->activeSheet()->topRow(position.y(), ypos);
    // Check boundaries.
    if (col > KS_colMax || row > KS_rowMax) {
        kDebug(36005) << "col or row is out of range:" << "col:" << col << " row:" << row;
    } else if (d->cell == Cell(selection()->activeSheet(), col, row)) {
        selection()->initialize(QPoint(col, row), selection()->activeSheet());
    }
    return 0;
}

bool DragAndDropStrategy::dragStarted() const
{
    return d->started;
}

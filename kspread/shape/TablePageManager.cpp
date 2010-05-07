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

#include "TablePageManager.h"

#include "TableShape.h"

#include <Sheet.h>

#include <KoShapeContainer.h>
#include <KoTextDocumentLayout.h>
#include <KoTextShapeData.h>

#include <kdebug.h>

#include <QList>

using namespace KSpread;

class TablePageManager::Private
{
public:
    TableShape* master;
    QList<TableShape*> pages;
};

TablePageManager::TablePageManager(TableShape* master)
        : PageManager(master->sheet())
        , d(new Private)
{
    d->master = master;
}

TablePageManager::~TablePageManager()
{
    delete d;
}

QSizeF TablePageManager::size(int page) const
{
    if (page < 1 || page > d->pages.count() || !d->pages[page - 1]->KoShape::parent())
        return QSizeF();
    return (page == 1) ? d->master->size() : d->pages[page - 1]->KoShape::parent()->size();
}

void TablePageManager::clearPages()
{
    qDeleteAll(d->pages);
    d->pages.clear();
}

void TablePageManager::insertPage(int page)
{
    if (page <= 1 || page > d->pages.count()) {
        return;
    }
    TableShape* const shape = static_cast<TableShape*>(d->pages[page - 1]);
    const QRect cellRange = this->cellRange(page);
    shape->setVisibleCellRange(cellRange);
    shape->KoShape::setSize(shape->sheet()->cellCoordinatesToDocument(cellRange).size());
}

void TablePageManager::preparePage(int page)
{
    // The first page is the master page, which always exists.
    if (page == 1) {
        return;
    }
    KoTextShapeData* const data = static_cast<KoTextShapeData*>(d->master->KoShape::parent()->userData());
    if (!data) {
        // not embedded in a text shape
        return;
    }
    Q_CHECK_PTR(data);
    QTextDocument* const document = data->document();
    Q_CHECK_PTR(document);
    KoTextDocumentLayout* const layout = qobject_cast<KoTextDocumentLayout*>(document->documentLayout());
    Q_CHECK_PTR(layout);
    const QList<KoShape*> textShapes = layout->shapes();
    const int masterIndex = textShapes.indexOf(d->master);
    if (masterIndex < 0)
        return;  // huh?
    KoShapeContainer* const textShape = dynamic_cast<KoShapeContainer*>(textShapes.value(masterIndex + page - 1));
    if (textShape) {
        TableShape* const shape = new TableShape(d->master->columns(), d->master->rows());
        const TableShape* predecessor = d->pages[page - 1];
        shape->setPosition(predecessor->position() + QPointF(0.0, predecessor->size().height()));
        d->pages.append(shape);
        textShape->addShape(shape);
    }
}

/* This file is part of the KDE project
*
* Copyright (C) 2011 Paul Mendez <paulestebanms@gmail.com>
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

#include "KPrSlidesSorterDocumentModel.h"

#include <KoPADocument.h>
#include <QMimeData>
#include "KPrViewModeSlidesSorter.h"
#include <KoPAPageBase.h>

KPrSlidesSorterDocumentModel::KPrSlidesSorterDocumentModel(KPrViewModeSlidesSorter *viewModeSlidesSorter, QWidget *parent, KoPADocument *document)
   : QAbstractListModel(parent)
   , m_viewModeSlidesSorter(viewModeSlidesSorter)
{
    setDocument(document);
    setSupportedDragActions(Qt::MoveAction);
}

KPrSlidesSorterDocumentModel::~KPrSlidesSorterDocumentModel()
{
}

void KPrSlidesSorterDocumentModel::setDocument(KoPADocument *document)
{
    m_document = document;
    reset();
}

QVariant KPrSlidesSorterDocumentModel::data(const QModelIndex &index, int role) const
{
    if (! index.isValid() || !m_document) {
        return QVariant();
    }

    Q_ASSERT(index.model() == this);

    KoPAPageBase *page = m_document->pageByIndex(index.row(), false);

    switch (role) {
        case Qt::DisplayRole:
        {
            QString name = i18n("Unknown");
            if (page)
            {
                name = page->name ();
                if (name.isEmpty())
                {
                    //Default case
                    name = i18n("Slide %1",  m_document->pageIndex(page) + 1);
                }
            }
            return name;
        }
        case Qt::DecorationRole:
        {
            return QIcon(page->thumbnail(m_viewModeSlidesSorter->iconSize()));
        }
        default:
            return QVariant();
    }
}

int KPrSlidesSorterDocumentModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (!m_document) {
        return 0;
    }

    return m_document->pages(false).count();
}

QStringList KPrSlidesSorterDocumentModel::mimeTypes() const
{
    return QStringList() << "application/x-koffice-sliderssorter";
}

QMimeData * KPrSlidesSorterDocumentModel::mimeData(const QModelIndexList &indexes) const
{
    QModelIndex page = indexes.first();
    QByteArray ssData = QVariant(page.row()).toByteArray();
    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-koffice-sliderssorter", ssData);
    return mimeData;
}

Qt::DropActions KPrSlidesSorterDocumentModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

bool KPrSlidesSorterDocumentModel::removeRows(int row, int count, const QModelIndex &parent)
{
    bool success = true;
    beginRemoveRows(parent,row, row + count- 1);
    endRemoveRows();

    return success;
}

Qt::ItemFlags KPrSlidesSorterDocumentModel::flags(const QModelIndex &index) const
{
    if (!m_document) {
        return 0;
    }

    if (index.isValid()) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;//| defaultFlags;
    } else {
        return Qt::ItemIsDropEnabled; //| defaultFlags;
    }
}

void KPrSlidesSorterDocumentModel::update()
{
    emit layoutAboutToBeChanged();
    emit layoutChanged();
}

#include "KPrSlidesSorterDocumentModel.moc"

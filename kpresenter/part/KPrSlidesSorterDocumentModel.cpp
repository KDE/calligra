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
#include <KoPAViewBase.h>
#include <KoPAPage.h>
#include <KoPAPageDeleteCommand.h>
#include <KoPAPageInsertCommand.h>
#include <KoPAPageMoveCommand.h>


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
    // check if there is data to encode
    if (! indexes.count()) {
        return 0;
    }

    // check if we support a format
    QStringList types = mimeTypes();
    if (types.isEmpty()) {
        return 0;
    }

    QMimeData *data = new QMimeData();
    QString format = types[0];
    QByteArray encoded;
    QDataStream stream(&encoded, QIODevice::WriteOnly);

    // encode the data
    QModelIndexList::ConstIterator it = indexes.begin();
    for (; it != indexes.end(); ++it) {
        stream << QVariant::fromValue((*it).row());
    }

    data->setData(format, encoded);
    return data;
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

    Qt::ItemFlags defaultFlags = QAbstractListModel::flags (index);

    if (index.isValid()) {
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
    } else {
        return Qt::ItemIsDropEnabled | defaultFlags;
    }
}

void KPrSlidesSorterDocumentModel::update()
{
    emit layoutAboutToBeChanged();
    emit layoutChanged();
}

bool KPrSlidesSorterDocumentModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if (action == Qt::IgnoreAction) {
        return true;
    }

    if (!data->hasFormat("application/x-koffice-sliderssorter")) {
        return false;
    }

    if (column > 0) {
        return false;
    }

    QByteArray encoded = data->data("application/x-koffice-sliderssorter");
    QDataStream stream(&encoded, QIODevice::ReadOnly);
    QList<KoPAPageBase *> slides;

    // decode the data
    while (! stream.atEnd()) {
        QVariant v;
        stream >> v;
        KoPAPageBase *page = m_document->pageByIndex(v.toInt (),false);
        if (page) {
            slides.append(page);
        }
    }

    if (slides.empty ()) {
        return false;
    }

    int beginRow;

    if (row != -1) {
        beginRow = row;
    } else if (parent.isValid ()) {
        beginRow = parent.row ();

    } else {
        beginRow = rowCount (QModelIndex());
    }

    KoPAPageBase * pageAfter = 0;

    if ((beginRow - 1) >= 0) {
        pageAfter = m_document->pageByIndex(beginRow - 1,false);
    }


    if (!slides.empty ()) {
        KoPAPageMoveCommand *command = new KoPAPageMoveCommand(m_document, slides, pageAfter);
        m_document->addCommand(command);
    }

    m_viewModeSlidesSorter->view()->setActivePage (slides.first ());

    return true;
}

#include "KPrSlidesSorterDocumentModel.moc"

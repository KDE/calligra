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

#include "KPrViewModeSlidesSorter.h"
#include "KPrDocument.h"
#include "commands/KPrDeleteSlidesCommand.h"
#include "StageDebug.h"

//Calligra headers
#include <KoPADocument.h>
#include <KoPAPageBase.h>
#include <KoPAViewBase.h>
#include <KoPAView.h>
#include <KoPAOdfPageSaveHelper.h>
#include <KoDrag.h>
#include <KoPAPageMoveCommand.h>
#include <KoShapeRenameCommand.h>
#include <KoIcon.h>

//Qt Headers
#include <QMimeData>
#include <QApplication>
#include <QMenu>

KPrSlidesSorterDocumentModel::KPrSlidesSorterDocumentModel(KPrViewModeSlidesSorter *viewModeSlidesSorter, QWidget *parent, KoPADocument *document)
   : QAbstractListModel(parent)
   , m_viewModeSlidesSorter(viewModeSlidesSorter)
{
    setDocument(document);
}

Qt::DropActions KPrSlidesSorterDocumentModel::supportedDragActions() const
{
    return Qt::MoveAction;
}

KPrSlidesSorterDocumentModel::~KPrSlidesSorterDocumentModel()
{
}

void KPrSlidesSorterDocumentModel::setDocument(KoPADocument *document)
{
    beginResetModel();
    m_document = document;
    endResetModel();
    if (m_document) {
        connect(m_document, SIGNAL(pageAdded(KoPAPageBase*)), this, SLOT(update()));
        connect(m_document, SIGNAL(pageRemoved(KoPAPageBase*)), this, SLOT(update()));
        connect(m_document, SIGNAL(update(KoPAPageBase*)), this, SLOT(update()));
    }
}

QModelIndex KPrSlidesSorterDocumentModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!m_document) {
        return QModelIndex();
    }

    // check if parent is root node
    if(!parent.isValid()) {
        if(row >= 0 && row < m_document->pages(false).count()) {
            return createIndex(row, column, m_document->pages(false).at(row));
        }
    }
    return QModelIndex();
}

QVariant KPrSlidesSorterDocumentModel::data(const QModelIndex &index, int role) const
{
    if (! index.isValid() || !m_document) {
        return QVariant();
    }

    Q_ASSERT(index.model() == this);
    KoPAPageBase *page = pageByIndex(index);

    switch (role) {
        case Qt::DisplayRole:
        {
            QString name = i18n("Unknown");
            if (page) {
                name = page->name();
                if (name.isEmpty()) {
                    //Default case
                    name = i18n("Slide %1", m_document->pageIndex(page) + 1);
                }
            }
            return name;
        }
        case Qt::DecorationRole:
        {
            return QIcon(page->thumbnail(m_viewModeSlidesSorter->iconSize()));
        }
        case Qt::EditRole:
        {
            return page->name();
        }
        default:
            return QVariant();
    }
}

bool KPrSlidesSorterDocumentModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || !m_document) {
        return false;
    }

    Q_ASSERT(index.model() == this);
    Q_ASSERT(index.internalPointer());

    KoShape *shape = static_cast<KoShape*>(index.internalPointer());
    switch (role)
    {
        case Qt::EditRole:
        {
            KUndo2Command *cmd = new KoShapeRenameCommand(shape, value.toString());
            // TODO 2.1 use different text for the command if e.g. it is a page/slide or layer
            m_document->addCommand(cmd);
            break;
        }
        default:
            return false;
    }

    emit dataChanged(index, index);
    return true;
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
    return QStringList() << "application/x-calligra-sliderssorter";
}

QMimeData * KPrSlidesSorterDocumentModel::mimeData(const QModelIndexList &indexes) const
{
    // check if there is data to encode
    if (!indexes.count()) {
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
    //  order slides
    QMap<int, KoPAPageBase*> map;
    for( ; it != indexes.end(); ++it) {
        map.insert(m_document->pages(false).indexOf((KoPAPageBase*)it->internalPointer()),
                   (KoPAPageBase*)it->internalPointer());
    }

    QList<KoPAPageBase *> slides = map.values();

    foreach (KoPAPageBase *slide, slides) {
        stream << QVariant::fromValue(qulonglong((void*)slide));
    }

    data->setData(format, encoded);
    return data;
}

Qt::DropActions KPrSlidesSorterDocumentModel::supportedDropActions() const
{
    return Qt::MoveAction | Qt::CopyAction;
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
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEditable | defaultFlags;
    }
    else {
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

    if (!data->hasFormat("application/x-calligra-sliderssorter") || (column > 0)) {
        return false;
    }

    QByteArray encoded = data->data("application/x-calligra-sliderssorter");
    QDataStream stream(&encoded, QIODevice::ReadOnly);
    QList<KoPAPageBase *> slides;

    // decode the data
    while (!stream.atEnd()) {
        QVariant v;
        stream >> v;
        slides.append(static_cast<KoPAPageBase*>((void*)v.value<qulonglong>()));
    }

    if (slides.empty()) {
        return false;
    }

    int beginRow;

    if (row != -1) {
        beginRow = row;
    }
    else if (parent.isValid()) {
        beginRow = parent.row();
    }
    else {
        beginRow = rowCount(QModelIndex());
    }

    KoPAPageBase *pageAfter = 0;

    if ((beginRow - 1) >= 0) {
        pageAfter = m_document->pageByIndex(beginRow - 1,false);
    }

    if (!slides.empty()) {
        doDrop(slides, pageAfter, action);
    }

    return true;
}

void KPrSlidesSorterDocumentModel::doDrop(QList<KoPAPageBase *> slides, KoPAPageBase *pageAfter, Qt::DropAction action)
{
     Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();
     bool enableMove = true;

     foreach (KoPAPageBase *slide, slides) {
         if (!m_document->pages(false).contains(slide)) {
             KoPAPageBase *newSlide = slide;
             slides.replace(slides.indexOf(slide), newSlide);
             enableMove = false;
             break;
         }
     }

     if (((modifiers & Qt::ControlModifier) == 0) &&
         ((modifiers & Qt::ShiftModifier) == 0)) {
            QMenu popup;
            QString seq = QKeySequence(Qt::ShiftModifier).toString();
            seq.chop(1);
            QAction *popupMoveAction = new QAction(i18n("&Move Here") + '\t' + seq, this);
            popupMoveAction->setIcon(koIcon("go-jump"));
            seq = QKeySequence(Qt::ControlModifier).toString();
            seq.chop(1);
            QAction *popupCopyAction = new QAction(i18n("&Copy Here") + '\t' + seq, this);
            popupCopyAction->setIcon(koIcon("edit-copy"));
            seq = QKeySequence( Qt::ControlModifier + Qt::ShiftModifier ).toString();
            seq.chop(1);
            QAction *popupCancelAction = new QAction(i18n("C&ancel") + '\t' + QKeySequence(Qt::Key_Escape).toString(), this);
            popupCancelAction->setIcon(koIcon("process-stop"));

            if (enableMove) {
                popup.addAction(popupMoveAction);
            }
            popup.addAction(popupCopyAction);
            popup.addSeparator();
            popup.addAction(popupCancelAction);

            QAction *result = popup.exec(QCursor::pos());

            if (result == popupCopyAction) {
                action = Qt::CopyAction;
            }
            else if (result == popupMoveAction) {
                action = Qt::MoveAction;
            }
            else {
                return;
            }
     }
     else if ((modifiers & Qt::ControlModifier) != 0) {
         action = Qt::CopyAction;
     }
     else if ((modifiers & Qt::ShiftModifier) != 0) {
         action = Qt::MoveAction;
     }
     else {
         return;
     }

    switch (action) {
    case Qt::MoveAction: {
        //You can't move slides that not belong to the current document
        foreach (KoPAPageBase *slide, slides) {
            if (!m_document->pages(false).contains(slide)) {
                slides.removeAll(slide);
            }
        }
        if (slides.isEmpty()) {
            return;
        }
        moveSlides(slides, pageAfter);
        return;
    }
    case Qt::CopyAction: {
        copySlides(slides);
        m_viewModeSlidesSorter->view()->setActivePage(pageAfter);
        pasteSlides();
        m_viewModeSlidesSorter->view()->setActivePage(slides.first());
        m_viewModeSlidesSorter->selectSlides(slides);
        return;
    }
    default:
        debugStage << "Unknown action:" << (int)action;
        return;
    }
}

KoPAPageBase * KPrSlidesSorterDocumentModel::pageByIndex(const QModelIndex &index) const
{
    Q_ASSERT(index.internalPointer());
    return static_cast<KoPAPageBase*>(index.internalPointer());
}

bool KPrSlidesSorterDocumentModel::removeSlides(const QList<KoPAPageBase *> &slides)
{
    if (!slides.empty() && m_document->pages().count() > slides.count()) {
        KPrDocument *doc = static_cast<KPrDocument *>(m_document);
        KUndo2Command *cmd = new KPrDeleteSlidesCommand(doc, slides);
        if (cmd) {
            removeRows(m_document->pageIndex(slides.first()), slides.count(), QModelIndex());
            m_document->addCommand(cmd);
            return true;
        }
    }
    return false;
}

bool KPrSlidesSorterDocumentModel::addNewSlide()
{
    KoPAView *view = dynamic_cast<KoPAView *>(m_viewModeSlidesSorter->view());
    if (view) {
        view->insertPage();
        return true;
    }
    return false;
}

bool KPrSlidesSorterDocumentModel::copySlides(const QList<KoPAPageBase *> &slides)
{
    if (!slides.empty()) {
        // Copy Pages
        KoPAOdfPageSaveHelper saveHelper(m_document, slides);
        KoDrag drag;
        drag.setOdf(KoOdf::mimeType(m_document->documentType()), saveHelper);
        drag.addToClipboard();
        return true;
    }
    return false;
}

bool KPrSlidesSorterDocumentModel::pasteSlides()
{
    KoPAView *view = dynamic_cast<KoPAView *>(m_viewModeSlidesSorter->view());
    if (view) {
        view->pagePaste();
        return true;
    }
    return false;
}

bool KPrSlidesSorterDocumentModel::moveSlides(const QList<KoPAPageBase *> &slides, KoPAPageBase *pageAfter)
{
    KoPAPageMoveCommand *command = new KoPAPageMoveCommand(m_document, slides, pageAfter);
    if (command) {
        m_document->addCommand(command);
        m_viewModeSlidesSorter->view()->setActivePage(slides.first());
        m_viewModeSlidesSorter->selectSlides(slides);
        return true;
    }
    return false;
}

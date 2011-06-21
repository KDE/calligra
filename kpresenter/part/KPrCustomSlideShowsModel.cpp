/* This file is part of the KDE project
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

#include "KPrCustomSlideShowsModel.h"

//Calligra headers
#include "KPrCustomSlideShows.h"
#include "KPrDocument.h"
#include "KoPAPageBase.h"
#include "commands/KPrEditCustomSlideShowsCommand.h"
#include "commands/KPrAddCustomSlideShowCommand.h"
#include "commands/KPrDelCustomSlideShowCommand.h"
#include "commands/KPrRenameCustomSlideShowCommand.h"

//KDE headers
#include "KLocalizedString"
#include "KIcon"

//Qt headers
#include <QIcon>
#include <QMimeData>
#include <QApplication>
#include <QMenu>

KPrCustomSlideShowsModel::KPrCustomSlideShowsModel(KPrDocument *document, QObject *parent)
    : QAbstractListModel(parent)
    , m_customSlideShows(document->customSlideShows())
    , m_iconSize(QSize(200,200))
    , m_document(document)
{

}

KPrCustomSlideShowsModel::~KPrCustomSlideShowsModel(){
}

QVariant KPrCustomSlideShowsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !m_customSlideShows || m_currentCustomSlideShowName.isEmpty()) {
        return QVariant();
    }
    Q_ASSERT(index.model() == this);

    KoPAPageBase *page = m_customSlideShows->pageByIndex(m_currentCustomSlideShowName, index.row());

    switch (role) {
        case Qt::DisplayRole:
        {
            QString name = i18n("Unknown");
            if (page) {
                name = page->name ();
                if (name.isEmpty()) {
                    //Default case
                    name = i18n("Slide %1",  index.row());
                }
            }
            return name;
        }
        case Qt::DecorationRole:
        {
            return QIcon(page->thumbnail(m_iconSize));
        }
        default:
            return QVariant();
    }
}

int KPrCustomSlideShowsModel::rowCount(const QModelIndex &parent) const
{
    if (!m_currentCustomSlideShowName.isEmpty()) {
        if (!parent.isValid()) {
            return m_customSlideShows->getByName(m_currentCustomSlideShowName).count();
        }
    }

    return 0;
}

Qt::ItemFlags KPrCustomSlideShowsModel::flags(const QModelIndex &index) const
{
    if (m_currentCustomSlideShowName.isEmpty()) {
        return 0;
    }

    Qt::ItemFlags defaultFlags = QAbstractListModel::flags (index);

    if (index.isValid()) {
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
    }
    else {
        return Qt::ItemIsDropEnabled | defaultFlags;
    }
}

QModelIndex KPrCustomSlideShowsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (m_currentCustomSlideShowName.isEmpty()) {
        return QModelIndex();
    }

    // check if parent is root node
    if (!parent.isValid()) {
        if (row >= 0 && row < rowCount(QModelIndex())) {
            return createIndex(row, column, m_customSlideShows->pageByIndex(m_currentCustomSlideShowName, row));
        }
    }
    return QModelIndex();
}

QStringList KPrCustomSlideShowsModel::mimeTypes() const
{
    return QStringList() << "application/x-koffice-customslideshows";
}

QMimeData * KPrCustomSlideShowsModel::mimeData(const QModelIndexList &indexes) const
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
    for ( ; it != indexes.end(); ++it) {
        stream << QVariant::fromValue(qulonglong(it->internalPointer()));
    }

    data->setData(format, encoded);
    return data;
}

Qt::DropActions KPrCustomSlideShowsModel::supportedDropActions() const
{
    return Qt::MoveAction | Qt::CopyAction;
}

bool KPrCustomSlideShowsModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if (action == Qt::IgnoreAction) {
        return true;
    }

    if (data->hasFormat("application/x-koffice-sliderssorter") | data->hasFormat("application/x-koffice-customslideshows")) {

        if (column > 0) {
            return false;
        }

        QList<KoPAPageBase *> slides;

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

        if (data->hasFormat("application/x-koffice-sliderssorter")) {
            QByteArray encoded = data->data("application/x-koffice-sliderssorter");
            slides = decodeSlidesList(encoded);

            if (slides.empty()) {
                return false;
            }

            doCustomSlideShowAction(KPrCustomSlideShowsModel::SLIDES_ADD, slides, beginRow);
        }

        if (data->hasFormat("application/x-koffice-customslideshows")) {
            QByteArray encoded = data->data("application/x-koffice-customslideshows");
            slides = decodeSlidesList(encoded);

            if (slides.empty()) {
                return false;
            }

            doCustomSlideShowAction(KPrCustomSlideShowsModel::SLIDES_MOVE, slides, beginRow);
        }
        return true;
    }
    return false;
}

QList<KoPAPageBase *> KPrCustomSlideShowsModel::decodeSlidesList(QByteArray encoded)
{
    QList<KoPAPageBase *> slides;
    QDataStream stream(&encoded, QIODevice::ReadOnly);

    // decode the data
    while(!stream.atEnd()) {
        QVariant v;
        stream >> v;
        slides.append(static_cast<KoPAPageBase*>((void*)v.value<qulonglong>()));
    }

    if (slides.empty()) {
        return slides;
    }

    //order slides
    QMap<int, KoPAPageBase*> map;
    foreach (KoPAPageBase *slide, slides)
        map.insert(m_customSlideShows->indexByPage(m_currentCustomSlideShowName, slide), slide);
    slides = map.values();

    return slides;
}

void KPrCustomSlideShowsModel::setCustomSlideShows(KPrCustomSlideShows *customShows)
{
    m_customSlideShows = customShows;
    m_currentCustomSlideShowName.clear();
    reset();
}


QString KPrCustomSlideShowsModel::currentSlideShow()
{
    return m_currentCustomSlideShowName;
}

void KPrCustomSlideShowsModel::setCurrentSlideShow(QString name)
{
    if (!m_customSlideShows | (m_currentCustomSlideShowName == name)) {
        return;
    }
    if (m_customSlideShows->names().contains(name)) {
        m_currentCustomSlideShowName = name;
    }
    reset();
}

void KPrCustomSlideShowsModel::setCurrentSlideShow(int index)
{
    if (!m_customSlideShows) {
        return;
    }
    QString name = m_customSlideShows->names().value(index);
    setCurrentSlideShow(name);
}

void KPrCustomSlideShowsModel::setIconSize(QSize size)
{
    if (size != m_iconSize) {
        m_iconSize = size;
    }
}

QStringList KPrCustomSlideShowsModel::customShowsNamesList() const
{
    if (m_customSlideShows) {
        return m_customSlideShows->names();
    }

    return QStringList();
}

void KPrCustomSlideShowsModel::setDocument(KPrDocument *document)
{
    m_document = document;
    setCustomSlideShows(document->customSlideShows());
}

void KPrCustomSlideShowsModel::updateCustomShow(QString name, QList<KoPAPageBase *> newCustomShow)
{
    if (!m_customSlideShows) {
        return;
    }
    m_customSlideShows->update(name, newCustomShow);
    reset();
}

void KPrCustomSlideShowsModel::removeSlidesFromAll(QList<KoPAPageBase *> pages)
{
    if (!m_customSlideShows) {
        return;
    }
    m_customSlideShows->removeSlidesFromAll(pages);
    reset();
}

void KPrCustomSlideShowsModel::removeSlidesByIndexes(QModelIndexList pageIndexes)
{
    QList<KoPAPageBase *> slides;

    foreach (QModelIndex index, pageIndexes)
        slides.append(m_customSlideShows->pageByIndex(m_currentCustomSlideShowName, index.row()));

    doCustomSlideShowAction(KPrCustomSlideShowsModel::SLIDES_DELETE, slides, 0);
}

void KPrCustomSlideShowsModel::addSlides(QList<KoPAPageBase *> pages, int row)
{
    doCustomSlideShowAction(KPrCustomSlideShowsModel::SLIDES_ADD, pages, row);
}

bool KPrCustomSlideShowsModel::doCustomSlideShowAction(CustomShowActions c_action, QList<KoPAPageBase *> slides, int beginRow)
{
    bool updated = false;

    //get the slideshow
    QList<KoPAPageBase*> selectedSlideShow = m_customSlideShows->getByName(m_currentCustomSlideShowName);

    if (c_action == KPrCustomSlideShowsModel::SLIDES_ADD) {
        //insert the slides on the current custom show
        int i = beginRow;
        foreach(KoPAPageBase *page, slides) {
            //You can insert a slide just one time.
            if (!selectedSlideShow.contains(page)) {
                selectedSlideShow.insert(i, page);
                i++;
            }
        }
        updated = true;
    }
    else if (c_action == KPrCustomSlideShowsModel::SLIDES_MOVE) {
       //move the slides on the current custom show
       // slides order within the slides list is important to get the expected behaviour
       if (beginRow >= selectedSlideShow.count()) {
           beginRow = selectedSlideShow.count() - 1;
       }

        foreach(KoPAPageBase *page, slides)
        {
            int from = selectedSlideShow.indexOf(page);
            selectedSlideShow.move(from, beginRow);
        }
        updated = true;
    }
    else if (c_action == KPrCustomSlideShowsModel::SLIDES_DELETE) {
        //delete de slides on the current custom show
        foreach(KoPAPageBase *page, slides) {
            selectedSlideShow.removeAll(page);
        }
        updated = true;
    }
    else {
        updated = false;
    }

    if (updated) {
        //update the SlideShow with the resulting list
        KPrEditCustomSlideShowsCommand *command = new KPrEditCustomSlideShowsCommand(
                    m_document, this, m_currentCustomSlideShowName, selectedSlideShow);
        m_document->addCommand(command);
    }

    return updated;
}

void KPrCustomSlideShowsModel::addNewCustomShow(const QString &name)
{
    KPrAddCustomSlideShowCommand *command = new KPrAddCustomSlideShowCommand(m_document, this, name);
    m_document->addCommand(command);
}

void KPrCustomSlideShowsModel::renameCustomShow(const QString &oldName, const QString &newName)
{
    KPrRenameCustomSlideShowCommand *command = new KPrRenameCustomSlideShowCommand(m_document, this, oldName, newName);
    m_document->addCommand(command);
}

void KPrCustomSlideShowsModel::removeCustomShow(const QString &name)
{
    KPrDelCustomSlideShowCommand *command = new KPrDelCustomSlideShowCommand(m_document, this, name);
    m_document->addCommand(command);
}

void KPrCustomSlideShowsModel::updateCustomSlideShowsList(const QString &name)
{
    m_currentCustomSlideShowName.clear();
    if (m_customSlideShows->names().contains(name)) {
        m_currentCustomSlideShowName = name;
    }
    reset();
    emit customSlideShowsChanged();
}

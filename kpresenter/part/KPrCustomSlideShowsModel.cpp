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

#include "KPrCustomSlideShowsModel.h"

//Calligra headers
#include "KPrCustomSlideShows.h"
#include "KPrDocument.h"
#include "KoPAPageBase.h"
#include "commands/KPrEditCustomSlideShowsCommand.h"

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
    , m_customShows(document->customSlideShows())
    , m_iconSize(QSize(200,200))
    , m_document(document)
{

}

KPrCustomSlideShowsModel::~KPrCustomSlideShowsModel(){
}

QVariant KPrCustomSlideShowsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !m_customShows || m_currentSlideShowName.isEmpty()) {
        return QVariant();
    }
    Q_ASSERT(index.model() == this);

    KoPAPageBase *page = m_customShows->pageByIndex(m_currentSlideShowName, index.row());

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
    if (!m_currentSlideShowName.isEmpty()) {
        if (!parent.isValid())
            return m_customShows->getByName(m_currentSlideShowName).count();
    }

    return 0;
}

Qt::ItemFlags KPrCustomSlideShowsModel::flags(const QModelIndex &index) const
{
    if (m_currentSlideShowName.isEmpty()) {
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
    if(m_currentSlideShowName.isEmpty())
        return QModelIndex();

    // check if parent is root node
    if(!parent.isValid())
    {
        if(row >= 0 && row < rowCount(QModelIndex()))
            return createIndex(row, column, m_customShows->pageByIndex(m_currentSlideShowName, row));
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
    if( ! indexes.count() )
        return 0;

    // check if we support a format
    QStringList types = mimeTypes();
    if( types.isEmpty() )
        return 0;

    QMimeData *data = new QMimeData();
    QString format = types[0];
    QByteArray encoded;
    QDataStream stream(&encoded, QIODevice::WriteOnly);

    // encode the data
    QModelIndexList::ConstIterator it = indexes.begin();
    for( ; it != indexes.end(); ++it)
        stream << QVariant::fromValue( qulonglong( it->internalPointer() ) );

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
            QDataStream stream(&encoded, QIODevice::ReadOnly);

            // decode the data
            while( ! stream.atEnd() )
            {
                QVariant v;
                stream >> v;
                slides.append( static_cast<KoPAPageBase*>((void*)v.value<qulonglong>()));
            }

            if (slides.empty ()) {
                return false;
            }

            //order slides
            QMap<int, KoPAPageBase*> map;
            foreach (KoPAPageBase *slide, slides)
                map.insert(m_document->pageIndex(slide), slide);

            slides = map.values();

            doCustomShowAction(KPrCustomSlideShowsModel::SLIDES_ADD, slides, beginRow);
        }

        if (data->hasFormat("application/x-koffice-customslideshows")) {

            QByteArray encoded = data->data("application/x-koffice-customslideshows");
            QDataStream stream(&encoded, QIODevice::ReadOnly);

            // decode the data
            while( ! stream.atEnd() )
            {
                QVariant v;
                stream >> v;
                slides.append( static_cast<KoPAPageBase*>((void*)v.value<qulonglong>()));
            }

            if (slides.empty ()) {
                return false;
            }

            //order slides
            QMap<int, KoPAPageBase*> map;
            foreach (KoPAPageBase *slide, slides)
                map.insert(m_customShows->indexByPage(m_currentSlideShowName, slide), slide);

            slides = map.values();

            doCustomShowAction(KPrCustomSlideShowsModel::SLIDES_MOVE, slides, beginRow);
        }
        return true;
    }
    return false;
}

void KPrCustomSlideShowsModel::setCustomSlideShows(KPrCustomSlideShows *customShows)
{
    m_customShows = customShows;
    m_currentSlideShowName = "";
    reset();
}


QString KPrCustomSlideShowsModel::currentSlideShow()
{
    return m_currentSlideShowName;
}

void KPrCustomSlideShowsModel::setCurrentSlideShow(QString name)
{
    if (!m_customShows | (m_currentSlideShowName == name))
        return;
    if (m_customShows->names().contains(name)) {
        m_currentSlideShowName = name;
    }
    reset();
}

void KPrCustomSlideShowsModel::setCurrentSlideShow(int index)
{
    if (!m_customShows)
        return;
    QString name = m_customShows->names().value(index);
    setCurrentSlideShow(name);
}

void KPrCustomSlideShowsModel::setIconSize(QSize size)
{
    if (size != m_iconSize)
        m_iconSize = size;
}

QStringList KPrCustomSlideShowsModel::customShowsNamesList() const
{
    if (m_customShows) {
        return m_customShows->names();
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
    if (!m_customShows)
        return;
    m_customShows->update(name, newCustomShow);
    reset();
}

void KPrCustomSlideShowsModel::removeSlidesFromAll(QList<KoPAPageBase *> pages)
{
    if (!m_customShows)
        return;
    m_customShows->removeSlidesFromAll(pages);
    reset();
}

void KPrCustomSlideShowsModel::removeIndexes(QModelIndexList pageIndexes)
{
    QList<KoPAPageBase *> slides;

    foreach (QModelIndex index, pageIndexes)
        slides.append(m_customShows->pageByIndex(m_currentSlideShowName, index.row()));

    doCustomShowAction(KPrCustomSlideShowsModel::SLIDES_DELETE, slides, 0);
}

bool KPrCustomSlideShowsModel::doCustomShowAction(CustomShowActions c_action, QList<KoPAPageBase *> slides, int beginRow)
{
    bool updated = false;

    //get the slideshow
    QList<KoPAPageBase*> selectedSlideShow = m_customShows->getByName(m_currentSlideShowName);

    if (c_action == KPrCustomSlideShowsModel::SLIDES_ADD) {
        //insert the slides on the current custom show
        int i = beginRow;
        foreach(KoPAPageBase *page, slides)
        {
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
       if (beginRow >= selectedSlideShow.count())
           beginRow = selectedSlideShow.count() - 1;

        foreach(KoPAPageBase *page, slides)
        {
            int from = selectedSlideShow.indexOf(page);
            selectedSlideShow.move(from, beginRow);
        }
        updated = true;
    }
    else if (c_action == KPrCustomSlideShowsModel::SLIDES_DELETE) {
        //delete de slides on the current custom show
        foreach(KoPAPageBase *page, slides)
        {
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
                    m_document, this, m_currentSlideShowName, selectedSlideShow);
        m_document->addCommand(command);
    }

    return updated;
}

void KPrCustomSlideShowsModel::addNewCustomShow(const QString &name)
{
    m_customShows->insert(name, QList<KoPAPageBase*>());
    setCurrentSlideShow(name);
}

void KPrCustomSlideShowsModel::renameCustomShow(const QString &oldName, const QString &newName)
{
    m_customShows->rename(oldName, newName);
    setCurrentSlideShow(newName);
}



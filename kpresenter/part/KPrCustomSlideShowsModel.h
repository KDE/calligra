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

#ifndef KPRCUSTOMSLIDESHOWSMODEL_H
#define KPRCUSTOMSLIDESHOWSMODEL_H

#include <QAbstractListModel>
#include <QSize>

class KPrCustomSlideShows;
class KoPAPageBase;
class KPrDocument;

class KPrCustomSlideShowsModel : public QAbstractListModel
{
    Q_OBJECT
public:

    //List the actions that you could perform with slides
    //within a custom show.
    enum CustomShowActions
    {
        SLIDES_ADD = 0,        ///< add slides
        SLIDES_DELETE    = 1,  ///< delete slides
        SLIDES_MOVE     = 2,  ///< move slides
    };

    explicit KPrCustomSlideShowsModel(KPrDocument *document, QObject *parent = 0);

    virtual ~KPrCustomSlideShowsModel();

    QVariant data(const QModelIndex &index, int role) const;

    int rowCount(const QModelIndex &parent) const;

    virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;

    virtual QStringList mimeTypes() const;

    virtual QMimeData *mimeData(const QModelIndexList &indexes) const;

    virtual Qt::DropActions supportedDropActions() const;

    //virtual bool removeRows(int row, int count, const QModelIndex &parent);

    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    virtual bool dropMimeData (const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);

    void setCustomSlideShows(KPrCustomSlideShows *customShows);

    QString currentSlideShow();

    void setCurrentSlideShow(QString name);

    void setCurrentSlideShow(int index);

    void setIconSize(QSize size);

    QStringList customShowsNamesList() const;

    void setDocument(KPrDocument *document);

    void updateCustomShow(QString name, QList<KoPAPageBase *> newCustomShow);

    /**
     * @brief Deletes all the ocurrencies of a given list of slides from all the known custom SlideShows
     *
     * @param slideShow list of slides to be removed
     */
    void removeSlidesFromAll(QList<KoPAPageBase *> pages);

    void removeIndexes(QModelIndexList pageIndexes);

    bool doCustomShowAction(CustomShowActions c_action, QList<KoPAPageBase *> slides, int beginRow);

    void addNewCustomShow(const QString &name);

    void renameCustomShow(const QString &oldName, const QString &newName);

signals:

public slots:

private:
    KPrCustomSlideShows *m_customShows;
    QString m_currentSlideShowName;
    QSize m_iconSize;
    KPrDocument *m_document;

};

#endif // KPRCUSTOMSLIDESHOWSMODEL_H

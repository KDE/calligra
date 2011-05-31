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

#ifndef KPRSLIDESSORTERDOCUMENTMODEL_H
#define KPRSLIDESSORTERDOCUMENTMODEL_H

#include <QAbstractListModel>

class KPrViewModeSlidesSorter;
class KoPADocument;
class KoPAPageBase;

/**
 * Class meant to hold the model for Slides Sorter
 * This model lets display slides as thumbnails in a List view, using standard QT
 * view/model framework. It supports copy and move of slides, and include a context
 * menu to present options when dropping.
 */
class KPrSlidesSorterDocumentModel: public QAbstractListModel
{
    Q_OBJECT
public:
    KPrSlidesSorterDocumentModel(KPrViewModeSlidesSorter *viewModeSlidesSorter, QWidget *parent, KoPADocument *document = 0);
    ~KPrSlidesSorterDocumentModel();

    /**
     * Set the current document
     * @param document a KoPADocument that holds current document
     */
    void setDocument(KoPADocument *document);

    virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;

    QVariant data(const QModelIndex &index, int role) const;

    int rowCount(const QModelIndex &parent) const;

    virtual QStringList mimeTypes() const;

    virtual QMimeData* mimeData(const QModelIndexList &indexes) const;

    virtual Qt::DropActions supportedDropActions() const;

    virtual bool removeRows(int row, int count, const QModelIndex &parent);

    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    virtual bool dropMimeData (const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);

    /**
     * Drop selected slides (copy/move) if a modifier key is pressed
     * or display a context menu with alternatives.
     * @param slides list of slides to be dropped
     * @param pageAfter destination of the drop
     * @param action the drop action
     */
    void doDrop(QList<KoPAPageBase *> slides, KoPAPageBase * pageAfter, Qt::DropAction action);

     KoPAPageBase* pageFromIndex(const QModelIndex& index) const;

public slots:
    void update();

private:
    //A reference to current document
    KoPADocument *m_document;
    //A reference to Slides sorter class
    KPrViewModeSlidesSorter *m_viewModeSlidesSorter;
};

#endif // KPRSLIDESSORTERDOCUMENTMODEL_H

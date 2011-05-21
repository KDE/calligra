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

class KPrSlidesSorterDocumentModel: public QAbstractListModel
{
    Q_OBJECT
public:
    KPrSlidesSorterDocumentModel(KPrViewModeSlidesSorter *viewModeSlidesSorter, QWidget *parent, KoPADocument *document = 0);
    ~KPrSlidesSorterDocumentModel();

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

public slots:
    void update();

private:
    KoPADocument *m_document;
    KPrViewModeSlidesSorter *m_viewModeSlidesSorter;
};

#endif // KPRSLIDESSORTERDOCUMENTMODEL_H

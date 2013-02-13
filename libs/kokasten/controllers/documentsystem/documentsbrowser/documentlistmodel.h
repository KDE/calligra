/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2009,2012 Friedrich W. H. Kossebau <kossebau@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DOCUMENTLISTMODEL_H
#define DOCUMENTLISTMODEL_H

// Qt
#include <QtCore/QAbstractTableModel>


namespace Kasten2
{

class DocumentsTool;
class AbstractModelSynchronizer;
class AbstractDocument;


class DocumentListModel : public QAbstractTableModel
{
  Q_OBJECT

  public:
    enum ColumnIds
    {
        CurrentColumnId = 0,
        LocalStateColumnId = 1,
        RemoteStateColumnId = 2,
        TitleColumnId = 3,
        NoOfColumnIds = 4 // TODO: what pattern is usually used to mark number of ids?
    };

  public:
    explicit DocumentListModel( DocumentsTool* documentsTool, QObject* parent = 0 );
    virtual ~DocumentListModel();

  public: // QAbstractTableModel API
    virtual int rowCount( const QModelIndex& parent ) const;
    virtual int columnCount( const QModelIndex& parent ) const;
    virtual QVariant data( const QModelIndex& index, int role ) const;
    virtual QVariant headerData( int section, Qt::Orientation orientation, int role ) const;

  private Q_SLOTS:
    void onDocumentsAdded( const QList<Kasten2::AbstractDocument*>& documents );
    void onDocumentsClosing( const QList<Kasten2::AbstractDocument*>& documents );
    void onFocussedDocumentChanged( Kasten2::AbstractDocument* document );
    void onSyncStatesChanged();
    void onSynchronizerChanged( Kasten2::AbstractModelSynchronizer* synchronizer );

  private:
    DocumentsTool* mDocumentsTool;
};

}

#endif

/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2008 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef VERSIONTABLEMODEL_H
#define VERSIONTABLEMODEL_H


// Qt
#include <QtCore/QAbstractTableModel>


namespace Kasten2
{

class DocumentVersionData;
namespace If {
class Versionable;
}
class AbstractModel;


class VersionTableModel : public QAbstractTableModel
{
  Q_OBJECT

  public:
    enum ColumnIds
    {
        CurrentColumnId = 0,
        IdColumnId = 1,
        ChangeDescriptionColumnId = 2,
        NoOfColumnIds = 3 // TODO: what pattern is usually used to mark number of ids?
    };

  public:
    VersionTableModel( AbstractModel* model, If::Versionable* versionControl, QObject* parent = 0 );
    virtual ~VersionTableModel();

  public: // QAbstractTableModel API
    virtual int rowCount( const QModelIndex &parent ) const;
    virtual int columnCount( const QModelIndex &parent ) const;
    virtual QVariant data( const QModelIndex &index, int role ) const;
    virtual QVariant headerData( int section, Qt::Orientation orientation, int role ) const;

  public:
    void setModel( AbstractModel* model, If::Versionable* versionControl );

  private Q_SLOTS:
    void onRevertedToVersionIndex( int versionIndex );
    void onHeadVersionChanged( int newHeadVersionIndex );
    void onHeadVersionDataChanged( const Kasten2::DocumentVersionData& newVersionData );

  private:
    AbstractModel* mModel;
    If::Versionable *mVersionControl;
    /// holds the current version index
    int mVersionIndex;
};

}

#endif

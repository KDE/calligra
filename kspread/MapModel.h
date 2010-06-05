/* This file is part of the KDE project
   Copyright 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_MAP_MODEL
#define KSPREAD_MAP_MODEL

#include <QAbstractListModel>

class QUndoCommand;

namespace KSpread
{
class Map;
class Sheet;

/**
 * A model for the 'embedded document'.
 * Actually, a model for a sheet list.
 * \ingroup Model
 */
class MapModel : public QAbstractListModel
{
    Q_OBJECT
public:
    MapModel(Map* map);
    virtual ~MapModel();

    // QAbstractItemModel interface
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

public Q_SLOTS:
    /**
     * Hides \p sheet, if \p hidden is \c true and \p sheet is visible.
     * Shows \p sheet, if \p hidden is \c false and \p sheet is hidden.
     * \return \c true on success; \c false on failure
     */
    bool setHidden(Sheet* sheet, bool hidden = true);

Q_SIGNALS:
    void addCommandRequested(QUndoCommand* command);

protected:
    Map* map() const;

private Q_SLOTS:
    void addSheet(Sheet* sheet);

private:
    class Private;
    Private * const d;
};

} // namespace KSpread

#endif // KSPREAD_MAP_MODEL

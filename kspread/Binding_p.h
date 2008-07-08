/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
    Copyright (C) 2008 Thomas Zander <zander@kde.org>

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
#ifndef BINDINGMODELMODEL_H
#define BINDINGMODELMODEL_H

#include <QAbstractTableModel>

namespace KSpread
{
class BindingModelModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    BindingModelModel(QObject *parent = 0);

    virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    virtual int rowCount( const QModelIndex& parent = QModelIndex() ) const;
    virtual int columnCount( const QModelIndex& parent = QModelIndex() ) const;
    virtual QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const;

    Sheet *sheet() const;
    void setSheet(Sheet *sheet);

    const Region& region() const;
    void setRegion(const Region& region);

    void emitDataChanged(const QRect& range);
    void emitChanged(const Region& region);

signals:
    void changed(const Region& region);

private:
    Region m_region;
    Sheet *m_sheet;
};

} // namespace KSpread

#endif

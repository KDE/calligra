/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef _STATESMODEL_H_
#define _STATESMODEL_H_

#include <QAbstractListModel>
#include <QImage>

class State;

class StatesModel : public QAbstractListModel
{
public:
    enum Role {
        SortRole = 0x514eb9a0
    };
public:
    StatesModel();
    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    const State* stateAt(int index) const;
    QModelIndex indexFor(const QString& catId, const QString& stateId) const;
private:
    QList<const State*> m_states;
    QVector<QImage> m_icons;
};

#endif

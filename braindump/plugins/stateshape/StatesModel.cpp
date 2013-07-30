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

#include "StatesModel.h"

#include <QPainter>
#include <QSvgRenderer>

#include <kcategorizedsortfilterproxymodel.h>

#include "State.h"
#include "StateCategory.h"
#include "StatesRegistry.h"

StatesModel::StatesModel()
{
    foreach(const QString & catId, StatesRegistry::instance()->categorieIds()) {
        foreach(const QString & stateId, StatesRegistry::instance()->stateIds(catId)) {
            const State* state = StatesRegistry::instance()->state(catId, stateId);
            Q_ASSERT(state);
            m_states.push_back(state);
            QImage image(32, 32, QImage::Format_ARGB32);
            QPainter p(&image);
            state->renderer()->render(&p, QRectF(0, 0, 32, 32));
            m_icons.push_back(image);
        }
    }
    Q_ASSERT(m_states.size() == m_icons.size());
}

int StatesModel::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return m_states.size();
}

QVariant StatesModel::data(const QModelIndex & index, int role) const
{
    if(index.isValid()) {
        switch(role) {
        case Qt::DisplayRole:
            return m_states[index.row()]->name();
        case Qt::DecorationRole:
            return m_icons[index.row()];
        case SortRole:
            return m_states[index.row()]->priority();
        case KCategorizedSortFilterProxyModel::CategorySortRole:
            return QString(QString::number(m_states[index.row()]->category()->priority()) + m_states[index.row()]->category()->id());
        case KCategorizedSortFilterProxyModel::CategoryDisplayRole:
            return m_states[index.row()]->category()->name();
        }
    }
    return QVariant();
}

const State* StatesModel::stateAt(int index) const
{
    Q_ASSERT(index >= 0 && index < m_states.count());
    return m_states[index];
}

QModelIndex StatesModel::indexFor(const QString& catId, const QString& stateId) const
{
    for(int i = 0; i < m_states.count(); ++i) {
        const State* state = m_states[i];
        if(state->category()->id() == catId && state->id() == stateId) {
            return index(i, 0, QModelIndex());
        }
    }
    return QModelIndex();
}

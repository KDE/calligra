/* This file is part of the KDE project
   Copyright (C) 2005-2015 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and,or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KexiTableScrollAreaHeaderModel.h"
#include "KexiTableScrollArea.h"
#include <KexiIcon.h>

#include <KDbTableViewColumn>

#include <QDebug>

class KexiTableScrollAreaHeaderModel::Private
{
public:
    explicit Private(KexiTableScrollAreaHeaderModel *qq)
        : q(qq)
    {
        KexiTableScrollArea *scrollArea = qobject_cast<KexiTableScrollArea*>(qobject_cast<QObject*>(q)->parent());
        plusPixmap = KexiRecordNavigator::plusPixmap(scrollArea->palette());
        penPixmap = KexiRecordNavigator::penPixmap(scrollArea->palette());
        pointerPixmap = KexiRecordNavigator::pointerPixmap(scrollArea->palette());
    }
    KexiTableScrollAreaHeaderModel *q;
    QPixmap penPixmap;
    QPixmap plusPixmap;
    QPixmap pointerPixmap;
};

KexiTableScrollAreaHeaderModel::KexiTableScrollAreaHeaderModel(KexiTableScrollArea* parent)
        : QAbstractTableModel(parent)
        , d(new Private(this))
{
}

KexiTableScrollAreaHeaderModel::~KexiTableScrollAreaHeaderModel()
{
    delete d;
}

int KexiTableScrollAreaHeaderModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    const KexiTableScrollArea *scrollArea = qobject_cast<const KexiTableScrollArea*>(QObject::parent());
    return scrollArea->recordCount()
            + (scrollArea->isInsertingEnabled() ? 1 : 0);
}

int KexiTableScrollAreaHeaderModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return qobject_cast<KexiTableScrollArea*>(QObject::parent())->columnCount();
}

QVariant KexiTableScrollAreaHeaderModel::data(const QModelIndex& index, int role) const
{
    Q_UNUSED(index);
    Q_UNUSED(role);
    // result unused so return nothing
    return QVariant();
}

QVariant KexiTableScrollAreaHeaderModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    //qDebug() << orientation << section << role;
    switch (orientation) {
    case Qt::Horizontal: {
        KDbTableViewData *data = qobject_cast<KexiTableScrollArea*>(QObject::parent())->data();
        KDbTableViewColumn *col = data->visibleColumn(section);
        if (!col) {
            return QVariant();
        }
        switch (role) {
        case Qt::DisplayRole:
            return col->isHeaderTextVisible() ? col->captionAliasOrName() : QString();
        case Qt::DecorationRole: {
            QIcon icon = col->icon();
            if (!icon.isNull()) {
                return icon;
            }
            if (col->field() && col->field()->isPrimaryKey()) {
                return koSmallIcon("key");
            }
            break;
        }
        case Qt::ToolTipRole: {
            KDbField *f = col->field();
            return f ? f->description() : QString();
        }
        }
        break;
    }
    case Qt::Vertical: {
        switch (role) {
        case Qt::DecorationRole: {
            const KexiTableScrollArea *scrollArea = qobject_cast<const KexiTableScrollArea*>(QObject::parent());
            if (scrollArea->isInsertingEnabled()) {
                const int plusRow = scrollArea->recordCount();
                if (section == plusRow) {
                    return d->plusPixmap;
                }
            }
            if (!scrollArea->isReadOnly() &&
                    section == scrollArea->currentRecord() && scrollArea->currentRecord() == scrollArea->recordEditing())
            {
                return d->penPixmap;
            }
            if (section == scrollArea->currentRecord()) {
                return d->pointerPixmap;
            }
        }
        }
        //! @todo add option to display row numbers or other data in headers?
//        if (role == Qt::DisplayRole) {
//            return QString::number(section + 1);
//        }
        break;
    }
    }
    return QVariant();
}

void KexiTableScrollAreaHeaderModel::beginInsertRows(const QModelIndex &parent, int first, int last)
{
    QAbstractTableModel::beginInsertRows(parent, first, last);
}

void KexiTableScrollAreaHeaderModel::endInsertRows()
{
    QAbstractTableModel::endInsertRows();
}

void KexiTableScrollAreaHeaderModel::beginRemoveRows(const QModelIndex &parent, int first, int last)
{
    QAbstractTableModel::beginRemoveRows(parent, first, last);
}

void KexiTableScrollAreaHeaderModel::endRemoveRows()
{
    QAbstractTableModel::endRemoveRows();
}

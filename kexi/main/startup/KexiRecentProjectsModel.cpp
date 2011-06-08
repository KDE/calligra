/* This file is part of the KDE project
   Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
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

#include "KexiRecentProjectsModel.h"
#include <kexi.h>

#include <core/KexiRecentProjects.h>
#include <core/kexiprojectdata.h>

#include <kexidb/utils.h>

KexiRecentProjectsModel::KexiRecentProjectsModel(
    const KexiRecentProjects& projects, QObject *parent)
 : QAbstractListModel(parent), m_projects(&projects)
{
}

int KexiRecentProjectsModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_projects->list().count();
}

QModelIndex KexiRecentProjectsModel::index(int row, int column,
                                           const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    if (row < 0 || row >= m_projects->list().count())
        return QModelIndex();
    return createIndex(row, column, (void*)m_projects->list().at(row));
}

QVariant KexiRecentProjectsModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    KexiProjectData *pdata = static_cast<KexiProjectData*>(index.internalPointer());
    bool fileBased = !pdata->constConnectionData()->dbFileName().isEmpty();
    switch (role) {
    case Qt::DisplayRole: {
        if (fileBased) {
            return pdata->captionOrName() + "\n"
                + pdata->constConnectionData()->fileName();
        }
        else {
            return pdata->captionOrName() + "\n"
                + pdata->connectionData()->serverInfoString();
        }
    }
    case Qt::ToolTipRole:
        return pdata->captionOrName();
    case Qt::DecorationRole: {
        //! @todo show icon specific to given database or mimetype
        if (fileBased) {
            return KIcon(KexiDB::defaultFileBasedDriverIcon());
        }
        else {
            return KIcon(KEXI_ICON_DATABASE_SERVER);
        }
    }
    /*case KCategorizedSortFilterProxyModel::CategorySortRole: {
        int index = m_categoryNameIndex.value(info->category);
        if (index >= 0 && index < m_templateCategories.count()) {
            QVariantList list;
            list << index << info->caption; 
            return list;
        }
        return QVariantList();
    }
    case KCategorizedSortFilterProxyModel::CategoryDisplayRole: {
        int index = m_categoryNameIndex.value(info->category);
        if (index >= 0 && index < m_templateCategories.count()) {
            KexiTemplateCategoryInfo category = m_templateCategories.value(index);
            return category.caption;
        }
        return QVariant();
    }*/
    case NameRole:
        return pdata->databaseName();
    /*case CategoryRole:
        return info->category;*/
    default:
        break;
    }
    return QVariant();
}

Qt::ItemFlags KexiRecentProjectsModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags f;
    if (index.isValid()) {
//        KexiProjectData *pdata = static_cast<KexiProjectData*>(index.internalPointer());
//        if (pdata->enabled)
        f |= (Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    }
    return f;
}

// ----

KexiRecentProjectsProxyModel::KexiRecentProjectsProxyModel(QObject *parent)
 : KCategorizedSortFilterProxyModel(parent)
{
    setCategorizedModel(true);
}

#include "KexiRecentProjectsModel.moc"

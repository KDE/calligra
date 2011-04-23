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

#include "KexiTemplatesModel.h"

KexiTemplatesModel::KexiTemplatesModel(
    const KexiTemplateCategoryInfoList& templateCategories, QObject *parent)
 : QAbstractListModel(parent), m_templateCategories(templateCategories)
{
    int i = 0;
    foreach(const KexiTemplateCategoryInfo& category, templateCategories) {
        m_templates += category.templates();
        m_categoryNameIndex.insert(category.name, i);
        i++;
    }
}

int KexiTemplatesModel::rowCount(const QModelIndex& parent) const
{
    return m_templates.count();
}

QModelIndex KexiTemplatesModel::index(int row, int column,
                                      const QModelIndex& parent ) const
{
    if (row < 0 || row >= m_templates.count())
        return QModelIndex();
    return createIndex(row, column, (void*)&m_templates[row]);
}

QVariant KexiTemplatesModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    KexiTemplateInfo *info = static_cast<KexiTemplateInfo*>(index.internalPointer());
    switch (role) {
    case Qt::DisplayRole:
        return info->caption;
    case Qt::ToolTipRole:
        return info->description;
    case Qt::DecorationRole:
        return info->icon;
    case KCategorizedSortFilterProxyModel::CategorySortRole: {
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
    }
    case NameRole:
        return info->name;
    case CategoryRole:
        return info->category;
    default:
        break;
    }
    return QVariant();
}

Qt::ItemFlags KexiTemplatesModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags f;
    if (index.isValid()) {
        KexiTemplateInfo *info = static_cast<KexiTemplateInfo*>(index.internalPointer());
        if (info->enabled)
            f |= (Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    }
    return f;
}

// ----

KexiTemplatesProxyModel::KexiTemplatesProxyModel(QObject *parent)
 : KCategorizedSortFilterProxyModel(parent)
{
    setCategorizedModel(true);
}

#include "KexiTemplatesModel.moc"

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

class KexiTemplatesModel::Private
{
public:
    Private(const KexiTemplateCategoryInfoList& templateCategories_);
    ~Private();

    KexiTemplateCategoryInfoList templateCategories;
    KexiTemplateInfoList templates;
    QMap<QString, int> categoryNameIndex;
};

KexiTemplatesModel::Private::Private(const KexiTemplateCategoryInfoList& templateCategories_)
    : templateCategories(templateCategories_)
{
    int i = 0;
    foreach(const KexiTemplateCategoryInfo& category, templateCategories)
    {
        templates += category.templates();
        categoryNameIndex.insert(category.name, i);
        i++;
    }
}

KexiTemplatesModel::Private::~Private()
{

}

KexiTemplatesModel::KexiTemplatesModel(
    const KexiTemplateCategoryInfoList& templateCategories, QObject *parent)
    : QAbstractListModel(parent), d(new Private(templateCategories))
{

}

KexiTemplatesModel::~KexiTemplatesModel()
{
    delete d;
}

int KexiTemplatesModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return d->templates.count();
}

QModelIndex KexiTemplatesModel::index(int row, int column,
                                      const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    if (row < 0 || row >= d->templates.count())
        return QModelIndex();
    return createIndex(row, column, (void*)&d->templates[row]);
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
        int index = d->categoryNameIndex.value(info->category);
        if (index >= 0 && index < d->templateCategories.count()) {
            QVariantList list;
            list << index << info->caption; 
            return list;
        }
        return QVariantList();
    }
    case KCategorizedSortFilterProxyModel::CategoryDisplayRole: {
        int index = d->categoryNameIndex.value(info->category);
        if (index >= 0 && index < d->templateCategories.count()) {
            KexiTemplateCategoryInfo category = d->templateCategories.value(index);
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

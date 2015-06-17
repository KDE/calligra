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
#include <KexiIcon.h>

#include <KDbUtils>
#include <KDbDriverManager>
#include <KDbDriverMetaData>

#include <KLocalizedString>

#include <QDebug>

#include <QFileInfo>

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

//! @return "opened x minutes ago" string or similar
static QString openedString(const QDateTime& _opened)
{
    //qDebug() << _opened;
    const KDateTime cur(KDateTime::currentUtcDateTime());
    const KDateTime opened = KDateTime(_opened);
    if (!opened.isValid() || opened >= cur)
        return QString();

    const int days = opened.daysTo(cur);
    if (days <= 1 && opened.secsTo(cur) < 24*60*60) {
        const int minutes = opened.secsTo(cur) / 60;
        const int hours = minutes / 60;
        if (hours < 1) {
            if (minutes == 0)
                return xi18n("Opened less than minute ago");
            else
                return xi18np("Opened 1 minute ago", "Opened %1 minutes ago", minutes);
        } else {
            return xi18np("Opened 1 hour ago", "Opened %1 hours ago", hours);
        }
    } else {
        if (days < 30)
            return xi18np("Opened yesterday", "Opened %1 days ago", days);
        if (days < 365)
            return xi18np("Opened over a month ago", "Opened %1 months ago", days / 30);
        return xi18np("Opened one year ago", "Opened %1 years ago", days / 365);
    }
    return QString();
}

QVariant KexiRecentProjectsModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    KexiProjectData *pdata = static_cast<KexiProjectData*>(index.internalPointer());
    bool fileBased = !pdata->connectionData()->databaseName().isEmpty();
    QString opened(openedString(pdata->lastOpened()));
    if (!opened.isEmpty())
        opened.prepend('\n');
    switch (role) {
    case Qt::DisplayRole: {
        //! @todo add support for imported entries, e.g. MS Access
        if (fileBased) {
            QString n = pdata->caption().trimmed();
            if (n.isEmpty())
                n = pdata->connectionData()->databaseName();
            return QString(n + opened);
        }
        else {
            QString n = pdata->captionOrName();
            if (!n.isEmpty()) {
                n += '\n';
            }
            QString serverInfo = pdata->connectionData()->toUserVisibleString(KDbConnectionData::NoUserVisibleStringOption);
            // friendly message:
            if (serverInfo == "localhost") {
                serverInfo = xi18n("on local server");
            }
            else {
                serverInfo = xi18n("on \"%1\" server", serverInfo);
            }
            return QString(n + serverInfo + opened);
        }
    }
    case Qt::ToolTipRole:
        //! @todo add support for imported entries, e.g. MS Access
        if (fileBased) {
            return xi18nc("File database <file>", "File database %1",
                         pdata->connectionData()->databaseName());
        }
        else {
            KDbDriverManager manager;
            const KDbDriverMetaData *driverMetaData = manager.driverMetaData(pdata->connectionData()->driverId());
            if (!driverMetaData) {
                return xi18n("database");
            }
            return xi18nc("<type> database, e.g. PostgreSQL database, MySQL database", "%1 database",
                          driverMetaData->name());
        }
    case Qt::DecorationRole: {
        //! @todo show icon specific to given database or mimetype
        if (fileBased) {
            return Kexi::defaultFileBasedDriverIcon();
        }
        else {
            return Kexi::serverIcon();
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
        f |= (Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    }
    return f;
}

// ----

KexiRecentProjectsProxyModel::KexiRecentProjectsProxyModel(QObject *parent)
 : KCategorizedSortFilterProxyModel(parent)
{
}

bool KexiRecentProjectsProxyModel::subSortLessThan(
    const QModelIndex& left, const QModelIndex& right) const
{
    KexiProjectData *pdataLeft = static_cast<KexiProjectData*>(left.internalPointer());
    KexiProjectData *pdataRight = static_cast<KexiProjectData*>(right.internalPointer());
    //qDebug() << *pdataLeft << *pdataRight;
    return pdataLeft->lastOpened() < pdataRight->lastOpened();
}


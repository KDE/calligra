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

#include <db/utils.h>

#include <KoIcon.h>

#include <kdatetime.h>
#include <kdebug.h>

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
                return i18n("Opened less than minute ago");
            else
                return i18np("Opened 1 minute ago", "Opened %1 minutes ago", minutes);
        } else {
            return i18np("Opened 1 hour ago", "Opened %1 hours ago", hours);
        }
    } else {
        if (days < 30)
            return i18np("Opened yesterday", "Opened %1 days ago", days);
        if (days < 365)
            return i18np("Opened over a month ago", "Opened %1 months ago", days / 30);
        return i18np("Opened one year ago", "Opened %1 years ago", days / 365);
    }
    return QString();
}

QVariant KexiRecentProjectsModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    KexiProjectData *pdata = static_cast<KexiProjectData*>(index.internalPointer());
    bool fileBased = !pdata->constConnectionData()->dbFileName().isEmpty();
    QString opened(openedString(pdata->lastOpened()));
    if (!opened.isEmpty())
        opened.prepend('\n');
    switch (role) {
    case Qt::DisplayRole: {
        //! @todo add support for imported entries, e.g. MS Access
        if (fileBased) {
            QString n = pdata->caption().trimmed();
            if (n.isEmpty())
                n = pdata->constConnectionData()->dbFileName();
            return n
                   + opened;
        }
        else {
            QString n = pdata->captionOrName();
            if (!n.isEmpty()) {
                n += '\n';
            }
            QString serverInfo = pdata->connectionData()->serverInfoString(false /* without user */);
            // friendly message:
            if (serverInfo == "localhost") {
                serverInfo = i18n("on local server");
            }
            else {
                serverInfo = i18n("on \"%1\" server", serverInfo);
            }
            return n + serverInfo + opened;
        }
    }
    case Qt::ToolTipRole:
        //! @todo add support for imported entries, e.g. MS Access
        if (fileBased) {
            return i18nc("File database <file>", "File database %1",
                         pdata->constConnectionData()->fileName());
        }
        else {
            KexiDB::DriverManager manager;
            return i18nc("<type> database", "%1 database",
                  manager.driverInfo(pdata->constConnectionData()->driverName).caption);
            return pdata->objectName();
        }
    case Qt::DecorationRole: {
        //! @todo show icon specific to given database or mimetype
        if (fileBased) {
            return KIcon(KexiDB::defaultFileBasedDriverIconName());
        }
        else {
            return KIcon(KEXI_DATABASE_SERVER_ICON_NAME);
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
    // disable since by default we are globally sorting by date: setCategorizedModel(true);
}

bool KexiRecentProjectsProxyModel::subSortLessThan(
    const QModelIndex& left, const QModelIndex& right) const
{
    KexiProjectData *pdataLeft = static_cast<KexiProjectData*>(left.internalPointer());
    KexiProjectData *pdataRight = static_cast<KexiProjectData*>(right.internalPointer());
    //kDebug() << *pdataLeft << *pdataRight;
    return pdataLeft->lastOpened() < pdataRight->lastOpened();
}

#include "KexiRecentProjectsModel.moc"

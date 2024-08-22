/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoPAPageThumbnailModel.h"

#include <QIcon>

#include <KLocalizedString>

#include "KoPAPageBase.h"

KoPAPageThumbnailModel::KoPAPageThumbnailModel(const QList<KoPAPageBase *> &pages, QObject *parent)
    : QAbstractListModel(parent)
    , m_pages(pages)
    , m_iconSize(512, 512)
{
}

KoPAPageThumbnailModel::~KoPAPageThumbnailModel() = default;

int KoPAPageThumbnailModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return m_pages.size();

    return 0;
}

QModelIndex KoPAPageThumbnailModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        if (row >= 0 && row < m_pages.size())
            return createIndex(row, column, m_pages.at(row));
    }

    return QModelIndex();
}

QVariant KoPAPageThumbnailModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        QString name = m_pages.at(index.row())->name();
        if (name.isEmpty()) {
            if (m_pages.at(index.row())->pageType() == KoPageApp::Slide) {
                name = i18n("Slide %1", index.row() + 1);
            } else {
                name = i18n("Page %1", index.row() + 1);
            }
        }
        return name;
    } else if (role == Qt::DecorationRole) {
        return QIcon(m_pages.at(index.row())->thumbnail(m_iconSize));
    }

    return QVariant();
}

void KoPAPageThumbnailModel::setIconSize(const QSize &size)
{
    m_iconSize = size;
}

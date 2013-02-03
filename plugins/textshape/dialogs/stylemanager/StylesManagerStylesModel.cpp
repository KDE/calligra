/* This file is part of the KDE project
 * Copyright (C) 2013 Pierre Stirnweiss <pstirnweiss@googlemail.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "StylesManagerStylesModel.h"

#include <KLocale>

#include <QImage>

#include <KDebug>

StylesManagerStylesModel::StylesManagerStylesModel(QObject *parent) :
    StylesFilteredModelBase(parent)
{
}

StylesManagerStylesModel::~StylesManagerStylesModel()
{

}

Qt::ItemFlags StylesManagerStylesModel::flags(const QModelIndex &index) const
{
    if (index.internalId() == NewStyleId || index.internalId() == ExistingStyleId) {
        return (Qt::NoItemFlags);
    }
    return m_sourceModel->flags(m_sourceModel->index(m_proxyToSource.at(index.row()), 0, QModelIndex()));
}

QModelIndex StylesManagerStylesModel::index(int row, int column, const QModelIndex &parent) const
{
    if (row < 0 || column != 0)
        return QModelIndex();

    if (!parent.isValid()) {
        if (row >= m_proxyToSource.count()) {
            return QModelIndex();
        }
        //m_proxyToSource stores a mix of internalIds (negative, specific to this model) and row number in the source model
        return createIndex(row, column, (m_proxyToSource.at(row) >= 0)?int(m_sourceModel->index(m_proxyToSource.at(row), 0, QModelIndex()).internalId()):m_proxyToSource.at(row));
    }
    return QModelIndex();
}

QVariant StylesManagerStylesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role){
    case AbstractStylesModel::isTitleRole: {
        if (index.internalId() == NewStyleId || index.internalId() == ExistingStyleId) {
            return true;
        }
        break;
    }
    case Qt::DisplayRole: {
        if (index.internalId() == NewStyleId) {
            return i18n("New Styles");
        }
        if (index.internalId() == ExistingStyleId) {
            return i18n("Existing Styles");
        }
        break;
    }
    case Qt::DecorationRole: {
        if (index.internalId() >= 0) {
            return m_sourceModel->data(m_sourceModel->index(m_proxyToSource.at(index.row()), 0, QModelIndex()), role);
        }
        else {
            /// TODO: create thumbnail for the new/unsaved style there
        }
        break;
    }
    case Qt::SizeHintRole: {
        return QVariant(QSize(250, 48));
    }
    default: break;
    };
    if (index.internalId() >= 0) {
        return m_sourceModel->data(m_sourceModel->index(m_proxyToSource.at(index.row()), 0, QModelIndex()), role);
    }
    return QVariant();
}

QModelIndex StylesManagerStylesModel::indexForCharacterStyle(const KoCharacterStyle &style) const
{
    if (&style) {
        QModelIndex sourceIndex(m_sourceModel->indexForCharacterStyle(style));

        if (!sourceIndex.isValid() || (m_sourceToProxy.at(sourceIndex.row()) < 0)) {
            return QModelIndex();
        }
        return createIndex(m_sourceToProxy.at(sourceIndex.row()), 0, int(sourceIndex.internalId()));
    }
    return QModelIndex();
}

QImage StylesManagerStylesModel::stylePreview(const QModelIndex &index, QSize size)
{
    if (!index.isValid()) {
        return QImage();
    }
    /// TODO create preview for new/unsaved styles
    return m_sourceModel->stylePreview(m_sourceModel->index(m_proxyToSource.at(index.row()), 0), size);
}

void StylesManagerStylesModel::createMapping()
{
    Q_ASSERT(m_sourceModel);
    if (!m_sourceModel) {
        return;
    }

    m_proxyToSource.clear();
    m_sourceToProxy.clear();

    if (!m_newStylesId.isEmpty()) {
        m_proxyToSource << NewStyleId << m_newStylesId;
    }

    if (m_sourceModel->rowCount()) {
        m_proxyToSource << ExistingStyleId;
        for(int i = 0; i < m_sourceModel->rowCount(QModelIndex()); ++i) {
            m_proxyToSource.append(i);
        }
    }
    m_sourceToProxy.fill(-1, m_sourceModel->rowCount(QModelIndex()));
    for (int i = 0; i < m_proxyToSource.count(); ++i) {
        if (m_proxyToSource.at(i) >= 0) { //we do not need to map to the titles
            m_sourceToProxy[m_proxyToSource.at(i)] = i;
        }
    }
}

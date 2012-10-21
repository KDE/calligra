/* This file is part of the KDE project
 * Copyright (C) 2012 Pierre Stirnweiss <pstirnweiss@googlemail.org>
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

#include "StylesManagerStylesListModel.h"

#include "StylesModel.h"

#include <KoCharacterStyle.h>
#include <KoParagraphStyle.h>
#include <KoStyleManager.h>
#include <KoStyleThumbnailer.h>

#include <KLocale>

#include <QSize>
#include <QSortFilterProxyModel>
#include <QVariant>

#include <KDebug>

StylesManagerStylesListModel::StylesManagerStylesListModel(QObject *parent) :
    StylesFilteredModelBase(parent)
//    m_styleManager(0),
//    m_thumbnailer(0),
//    m_currentSelectedStyle(0)//,
//    QSortFilterProxyModel(parent),
//    KCategorizedSortFilterProxyModel(parent),
//    m_sourceModel(0)
{
}

Qt::ItemFlags StylesManagerStylesListModel::flags(const QModelIndex &index) const
{
    if (index.internalId() == ModifiedStyleId || index.internalId() == OriginalStyleId) {
        return (Qt::NoItemFlags);
    }
    return (Qt::ItemIsEnabled | Qt::ItemIsSelectable);
}

QModelIndex StylesManagerStylesListModel::parent(const QModelIndex &child) const
{
    if (child.data(isTitleRole).toBool()) {
        return QModelIndex();
    }
    else {
        if (child.data(AbstractStylesModel::isModifiedStyle).toBool()) {
            return createIndex(0, 0, ModifiedStyleId);
        }
        else {
            return createIndex(1, 0, OriginalStyleId);
        }
    }
    return QModelIndex();
}

int StylesManagerStylesListModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return 2;
    }
    if (parent.internalId() == ModifiedStyleId) {
        kDebug() << "row count modified: " << m_modifiedStylesCount;
        return m_modifiedStylesCount;
    }
    if (parent.internalId() == OriginalStyleId) {
        kDebug() << "row count original: " << m_originalStylesCount;
        return m_originalStylesCount;
    }
    return 0;
}

QModelIndex StylesManagerStylesListModel::index(int row, int column, const QModelIndex &parent) const
{
    if (row < 0 || column != 0)
        return QModelIndex();

    if (!parent.isValid()) {
        if (row >= 2) {
            return QModelIndex();
        }
        return createIndex(row, 0, (row == 0)?ModifiedStyleId:OriginalStyleId);
    }
    else {
        if (parent.internalId() == ModifiedStyleId) {
            if (row >= m_modifiedStylesCount) {
                return QModelIndex();
            }
            return createIndex(row, 0, int(m_sourceModel->index(row, 0, QModelIndex()).internalId()));
        }
        else {
            if (row >= m_originalStylesCount) {
                return QModelIndex();
            }
            return createIndex(row, 0, int(m_sourceModel->index(m_modifiedStylesCount + row, 0, QModelIndex()).internalId()));
        }
    }
    return QModelIndex();
}

QVariant StylesManagerStylesListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    switch (role) {
    case AbstractStylesModel::isTitleRole: {
        if (index.internalId() == ModifiedStyleId || index.internalId() == OriginalStyleId) {
            return true;
        }
    }
    case Qt::DisplayRole: {
        if (index.internalId() == ModifiedStyleId) {
            return i18n("Modified Styles");
        }
        if (index.internalId() == OriginalStyleId) {
            return i18n("Original Styles");
        }
        return QVariant();
    }
    case Qt::DecorationRole: {
        if (index.internalId() == ModifiedStyleId || index.internalId() == OriginalStyleId) {
            return QVariant();
        }
        if (index.parent().isValid() && index.parent().internalId() == ModifiedStyleId) {
            return m_sourceModel->data(m_sourceModel->index(index.row(), 0, QModelIndex()), role);
        }
        if (index.parent().isValid() && index.parent().internalId() == OriginalStyleId) {
            return m_sourceModel->data(m_sourceModel->index(index.row() + m_modifiedStylesCount, 0, QModelIndex()), role);
        }
        return QVariant();
    }
    case Qt::SizeHintRole: {
        return QVariant(QSize(250, 48));
    }
    default: break;
//        if (index.parent().isValid()) {
//        }
//        kDebug() << "other role. style id: " << index.row();
//        kDebug() << "other role. data: " << m_sourceModel->data(index, role);

//        return KCategorizedSortFilterProxyModel::data(index, role);
//        return QSortFilterProxyModel::data(index, role);
    }

    return QVariant();
}
/*
void StylesManagerStylesListModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    Q_ASSERT(sourceModel);
    if (!sourceModel) {
        return;
    }
/*
    if (m_sourceModel) {
        disconnect(m_sourceModel, SIGNAL(rowsAboutToBeInserted(const QModelIndex &, int, int)), this, SLOT(rowsAboutToBeInserted(QModelIndex,int,int)));
        disconnect(m_sourceModel, SIGNAL(rowsAboutToBeMoved(const QModelIndex &, int, int, const QModelIndex &, int)), this, SLOT(rowsAboutToBeMoved(QModelIndex, int, int, QModelIndex, int)));
        disconnect(m_sourceModel, SIGNAL(rowsAboutToBeRemoved(const QModelIndex &, int, int)), this, SLOT(rowsAboutToBeRemoved(QModelIndex, int, int, QModelIndex, int)));
        disconnect(m_sourceModel, SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(rowsInserted(QModelIndex,int,int)));
        disconnect(m_sourceModel, SIGNAL(rowsMoved(const QModelIndex &, int, int, const QModelIndex &, int)), this, SLOT(rowsMoved(QModelIndex, int, int, QModelIndex, int)));
        disconnect(m_sourceModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(rowsRemoved(QModelIndex, int, int, QModelIndex, int)));
        disconnect(m_sourceModel, SIGNAL(modelAboutToBeReset()), this, SLOT(modelAboutToBeReset()));
        disconnect(m_sourceModel, SIGNAL(modelReset()), this, SLOT(modelReset()));
    }

    m_sourceModel = sourceModel;
    connect(m_sourceModel, SIGNAL(/*QAbstractItemModel::*//*rowsAboutToBeInserted(const QModelIndex &, int, int)), this, SLOT(rowsAboutToBeInserted(QModelIndex,int,int)));
    connect(m_sourceModel, SIGNAL(rowsAboutToBeMoved(const QModelIndex &, int, int, const QModelIndex &, int)), this, SLOT(rowsAboutToBeMoved(QModelIndex, int, int, QModelIndex, int)));
    connect(m_sourceModel, SIGNAL(rowsAboutToBeRemoved(const QModelIndex &, int, int)), this, SLOT(rowsAboutToBeRemoved(QModelIndex, int, int, QModelIndex, int)));
    connect(m_sourceModel, SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(rowsInserted(QModelIndex,int,int)));
    connect(m_sourceModel, SIGNAL(rowsMoved(const QModelIndex &, int, int, const QModelIndex &, int)), this, SLOT(rowsMoved(QModelIndex, int, int, QModelIndex, int)));
    connect(m_sourceModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(rowsRemoved(QModelIndex &, int, int, QModelIndex, int)));
    connect(m_sourceModel, SIGNAL(modelAboutToBeReset()), this, SLOT(modelAboutToBeReset()));
    connect(m_sourceModel, SIGNAL(modelReset()), this, SLOT(modelReset()));

    QSortFilterProxyModel::setSourceModel(sourceModel);
    m_sourceModel = dynamic_cast<StylesModel*>(sourceModel);
    Q_ASSERT(m_sourceModel);
    if (!m_sourceModel) {
        return;
    }
    beginResetModel();
    createMapping();
    endResetModel();
}
*/
/*
KoCharacterStyle* StylesManagerStylesListModel::currentSelectedStyle()
{
    return m_currentSelectedStyle;
}
*/
/*
void StylesManagerStylesListModel::slotModifiedStyle()
{
    if (!m_modifiedStyles.contains(m_currentSelectedStyle->styleId())) {
        beginResetModel();
        if (m_currentSelectedStyle->styleType() == KoCharacterStyle::ParagraphStyle) {
            KoParagraphStyle *paragStyle = dynamic_cast<KoParagraphStyle*>(m_currentSelectedStyle);
            if (paragStyle) {
                m_currentSelectedStyle = paragStyle->clone();
                m_modifiedStyles.insert(m_currentSelectedStyle->styleId(), m_currentSelectedStyle);
            }
        }
        else {
            m_currentSelectedStyle = m_currentSelectedStyle->clone();
            m_modifiedStyles.insert(m_currentSelectedStyle->styleId(), m_currentSelectedStyle);
        }
        createMapping();
        endResetModel();
    }
}
*/
/*
void StylesManagerStylesListModel::slotSetCurrentIndex(QModelIndex index)
{
    if (m_modifiedStyles.contains(index.internalId())) {
        m_currentSelectedStyle = m_modifiedStyles.value(index.internalId());
    }
    else {
        if (!(m_currentSelectedStyle = m_styleManager->paragraphStyle(index.internalId()))) {
            m_currentSelectedStyle = m_styleManager->characterStyle(index.internalId());
        }
    }
}
*/
void StylesManagerStylesListModel::createMapping()
{
    Q_ASSERT(m_sourceModel);
    if (!m_sourceModel) {
        return;
    }

    m_originalStylesCount = 0;
    m_modifiedStylesCount = 0;

    for (int i = 0; i < m_sourceModel->rowCount(QModelIndex()); ++i) {
        QModelIndex index = m_sourceModel->index(i, 0, QModelIndex());
        if (m_sourceModel->data(index, AbstractStylesModel::isModifiedStyle).toBool()) {
            m_modifiedStylesCount++;
        }
        else {
            m_originalStylesCount++;
        }
    }
/*    m_sourceToProxy.clear();
    m_proxyToSource.clear();

    for (int i = 0; i < m_sourceModel->rowCount(QModelIndex()); ++i) {
        QModelIndex index = m_sourceModel->index(i, 0, QModelIndex());
        int id = (int)index.internalId();
        if (!m_modifiedStyles.contains(id)) {
            m_proxyToSource.append(i);
        }
    }
    m_sourceToProxy.fill(-1, m_sourceModel->rowCount((QModelIndex())));
    for (int i = 0; i < m_proxyToSource.count(); ++i) {
        m_sourceToProxy[m_proxyToSource.at(i)] = i;
    }
*/
}

/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Pierre Stirnweiss <pstirnweiss@googlemail.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "StylesFilteredModelBase.h"

#include <KoCharacterStyle.h>
#include <KoParagraphStyle.h>

#include <QDebug>

StylesFilteredModelBase::StylesFilteredModelBase(QObject *parent)
    : AbstractStylesModel(parent)
    , m_sourceModel(nullptr)
{
}

QModelIndex StylesFilteredModelBase::index(int row, int column, const QModelIndex &parent) const
{
    if (row < 0 || column != 0)
        return QModelIndex();

    if (!parent.isValid()) {
        if (row >= m_proxyToSource.count()) {
            return QModelIndex();
        }
        return createIndex(row, column, int(m_sourceModel->index(m_proxyToSource.at(row), 0, QModelIndex()).internalId()));
    }
    return QModelIndex();
}

QModelIndex StylesFilteredModelBase::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);
    return QModelIndex();
}

int StylesFilteredModelBase::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

int StylesFilteredModelBase::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return m_proxyToSource.size();
    }
    return 0;
}

QVariant StylesFilteredModelBase::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role) {
    case Qt::DisplayRole: {
        return QVariant();
    }
    case Qt::DecorationRole: {
        return m_sourceModel->data(m_sourceModel->index(m_proxyToSource.at(index.row()), 0, QModelIndex()), role);
        break;
    }
    case Qt::SizeHintRole: {
        return QVariant(QSize(250, 48));
    }
    default:
        break;
    };
    return QVariant();
}

Qt::ItemFlags StylesFilteredModelBase::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return {};
    return (Qt::ItemIsSelectable | Qt::ItemIsEnabled);
}

void StylesFilteredModelBase::setStyleThumbnailer(KoStyleThumbnailer *thumbnailer)
{
    Q_UNUSED(thumbnailer);
}

QModelIndex StylesFilteredModelBase::indexOf(const KoCharacterStyle *style) const
{
    QModelIndex sourceIndex(m_sourceModel->indexOf(style));

    if (!style || !sourceIndex.isValid() || m_sourceToProxy.at(sourceIndex.row()) < 0) {
        return QModelIndex();
    }
    return createIndex(m_sourceToProxy.at(sourceIndex.row()), 0, style->styleId());
}

QImage StylesFilteredModelBase::stylePreview(int row, const QSize &size)
{
    if (row < 0) {
        return QImage();
    }
    return m_sourceModel->stylePreview(m_proxyToSource.at(row), size);
}
/*
QImage StylesFilteredModelBase::stylePreview(QModelIndex &index, const QSize &size)
{
    if (!index.isValid()) {
        return QImage();
    }
    return m_sourceModel->stylePreview(index, size); //TODO be careful there. this is assuming the sourceModel is only using the internalId, and the index's
internalId matches the model's

}
*/
void StylesFilteredModelBase::setStylesModel(AbstractStylesModel *sourceModel)
{
    if (m_sourceModel == sourceModel) {
        return;
    }
    if (m_sourceModel) {
        disconnect(m_sourceModel, &AbstractStylesModel::rowsAboutToBeInserted, this, &StylesFilteredModelBase::slotRowsAboutToBeInserted);
        disconnect(m_sourceModel, &AbstractStylesModel::rowsAboutToBeMoved, this, &StylesFilteredModelBase::slotRowsAboutToBeMoved);
        disconnect(m_sourceModel, &AbstractStylesModel::rowsAboutToBeRemoved, this, &StylesFilteredModelBase::slotRowsAboutToBeRemoved);
        disconnect(m_sourceModel, &AbstractStylesModel::rowsInserted, this, &StylesFilteredModelBase::slotRowsInserted);
        disconnect(m_sourceModel, &AbstractStylesModel::rowsMoved, this, &StylesFilteredModelBase::slotRowsMoved);
        disconnect(m_sourceModel, &AbstractStylesModel::rowsRemoved, this, &StylesFilteredModelBase::slotRowsRemoved);
        disconnect(m_sourceModel, &AbstractStylesModel::modelAboutToBeReset, this, &StylesFilteredModelBase::slotModelAboutToBeReset);
        disconnect(m_sourceModel, &AbstractStylesModel::modelReset, this, &StylesFilteredModelBase::slotModelReset);
    }

    m_sourceModel = sourceModel;
    connect(m_sourceModel, &AbstractStylesModel::rowsAboutToBeInserted, this, &StylesFilteredModelBase::slotRowsAboutToBeInserted);
    connect(m_sourceModel, &AbstractStylesModel::rowsAboutToBeMoved, this, &StylesFilteredModelBase::slotRowsAboutToBeMoved);
    connect(m_sourceModel, &AbstractStylesModel::rowsAboutToBeRemoved, this, &StylesFilteredModelBase::slotRowsAboutToBeRemoved);
    connect(m_sourceModel, &AbstractStylesModel::rowsInserted, this, &StylesFilteredModelBase::slotRowsInserted);
    connect(m_sourceModel, &AbstractStylesModel::rowsMoved, this, &StylesFilteredModelBase::slotRowsMoved);
    connect(m_sourceModel, &AbstractStylesModel::rowsRemoved, this, &StylesFilteredModelBase::slotRowsRemoved);
    connect(m_sourceModel, &AbstractStylesModel::modelAboutToBeReset, this, &StylesFilteredModelBase::slotModelAboutToBeReset);
    connect(m_sourceModel, &AbstractStylesModel::modelReset, this, &StylesFilteredModelBase::slotModelReset);

    beginResetModel();
    createMapping();
    endResetModel();
}

AbstractStylesModel::Type StylesFilteredModelBase::stylesType() const
{
    Q_ASSERT(m_sourceModel);
    return m_sourceModel->stylesType();
}

void StylesFilteredModelBase::slotModelAboutToBeReset()
{
    beginResetModel();
}

void StylesFilteredModelBase::slotModelReset()
{
    createMapping();
    endResetModel();
}

void StylesFilteredModelBase::slotRowsAboutToBeInserted(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    beginResetModel(); // TODO instead of resetting the whole thing, implement proper logic. this will do for a start, there shouldn't be too many styles anyway
}

void StylesFilteredModelBase::slotRowsInserted(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    createMapping();
    endResetModel();
}

void StylesFilteredModelBase::slotRowsAboutToBeMoved(const QModelIndex &sourceParent,
                                                     int sourceStart,
                                                     int sourceEnd,
                                                     const QModelIndex &destinationParent,
                                                     int destinationRow)
{
    Q_UNUSED(sourceParent);
    Q_UNUSED(sourceStart);
    Q_UNUSED(sourceEnd);
    Q_UNUSED(destinationParent);
    Q_UNUSED(destinationRow);
    beginResetModel(); // TODO instead of resetting the whole thing, implement proper logic. this will do for a start, there shouldn't be too many styles anyway
}

void StylesFilteredModelBase::slotRowsMoved(const QModelIndex &sourceParent,
                                            int sourceStart,
                                            int sourceEnd,
                                            const QModelIndex &destinationParent,
                                            int destinationRow)
{
    Q_UNUSED(sourceParent);
    Q_UNUSED(sourceStart);
    Q_UNUSED(sourceEnd);
    Q_UNUSED(destinationParent);
    Q_UNUSED(destinationRow);
    createMapping();
    endResetModel();
}

void StylesFilteredModelBase::slotRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    beginResetModel(); // TODO instead of resetting the whole thing, implement proper logic. this will do for a start, there shouldn't be too many styles anyway
}

void StylesFilteredModelBase::slotRowsRemoved(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    createMapping();
    endResetModel();
}

void StylesFilteredModelBase::createMapping()
{
    Q_ASSERT(m_sourceModel);
    if (!m_sourceModel) {
        return;
    }
    m_sourceToProxy.clear();
    m_proxyToSource.clear();

    for (int i = 0; i < m_sourceModel->rowCount(QModelIndex()); ++i) {
        m_proxyToSource.append(i);
    }

    m_sourceToProxy.fill(-1, m_sourceModel->rowCount(QModelIndex()));
    for (int i = 0; i < m_proxyToSource.count(); ++i) {
        m_sourceToProxy[m_proxyToSource.at(i)] = i;
    }
}

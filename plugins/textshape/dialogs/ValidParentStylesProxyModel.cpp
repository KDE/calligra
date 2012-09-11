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

#include "ValidParentStylesProxyModel.h"

#include <QModelIndex>

#include <KoParagraphStyle.h>
#include <KoStyleManager.h>

#include <kdebug.h>

ValidParentStylesProxyModel::ValidParentStylesProxyModel(QObject *parent):
    AbstractStylesModel(parent),
    m_styleManager(0),
    m_sourceModel(0)
{
}

void ValidParentStylesProxyModel::setStyleManager(KoStyleManager *sm)
{
    Q_ASSERT(sm);

    m_styleManager = sm;
}

QModelIndex ValidParentStylesProxyModel::index(int row, int column, const QModelIndex &parent) const
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

int ValidParentStylesProxyModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return m_proxyToSource.size();
    }
    return 0;

}

QVariant ValidParentStylesProxyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role){
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
    default: break;
    };
    return QVariant();
}

Qt::ItemFlags ValidParentStylesProxyModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    return (Qt::ItemIsSelectable | Qt::ItemIsEnabled);

}

void ValidParentStylesProxyModel::setStyleThumbnailer(KoStyleThumbnailer *thumbnailer)
{
    Q_UNUSED(thumbnailer);
}

QModelIndex ValidParentStylesProxyModel::indexForParagraphStyle(const KoParagraphStyle &style) const
{
    QModelIndex sourceIndex(m_sourceModel->indexForParagraphStyle(style));

    if (!sourceIndex.isValid() || !m_sourceToProxy.at(sourceIndex.row())) {
        return QModelIndex();
    }
    return createIndex(m_sourceToProxy.at(sourceIndex.row()), 0, style.styleId());
}

QModelIndex ValidParentStylesProxyModel::indexForCharacterStyle(const KoCharacterStyle &style) const
{
    QModelIndex sourceIndex(m_sourceModel->indexForCharacterStyle(style));

    if (!sourceIndex.isValid() || !m_sourceToProxy.at(sourceIndex.row())) {
        return QModelIndex();
    }
    return createIndex(m_sourceToProxy.at(sourceIndex.row()), 0, style.styleId());
}

QImage ValidParentStylesProxyModel::stylePreview(int row, QSize size)
{
    if (row < 0) {
        return QImage();
    }
    return m_sourceModel->stylePreview(m_proxyToSource.at(row), size);

}

void ValidParentStylesProxyModel::setStylesModel(AbstractStylesModel *model)
{
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

    m_sourceModel = model;
    connect(m_sourceModel, SIGNAL(/*QAbstractItemModel::*/rowsAboutToBeInserted(const QModelIndex &, int, int)), this, SLOT(rowsAboutToBeInserted(QModelIndex,int,int)));
    connect(m_sourceModel, SIGNAL(rowsAboutToBeMoved(const QModelIndex &, int, int, const QModelIndex &, int)), this, SLOT(rowsAboutToBeMoved(QModelIndex, int, int, QModelIndex, int)));
    connect(m_sourceModel, SIGNAL(rowsAboutToBeRemoved(const QModelIndex &, int, int)), this, SLOT(rowsAboutToBeRemoved(QModelIndex, int, int, QModelIndex, int)));
    connect(m_sourceModel, SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(rowsInserted(QModelIndex,int,int)));
    connect(m_sourceModel, SIGNAL(rowsMoved(const QModelIndex &, int, int, const QModelIndex &, int)), this, SLOT(rowsMoved(QModelIndex, int, int, QModelIndex, int)));
    connect(m_sourceModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(rowsRemoved(QModelIndex &, int, int, QModelIndex, int)));
    connect(m_sourceModel, SIGNAL(modelAboutToBeReset()), this, SLOT(modelAboutToBeReset()));
    connect(m_sourceModel, SIGNAL(modelReset()), this, SLOT(modelReset()));

    beginResetModel();
    createMapping();
    endResetModel();
}

void ValidParentStylesProxyModel::modelAboutToBeReset()
{
    beginResetModel();
}

void ValidParentStylesProxyModel::modelReset()
{
    createMapping();
    endResetModel();
}

void ValidParentStylesProxyModel::rowsAboutToBeInserted(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    beginResetModel(); //TODO instead of resetting the whole thing, implement proper logic. this will do for a start, there shouldn't be too many styles anyway
}

void ValidParentStylesProxyModel::rowsInserted(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    createMapping();
    endResetModel();
}

void ValidParentStylesProxyModel::rowsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow)
{
    Q_UNUSED(sourceParent);
    Q_UNUSED(sourceStart);
    Q_UNUSED(sourceEnd);
    Q_UNUSED(destinationParent);
    Q_UNUSED(destinationRow);
    beginResetModel(); //TODO instead of resetting the whole thing, implement proper logic. this will do for a start, there shouldn't be too many styles anyway
}

void ValidParentStylesProxyModel::rowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow)
{
    Q_UNUSED(sourceParent);
    Q_UNUSED(sourceStart);
    Q_UNUSED(sourceEnd);
    Q_UNUSED(destinationParent);
    Q_UNUSED(destinationRow);
    createMapping();
    endResetModel();
}

void ValidParentStylesProxyModel::rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    beginResetModel(); //TODO instead of resetting the whole thing, implement proper logic. this will do for a start, there shouldn't be too many styles anyway
}

void ValidParentStylesProxyModel::rowsRemoved(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    createMapping();
    endResetModel();
}

void ValidParentStylesProxyModel::createMapping()
{

    if (!m_styleManager || !m_sourceModel) {
        return;
    }
    m_sourceToProxy.clear();
    m_proxyToSource.clear();

    for(int i = 0; i < m_sourceModel->rowCount(QModelIndex()); ++i) {
        QModelIndex index = m_sourceModel->index(i, 0, QModelIndex());
        int id = (int)index.internalId();
        KoParagraphStyle *paragraphStyle = m_styleManager->paragraphStyle(id);
        if (paragraphStyle) {
            bool ok = true;
            KoParagraphStyle *testStyle = paragraphStyle;
            while (testStyle && ok) {
                ok = testStyle->styleId() != m_currentChildStyleId;
                testStyle = testStyle->parentStyle();
            }
            if (!ok) {
                continue; //we cannot inherit ourself even indirectly through the parent chain
            }
            m_proxyToSource.append(i); //the style is ok for parenting
        }
        else {
            KoCharacterStyle *characterStyle = m_styleManager->characterStyle(id);
            if (characterStyle) {
                bool ok = true;
                KoCharacterStyle *testStyle = characterStyle;
                while (testStyle && ok) {
                    ok = testStyle->styleId() != m_currentChildStyleId;
                    testStyle = testStyle->parentStyle();
                }
                if (!ok) {
                    continue; //we cannot inherit ourself even indirectly through the parent chain
                }
                m_proxyToSource.append(i); //the style is ok for parenting
            }
        }
    }
    m_sourceToProxy.fill(-1, m_sourceModel->rowCount(QModelIndex()));
    for(int i = 0; i < m_proxyToSource.count(); ++i) {
        m_sourceToProxy[m_proxyToSource.at(i)] = i;
    }
}

void ValidParentStylesProxyModel::setCurrentChildStyleId(int styleId)
{
    m_currentChildStyleId = styleId;
    emit layoutAboutToBeChanged();
    createMapping();
    emit layoutChanged();
}

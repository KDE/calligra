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
        if (row >= m_proxyToSource.count())
            return QModelIndex();
        return createIndex(row, column, -1);
    }
    return QModelIndex();
}

int ValidParentStylesProxyModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        kDebug() << "will return row count: " << m_proxyToSource.count();
        return m_proxyToSource.size();
    }
    kDebug() << "will return 0 rows";
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
    kDebug() << "style preview requested for row: " << row;
    if (row < 0) {
        kDebug() << "will return an invalid QImage";
        return QImage();
    }
    kDebug() << "will request the preview from source row: " << m_proxyToSource.at(row);
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

    kDebug() << "sourceModel set: " << model << " with n rows: " << model->rowCount(QModelIndex());
    beginResetModel();
    createMapping();
    endResetModel();
}

void ValidParentStylesProxyModel::modelAboutToBeReset()
{
    kDebug() << "in slot modelAboutToBeReset. model row count: " << m_sourceModel->rowCount(QModelIndex());
    beginResetModel();
}

void ValidParentStylesProxyModel::modelReset()
{
    kDebug() << "in slot modelReset. model row count: " << m_sourceModel->rowCount(QModelIndex());
    createMapping();
    endResetModel();
}

void ValidParentStylesProxyModel::rowsAboutToBeInserted(const QModelIndex &parent, int start, int end)
{
    kDebug() << "in slot rowsAboutToBeInserted. model row count: " << m_sourceModel->rowCount(QModelIndex());
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    beginResetModel(); //TODO instead of resetting the whole thing, implement proper logic. this will do for a start, there shouldn't be too many styles anyway
}

void ValidParentStylesProxyModel::rowsInserted(const QModelIndex &parent, int start, int end)
{
    kDebug() << "in slot rowsInserted. model row count: " << m_sourceModel->rowCount(QModelIndex());
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    createMapping();
    endResetModel();
}

void ValidParentStylesProxyModel::rowsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow)
{
    kDebug() << "in slot rowsAboutToBeMoved. model row count: " << m_sourceModel->rowCount(QModelIndex());
    Q_UNUSED(sourceParent);
    Q_UNUSED(sourceStart);
    Q_UNUSED(sourceEnd);
    Q_UNUSED(destinationParent);
    Q_UNUSED(destinationRow);
    beginResetModel(); //TODO instead of resetting the whole thing, implement proper logic. this will do for a start, there shouldn't be too many styles anyway
}

void ValidParentStylesProxyModel::rowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow)
{
    kDebug() << "in slot rowsMoved. model row count: " << m_sourceModel->rowCount(QModelIndex());
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
    kDebug() << "in slot rowsAboutToBeRemoved. model row count: " << m_sourceModel->rowCount(QModelIndex());
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    beginResetModel(); //TODO instead of resetting the whole thing, implement proper logic. this will do for a start, there shouldn't be too many styles anyway
}

void ValidParentStylesProxyModel::rowsRemoved(const QModelIndex &parent, int start, int end)
{
    kDebug() << "in slot rowsRemoved. model row count: " << m_sourceModel->rowCount(QModelIndex());
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    createMapping();
    endResetModel();
}

void ValidParentStylesProxyModel::createMapping()
{

    kDebug() << "createMapping. m_styleManager: " << m_styleManager;
    kDebug() << "model: " << this;
    if (!m_styleManager || !m_sourceModel) {
        return;
    }
    m_sourceToProxy.clear();
    m_proxyToSource.clear();
    kDebug() << "m_sourcemodel.rowCount: " << m_sourceModel->rowCount(QModelIndex());
    kDebug() << "currentChildStyleId: " << m_currentChildStyleId;
/*
    for(int i = 0; i < m_sourceModel->rowCount(QModelIndex()); ++i)  {
        m_proxyToSource.append(i);
        m_sourceToProxy.append(i);
    }
*/

    for(int i = 0; i < m_sourceModel->rowCount(QModelIndex()); ++i) {
        kDebug() << "looping over source model i: " << i;
        QModelIndex index = m_sourceModel->index(i, 0, QModelIndex());
        int id = (int)index.internalId();
        kDebug() << "handling styleId: " << id;
        KoParagraphStyle *paragraphStyle = m_styleManager->paragraphStyle(id);
        if (paragraphStyle) {
            kDebug() << "style is a paragraph style name: " << paragraphStyle->name();
            bool ok = true;
            KoParagraphStyle *testStyle = paragraphStyle;
            while (testStyle && ok) {
                kDebug() << "testing. testStyle name: " << testStyle->name() << " styleId: " << testStyle->styleId();
                ok = testStyle->styleId() != m_currentChildStyleId;
                testStyle = testStyle->parentStyle();
            }
            kDebug() << "testing finished. ok: " << ok;
            if (!ok) {
                kDebug() << "cannot inherit, continue";
                continue; //we cannot inherit ourself even indirectly through the parent chain
            }
            kDebug() << "we can inherit appending styleId";
            m_proxyToSource.append(i); //the style is ok for parenting
        }
        else {
            KoCharacterStyle *characterStyle = m_styleManager->characterStyle(id);
            if (characterStyle) {
                kDebug() << "we have a char style style name: " << characterStyle->name();
                bool ok = true;
                KoCharacterStyle *testStyle = characterStyle;
                while (testStyle && ok) {
                    ok = testStyle->styleId() != m_currentChildStyleId;
                    testStyle = testStyle->parentStyle();
                }
                kDebug() << "testing finished. ok: " << ok;
                if (!ok) {
                    continue; //we cannot inherit ourself even indirectly through the parent chain
                }
                m_proxyToSource.append(i); //the style is ok for parenting
            }
        }
    }
    kDebug() << "finished filling up the proxyToSource. size: " << m_proxyToSource.count();
    m_sourceToProxy.fill(-1, m_proxyToSource.count());
    kDebug() << "will do sourceToProxy";
    for(int i = 0; i < m_proxyToSource.count(); ++i) {
        m_sourceToProxy[m_proxyToSource.at(i)] = i;
    }

    kDebug() << "m_proxyToSource count: " << m_proxyToSource.count();
    kDebug() << "m_sourceToProxy count: " << m_sourceToProxy.count();
}

void ValidParentStylesProxyModel::setCurrentChildStyleId(int styleId)
{
    kDebug() << "begin model: " << this;
    m_currentChildStyleId = styleId;
    kDebug() << "set member";
//    beginResetModel();
    emit layoutAboutToBeChanged();
    kDebug() << "sent begin reset";
    createMapping();
    kDebug() << "map created proxy item count: " << m_proxyToSource.count();
//    endResetModel();
    emit layoutChanged();
    kDebug() << "end";
}

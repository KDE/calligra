/* This file is part of the KDE project
 * Copyright (C) 2008-2009 Jan Hambrecht <jaham@gmx.net>
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

#include "KarbonLayerSortingModel.h"
#include "KarbonDocument.h"
#include <KoShape.h>
#include <KoShapeLayer.h>
#include <KoShapeContainer.h>

KarbonLayerSortingModel::KarbonLayerSortingModel(QObject * parent)
        : QSortFilterProxyModel(parent)
        , m_document(0)
{
    setDynamicSortFilter(true);
    // in qt-4.5.1 there was a bug (254234) preventing sorting to be enabled
    // so we explicitly trigger the sorting before setting the source model
    sort(0, Qt::DescendingOrder);
}

void KarbonLayerSortingModel::setDocument(KarbonDocument * newDocument)
{
    m_document = newDocument;
    invalidate();
}

bool KarbonLayerSortingModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    KoShape * leftShape = static_cast<KoShape*>(left.internalPointer());
    KoShape * rightShape = static_cast<KoShape*>(right.internalPointer());

    if (! leftShape || ! rightShape)
        return false;

    if (m_document) {
        KoShapeLayer * leftLayer = dynamic_cast<KoShapeLayer*>(leftShape);
        KoShapeLayer * rightLayer = dynamic_cast<KoShapeLayer*>(rightShape);
        if (leftLayer && rightLayer) {
            return m_document->layerPos(leftLayer) < m_document->layerPos(rightLayer);
        } else {
            if (leftShape->zIndex() == rightShape->zIndex()) {
                KoShapeContainer * leftParent = leftShape->parent();
                KoShapeContainer * rightParent = rightShape->parent();
                if (leftParent && leftParent == rightParent) {
                    QList<KoShape*> children = leftParent->shapes();
                    return children.indexOf(leftShape) < children.indexOf(rightShape);
                } else {
                    return leftShape < rightShape;
                }
            } else {
                return leftShape->zIndex() < rightShape->zIndex();
            }
        }
    } else {
        if (leftShape->zIndex() == rightShape->zIndex())
            return leftShape < rightShape;
        else
            return leftShape->zIndex() < rightShape->zIndex();
    }
}

#include "KarbonLayerSortingModel.moc"

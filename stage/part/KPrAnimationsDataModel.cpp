/* This file is part of the KDE project
 * Copyright (C) 2012 Paul Mendez <paulestebanms@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
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

#include "KPrAnimationsDataModel.h"

#include <animations/KPrAnimationStep.h>
#include "animations/KPrAnimationSubStep.h"
#include <KPrPage.h>
#include <KPrView.h>

//KDE HEADERS
#include <KIconLoader>
#include <KLocale>
#include "KoShape.h"
#include "KoShapeContainer.h"
#include "KoShapePainter.h"
#include <KoPADocument.h>
#include <KoShapeManager.h>

//QT HEADERS
#include <QImage>
#include <QPainter>
#include <QDebug>
#include <QAbstractAnimation>

KPrAnimationsDataModel::KPrAnimationsDataModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_activePage(0)
{
    m_data.clear();
}

QModelIndex KPrAnimationsDataModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!m_activePage) {
        return QModelIndex();
    }

    // check if parent is root node
    if(!parent.isValid()) {
        if(row >= 0 && row < rowCount(QModelIndex())) {
            return createIndex(row, column, m_data.at(row).shapeAnimation);
        }
    }
    return QModelIndex();
}

QModelIndex KPrAnimationsDataModel::indexByShape(KoShape *shape)
{
        int row = -1;
        foreach (AnimationsData data, m_data) {
            row++;
            if (shape == data.shapeAnimation->shape())
                return index(row, 1);
        }
        return index(-1,1);
}

int KPrAnimationsDataModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);   
    return m_data.count();
}

int KPrAnimationsDataModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 8;
}

enum ColumnNames {
    Order = 0,
    ShapeName = 1,
    ShapeThumbnail = 2,
    AnimationIcon = 3,
    TriggerEventIcon = 4,
    StartTime = 5,
    EndTime = 6,
    AnimationClass = 7
};

QVariant KPrAnimationsDataModel::data(const QModelIndex &index, int role) const
{
    if (!m_activePage)
        return QVariant();
    if (!index.isValid())
        return QVariant();
    if (role == Qt::TextAlignmentRole) {
        return int(Qt::AlignRight | Qt::AlignVCenter);
    } else if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case Order:
            //TODO: Only return a number when animation starts on click
            return (m_data.at(index.row()).order == 0) ? QVariant() : m_data.at(index.row()).order;
        case ShapeName:
            return m_data.at(index.row()).name;
        case ShapeThumbnail:
            return QVariant();
        case AnimationIcon:
            return QVariant();
        case TriggerEventIcon:
            return QVariant();
        case StartTime:
            return m_data.at(index.row()).startTime;
        case EndTime:
            return m_data.at(index.row()).duration;
        case AnimationClass:
            return m_data.at(index.row()).type;
        default:
            return QVariant();

        }
    } else if (role == Qt::DecorationRole) {
        switch (index.column()) {
        case ShapeThumbnail:
            return m_data.at(index.row()).thumbnail;
        case AnimationIcon:
            return m_data.at(index.row()).animationIcon;
        case TriggerEventIcon:
            if (m_data.at(index.row()).triggerEvent == KPrShapeAnimation::On_Click)
                return KIconLoader::global()->loadIcon(QString("onclick"),
                                                       KIconLoader::NoGroup,
                                                       32);
            if (m_data.at(index.row()).triggerEvent == KPrShapeAnimation::After_Previous)
                return KIconLoader::global()->loadIcon(QString("after_previous"),
                                                       KIconLoader::NoGroup,
                                                       32);
            if (m_data.at(index.row()).triggerEvent == KPrShapeAnimation::With_Previous)
                return KIconLoader::global()->loadIcon(QString("with_previous"),
                                                       KIconLoader::NoGroup,
                                                       32);
        default:
            return QVariant();
        }
    } else if (role == Qt::TextAlignmentRole) {
            return Qt::AlignCenter;
    } else if (role == Qt::ToolTipRole) {
        switch (index.column()) {
        case Order:
        case ShapeName:
        case ShapeThumbnail:
            return QVariant();
        case AnimationIcon:
            return m_data.at(index.row()).animationName;
        case TriggerEventIcon:
            if (m_data.at(index.row()).triggerEvent == KPrShapeAnimation::On_Click)
                return i18n("start on mouse click");
            if (m_data.at(index.row()).triggerEvent == KPrShapeAnimation::After_Previous)
                return i18n("start after previous animation");
            if (m_data.at(index.row()).triggerEvent == KPrShapeAnimation::With_Previous)
                return i18n("start with previous animation");
        case StartTime:
            return i18n("Start after %1 seconds. Duration of %2 seconds").
                    arg(m_data.at(index.row()).startTime).arg(m_data.at(index.row()).duration);
        case EndTime:
        case AnimationClass:
        default:
            return QVariant();

        }
    }
    return QVariant();
}

QVariant KPrAnimationsDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (!m_activePage)
        return QVariant();
    if (role != Qt::DisplayRole)
        return QVariant();
    if (orientation == Qt::Horizontal) {
        switch (section) {
        case ShapeName:
            return i18n("Seconds");
        default:
            return QVariant();
        }

    }
    return QVariant();
}

bool KPrAnimationsDataModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        switch (index.column()) {
        case Order:
            return false;
        case ShapeName:
            return false;
        case ShapeThumbnail:
            return false;
        case AnimationIcon:
            return false;
        case TriggerEventIcon:
            return false;
        case StartTime:
            //TODO: save new value in animation step.
            /*
            m_data[index.row()].startTime = value.toDouble();
            emit dataChanged(index, index);
            return true;
        case EndTime:
            m_data[index.row()].duration = value.toDouble();
            emit dataChanged(index, index);
            return true;
            */
        case AnimationClass:
            return false;
        default:
            return false;

        }
    }
    return false;
}

Qt::ItemFlags KPrAnimationsDataModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    return flags;
}

void KPrAnimationsDataModel::setActivePage(KPrPage *activePage)
{
    Q_ASSERT(activePage);
    m_activePage = activePage;
    m_data.clear();
    m_steps = activePage->animationSteps();
    int k = 0;
    int l = 1;
    foreach (KPrAnimationStep *step, m_steps) {
        for (int i=0; i < step->animationCount(); i++) {
            QAbstractAnimation *animation = step->animationAt(i);
            if (KPrAnimationSubStep *a = dynamic_cast<KPrAnimationSubStep*>(animation)) {
                for (int j=0; j < a->animationCount(); j++) {
                    QAbstractAnimation *shapeAnimation = a->animationAt(j);

                    if (KPrShapeAnimation *b = dynamic_cast<KPrShapeAnimation*>(shapeAnimation)) {
                        k++;
                        if ((b->presetClass() != KPrShapeAnimation::None) && (m_view->shapeManager()->shapes().contains(b->shape()))) {
                            AnimationsData data1;
                            //Load start and end time, convert them to seconds
                            QPair <int, int> timeRange = b->timeRange();
                            qDebug() << "Start at: " << timeRange.first << "end at: " << timeRange.second;
                            data1.startTime = timeRange.first/1000.0;
                            data1.duration = timeRange.second/1000.0;
                            data1.triggerEvent = b->NodeType();

                            if (data1.triggerEvent == KPrShapeAnimation::On_Click) {
                                data1.order = l;
                                l++;
                            }
                            else {
                                data1.order = 0;
                            }

                            qDebug() << "node type: " << b->NodeType();
                            data1.type = b->presetClass();
                            setNameAndAnimationIcon(data1, b->id());

                            //TODO: Draw image file to load when shape thumbnail is not loaded
                            data1.thumbnail = KIconLoader::global()->loadIcon(QString("stage"),
                                                                              KIconLoader::NoGroup,
                                                                              KIconLoader::SizeMedium);
                            data1.shapeAnimation = b;
                            if (!b->shape()->name().isEmpty()) {
                                data1.name = b->shape()->name();
                            }
                            else {
                                data1.name=i18n("Shape %1").arg(k);
                            }

                            QPixmap thumbnail;
                            if (thumbnail.convertFromImage(createThumbnail(b->shape(),
                                                                           QSize(KIconLoader::SizeMedium, KIconLoader::SizeMedium)))) {
                                thumbnail.scaled(QSize(KIconLoader::SizeMedium, KIconLoader::SizeMedium), Qt::KeepAspectRatio);
                            }
                            data1.thumbnail = thumbnail;

                            m_data.append(data1);
                            qDebug() << b->id();
                        }

                    }
                }
            }
        }
    }
    emit layoutAboutToBeChanged();
    emit layoutChanged();
}

QImage KPrAnimationsDataModel::createThumbnail(KoShape* shape, const QSize &thumbSize) const
{
    KoShapePainter painter;

    QList<KoShape*> shapes;

    shapes.append(shape);
    KoShapeContainer * container = dynamic_cast<KoShapeContainer*>(shape);
    if (container)
        shapes.append(container->shapes());

    painter.setShapes(shapes);

    QImage thumb(thumbSize, QImage::Format_RGB32);
    // draw the background of the thumbnail
    thumb.fill(QColor(Qt::white).rgb());

    QRect imageRect = thumb.rect();
    // use 2 pixel border around the content
    imageRect.adjust(2, 2, -2, -2);

    QPainter p(&thumb);
    painter.paint(p, imageRect, painter.contentRect());

    return thumb;
}

void KPrAnimationsDataModel::setNameAndAnimationIcon(KPrAnimationsDataModel::AnimationsData &data, QString id)
{
    //TODO: Identify animations supported by stage
    QStringList descriptionList = id.split("-");
    if (descriptionList.count() > 2) {
        descriptionList.removeFirst();
        descriptionList.removeFirst();
    }
    qDebug() << descriptionList;
    data.animationName = descriptionList.join(QString(" "));
    //TODO: Parse animation preset Class and read icon name
    data.animationIcon = KIconLoader::global()->loadIcon(QString("unrecognized_animation"),
                                                          KIconLoader::NoGroup,
                                                          KIconLoader::SizeMedium);
}

void KPrAnimationsDataModel::setDocumentView(KPrView *view)
{
    m_view = view;
    if (m_view->kopaDocument())
    {
        connect(m_view->kopaDocument(), SIGNAL(shapeRemoved(KoShape*)), this, SLOT(update()));
        connect(m_view->kopaDocument(), SIGNAL(shapeAdded(KoShape*)), this, SLOT(update()));
    }
    reset();
}

void KPrAnimationsDataModel::update()
{
    setActivePage(m_activePage);
    emit layoutAboutToBeChanged();
    emit layoutChanged();
}



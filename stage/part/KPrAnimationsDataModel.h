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

#ifndef KPRANIMATIONSDATAMODEL_H
#define KPRANIMATIONSDATAMODEL_H

#include "animations/KPrShapeAnimation.h"

#include <QAbstractTableModel>
#include <QPixmap>
#include <QIcon>

#include "stage_export.h"

/*Data Model for Animations Time Line View
It requires an activePage to be set*/

class KPrPage;
class QImage;
class KoShape;
class KPrAnimationStep;
class KoPADocument;
class KPrView;

class STAGE_EXPORT KPrAnimationsDataModel : public QAbstractTableModel
{
    Q_OBJECT
public:

    //Struct to resume animation data
    struct AnimationsData {
        QString name;                               //Shape Name
        int order;                                  //Animation Order
        QPixmap thumbnail;                           //Shape Thumbnail
        QPixmap animationIcon;                      //Animation Icon
        QString animationName;                      //Animation name
        KPrShapeAnimation::Preset_Class type;       //Type: Entrance, exit, custom, etc
        KPrShapeAnimation::Node_Type triggerEvent;  //On click, after previous, etc
        qreal startTime;                            //Animation start time
        qreal duration;
        KPrShapeAnimation* shapeAnimation;          //pointer to target element (shape)

    };

    explicit KPrAnimationsDataModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;

    //An active page is required before use the model
    void setActivePage(KPrPage *activePage);

    //requiere to update model if a shape is removed (or added with undo)
    void setDocumentView(KPrView *view);

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

    /// Return the first animation index for the given shape
    QModelIndex indexByShape(KoShape* shape);

public slots:
    /// Triggers an update of the complete model
    void update();

private:
    /// Generates a thumbnail of the animation target (shape)
    QImage createThumbnail(KoShape* shape, const QSize &thumbSize) const;

    /// Parses Animation name and icon from data
    void setNameAndAnimationIcon(AnimationsData &data, QString id);

    QList<AnimationsData> m_data;
    QList<KPrAnimationStep*> m_steps;
    KPrPage *m_activePage;
    KPrView *m_view;
};

#endif // KPRANIMATIONSDATAMODEL_H

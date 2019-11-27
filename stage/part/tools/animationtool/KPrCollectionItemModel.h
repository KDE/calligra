/* This file is part of the KDE project
 * Copyright (C) 2012 Paul Mendez <paulestebanms@gmail.com>
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

// Heavily based in CollectionItemModel work of Peter Simonsson <peter.simonsson@gmail.com>

#ifndef KPRCOLLECTIONITEMMODEL_H
#define KPRCOLLECTIONITEMMODEL_H

#include <QAbstractItemModel>
#include <QVector>
#include <QString>
#include <QIcon>
#include <KoXmlReader.h>

/**
 * Struct containing the information stored in CollectionItemModel item
 */
struct KPrCollectionItem
{
    QString id;                     //animation id
    QString name;                   //animation name (text to be displayed on animations view)
    QString toolTip;                // text of animation tool tip
    QIcon icon;                     // icon of the animation type
    KoXmlElement animationContext;  //xml data used to instantiate animations of this type
};

Q_DECLARE_TYPEINFO(KPrCollectionItem, Q_MOVABLE_TYPE);

/** Model used to store predefined animations data */
class KPrCollectionItemModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit KPrCollectionItemModel(QObject *parent = 0);

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    Qt::DropActions supportedDragActions() const override;

    /**
     * @brief Set the list of KoCollectionItem to be stored in the model
     *
     * @param newlist List of KPrCollectionItem
     */
    void setAnimationClassList(const QVector<KPrCollectionItem> &newlist);

    QVector<KPrCollectionItem> animationClassList() const {return m_animationClassList;}

    /**
     * @brief Return the xml context for the animation on index
     *
     * @param index of the animation
     */
    KoXmlElement animationContext(const QModelIndex &index) const;

private:
    QVector<KPrCollectionItem> m_animationClassList;
    QString m_family;
};
#endif // KPRCOLLECTIONITEMMODEL_H

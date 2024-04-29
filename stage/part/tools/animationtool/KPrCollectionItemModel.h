/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Paul Mendez <paulestebanms@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

// Heavily based in CollectionItemModel work of Peter Simonsson <peter.simonsson@gmail.com>

#ifndef KPRCOLLECTIONITEMMODEL_H
#define KPRCOLLECTIONITEMMODEL_H

#include <KoXmlReader.h>
#include <QAbstractItemModel>
#include <QIcon>
#include <QString>
#include <QVector>

/**
 * Struct containing the information stored in CollectionItemModel item
 */
struct KPrCollectionItem {
    QString id; // animation id
    QString name; // animation name (text to be displayed on animations view)
    QString toolTip; // text of animation tool tip
    QIcon icon; // icon of the animation type
    KoXmlElement animationContext; // xml data used to instantiate animations of this type
};

Q_DECLARE_TYPEINFO(KPrCollectionItem, Q_MOVABLE_TYPE);

/** Model used to store predefined animations data */
class KPrCollectionItemModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit KPrCollectionItemModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    Qt::DropActions supportedDragActions() const override;

    /**
     * @brief Set the list of KoCollectionItem to be stored in the model
     *
     * @param newlist List of KPrCollectionItem
     */
    void setAnimationClassList(const QVector<KPrCollectionItem> &newlist);

    QVector<KPrCollectionItem> animationClassList() const
    {
        return m_animationClassList;
    }

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

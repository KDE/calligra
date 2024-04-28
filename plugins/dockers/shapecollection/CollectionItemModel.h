/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Peter Simonsson <peter.simonsson@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KIVIOSHAPETEMPLATEMODEL_H
#define KIVIOSHAPETEMPLATEMODEL_H

#include <KoShape.h>

#include <QAbstractItemModel>
#include <QIcon>
#include <QList>
#include <QString>

class KoProperties;

/**
 * Struct containing the information stored in CollectionItemModel item
 */
struct KoCollectionItem {
    KoCollectionItem()
    {
        properties = 0;
    };

    QString id;
    QString name;
    QString toolTip;
    QIcon icon;
    const KoProperties *properties;
};

class CollectionItemModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit CollectionItemModel(QObject *parent = 0);

    Qt::DropActions supportedDragActions() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    QStringList mimeTypes() const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    /**
     * Set the list of KoCollectionItem to be stored in the model
     */
    void setShapeTemplateList(const QList<KoCollectionItem> &newlist);
    QList<KoCollectionItem> shapeTemplateList() const
    {
        return m_shapeTemplateList;
    }

    const KoProperties *properties(const QModelIndex &index) const;

private:
    QList<KoCollectionItem> m_shapeTemplateList;
    QString m_family;
};

QDebug operator<<(QDebug dbg, const KoCollectionItem &i);

#endif // KIVIOSHAPETEMPLATEMODEL_H

/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Peter Simonsson <peter.simonsson@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef COLLECTIONITEMMODEL_H
#define COLLECTIONITEMMODEL_H

#include <KoShape.h>

#include <QAbstractItemModel>
#include <QIcon>
#include <QList>
#include <QListView>
#include <QString>

class KoProperties;

/**
 * Struct containing the information stored in CollectionItemModel item
 */
struct KoCollectionItem {
    KoCollectionItem()
    {
        properties = nullptr;
    }

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
    explicit CollectionItemModel(QObject *parent = nullptr);

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
    QList<KoCollectionItem> shapeTemplateList()
    {
        return m_shapeTemplateList;
    }

    void setViewMode(QListView::ViewMode vm);
    QListView::ViewMode viewMode() const;
    const KoProperties *properties(const QModelIndex &index) const;

private:
    QList<KoCollectionItem> m_shapeTemplateList;
    QString m_family;
    QListView::ViewMode m_viewMode;
};

#endif // COLLECTIONITEMMODEL_H

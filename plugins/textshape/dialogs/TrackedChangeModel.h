/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Pierre Stirnweiss <pstirnweiss@googlemail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef TRACKEDCHANGEMODEL_H
#define TRACKEDCHANGEMODEL_H

#include <KoGenChange.h>

#include <QAbstractItemModel>
#include <QHash>
#include <QList>
#include <QMetaType>
#include <QObject>
#include <QPair>

class KoChangeTracker;
class KoTextDocumentLayout;

class QTextDocument;

struct ItemData {
    int changeId;
    QList<QPair<int, int>> changeRanges;
    KoGenChange::Type changeType;
    QString title;
    QString author;
};

Q_DECLARE_METATYPE(ItemData)

class ModelItem
{
public:
    explicit ModelItem(ModelItem *parent = nullptr);
    ~ModelItem();

    void setChangeId(int changeId);
    void setChangeType(KoGenChange::Type type);
    void setChangeTitle(const QString &title);
    void setChangeAuthor(const QString &author);

    void appendChild(ModelItem *child);

    ModelItem *child(int row);
    QList<ModelItem *> children();
    int childCount() const;
    int row() const;
    ModelItem *parent();

    ItemData itemData();

    void setChangeRange(int start, int end);

    void removeChildren();

private:
    QList<ModelItem *> m_childItems;
    ModelItem *m_parentItem;
    ItemData m_data;
};

class TrackedChangeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit TrackedChangeModel(QTextDocument *document, QObject *parent = nullptr);
    ~TrackedChangeModel() override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex indexForChangeId(int changeId);
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    ItemData changeItemData(const QModelIndex &index, int role = Qt::DisplayRole) const;

public Q_SLOTS:
    void setupModel();

private:
    void setupModelData(QTextDocument *document, ModelItem *parent);

    QTextDocument *m_document;
    ModelItem *m_rootItem;
    KoChangeTracker *m_changeTracker;
    KoTextDocumentLayout *m_layout;

    QHash<int, int> m_changeOccurenceCounter;
    QHash<int, ModelItem *> m_changeItems;
};

#endif // TRACKEDCHANGEMODEL_H

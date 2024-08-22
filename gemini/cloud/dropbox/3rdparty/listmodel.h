/*

    SPDX-FileCopyrightText: 2011 Cuong Le <metacuong@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later

*/

#ifndef LISTMODEL_H
#define LISTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QVariant>

class ListItem : public QObject
{
    Q_OBJECT

public:
    enum Roles {
        RevisionRole = Qt::UserRole + 1,
        Thumb_existsRole,
        BytesRole,
        ModifiedRole,
        PathRole,
        Is_dirRole,
        IconRole,
        Mime_typeRole,
        SizeRole,
        CheckedRole,
        NameRole,
        SectionRole
    };
    ListItem(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
    ~ListItem() override = default;
    virtual QString id() const = 0;
    virtual QVariant data(int role) const = 0;

Q_SIGNALS:
    void dataChanged();
};

class ListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count)

public:
    explicit ListModel(ListItem *prototype, QObject *parent = nullptr);
    ~ListModel() override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void appendRow(ListItem *item);
    void appendRows(const QList<ListItem *> &items);
    void insertRow(int row, ListItem *item);
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    ListItem *takeRow(int row);
    ListItem *getRow(int row);
    ListItem *find(const QString &id) const;
    QModelIndex indexFromItem(const ListItem *item) const;
    void clear();

    int count() const;
    int getCount()
    {
        return this->rowCount();
    }
    Q_INVOKABLE QVariantMap get(int row) const;

private Q_SLOTS:
    void handleItemChange();

Q_SIGNALS:
    void countChanged();

protected:
    QHash<int, QByteArray> roleNames() const override;

private:
    ListItem *m_prototype;
    QList<ListItem *> m_list;
};

#endif // LISTMODEL_H

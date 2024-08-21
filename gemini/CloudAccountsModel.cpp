/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 */

#include "CloudAccountsModel.h"
#include "PropertyContainer.h"

#include <QByteArray>
#include <QDataStream>
#include <QDebug>
#include <QFile>
#include <QMetaObject>
#include <QMetaProperty>
#include <QStandardPaths>
#include <QVariant>

struct AccountEntry {
public:
    AccountEntry(QObject *parent)
        : selected(false)
        , accountDetails(new PropertyContainer("", parent))
    {
    }
    QString text;
    bool selected;
    QString accountType;
    QString stackComponent;
    QObject *accountDetails;

    QByteArray serialize()
    {
        // selected is not serialised, because that would be silly
        QByteArray ba;
        QDataStream stream(&ba, QIODevice::WriteOnly);
        stream << text;
        stream << accountType;
        stream << stackComponent;
        Q_FOREACH (const QByteArray &name, accountDetails->dynamicPropertyNames()) {
            stream << name << accountDetails->property(name);
        }
        for (int i = accountDetails->metaObject()->propertyOffset(); i < accountDetails->metaObject()->propertyCount(); ++i) {
            stream << accountDetails->metaObject()->property(i).name() << accountDetails->metaObject()->property(i).read(accountDetails);
        }
        return ba;
    }

    static AccountEntry *fromByteArray(QByteArray &ba, QObject *parent)
    {
        AccountEntry *entry = new AccountEntry(parent);
        QDataStream stream(&ba, QIODevice::ReadOnly);
        stream >> entry->text;
        stream >> entry->accountType;
        stream >> entry->stackComponent;
        entry->accountDetails->setProperty("text", entry->text);
        while (!stream.atEnd()) {
            QByteArray propertyName;
            QVariant data;
            stream >> propertyName;
            stream >> data;
            entry->accountDetails->setProperty(propertyName, data);
        }
        return entry;
    }
};

class CloudAccountsModel::Private
{
public:
    Private(CloudAccountsModel *qq)
        : q(qq)
    {
        dataFile = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + QStringLiteral("/calligrageminicloudaccounts");
        loadList();
    }
    ~Private()
    {
        qDeleteAll(entries);
    }
    CloudAccountsModel *q;
    QList<AccountEntry *> entries;
    QString dataFile;

    void saveList()
    {
        QByteArray ba;
        QDataStream stream(&ba, QIODevice::WriteOnly);
        Q_FOREACH (AccountEntry *entry, entries) {
            stream << entry->serialize();
        }
        QFile data(dataFile);
        data.open(QIODevice::WriteOnly);
        data.write(ba);
        data.close();
    }

    void loadList()
    {
        QByteArray ba;
        QFile data(dataFile);
        data.open(QIODevice::ReadOnly);
        ba = data.readAll();
        data.close();

        q->beginResetModel();
        qDeleteAll(entries);
        entries.clear();

        QDataStream stream(&ba, QIODevice::ReadOnly);
        QByteArray entryBA;
        while (!stream.atEnd()) {
            stream >> entryBA;
            entries.append(AccountEntry::fromByteArray(entryBA, q));
        }

        q->endResetModel();
    }
};

CloudAccountsModel::CloudAccountsModel(QObject *parent)
    : QAbstractListModel(parent)
    , d(new Private(this))
{
}

CloudAccountsModel::~CloudAccountsModel()
{
    delete d;
}

QHash<int, QByteArray> CloudAccountsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TextRole] = "text";
    roles[SelectedRole] = "selected";
    roles[AccountTypeRole] = "accountType";
    roles[StackComponentRole] = "stackComponent";
    roles[AccountDetailsRole] = "accountDetails";
    return roles;
}

QVariant CloudAccountsModel::data(const QModelIndex &index, int role) const
{
    QVariant result;
    if (index.isValid() && index.row() > -1 && index.row() < d->entries.count()) {
        AccountEntry *entry = d->entries.at(index.row());
        switch (role) {
        case TextRole:
            result = entry->text;
            break;
        case SelectedRole:
            result = entry->selected;
            break;
        case AccountTypeRole:
            result = entry->accountType;
            break;
        case StackComponentRole:
            result = entry->stackComponent;
            break;
        case AccountDetailsRole:
            result = QVariant::fromValue<QObject *>(entry->accountDetails);
            break;
        default:
            break;
        }
    }
    return result;
}

int CloudAccountsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return d->entries.count();
}

void CloudAccountsModel::selectIndex(int newSelection)
{
    Q_FOREACH (AccountEntry *entry, d->entries) {
        entry->selected = false;
    }
    if (newSelection >= 0 && newSelection < d->entries.count()) {
        d->entries.at(newSelection)->selected = true;
    }
    dataChanged(index(0), index(d->entries.count() - 1));
}

void CloudAccountsModel::addAccount(QString text, QString accountType, QString stackComponent, QObject *accountDetails, bool removeExisting)
{
    if (removeExisting) {
        removeAccountByName(text);
    }
    AccountEntry *entry = new AccountEntry(this);
    entry->text = text;
    entry->accountType = accountType;
    entry->stackComponent = stackComponent;
    if (accountDetails) {
        Q_FOREACH (const QByteArray &name, accountDetails->dynamicPropertyNames()) {
            entry->accountDetails->setProperty(name, accountDetails->property(name));
        }
        for (int i = accountDetails->metaObject()->propertyOffset(); i < accountDetails->metaObject()->propertyCount(); ++i) {
            entry->accountDetails->setProperty(accountDetails->metaObject()->property(i).name(),
                                               accountDetails->metaObject()->property(i).read(accountDetails));
        }
    }
    int count = d->entries.count();
    beginInsertRows(QModelIndex(), count, count);
    d->entries.append(entry);
    endInsertRows();
    d->saveList();
}

void CloudAccountsModel::renameAccount(int index, QString newText)
{
    if (index > -1 && index < d->entries.count() - 1) {
        d->entries.at(index)->text = newText;
        dataChanged(this->index(index), this->index(index));
        d->saveList();
    }
}

void CloudAccountsModel::removeAccountByName(QString text)
{
    beginResetModel();
    for (int i = d->entries.count() - 1; i > -1; --i) {
        if (d->entries.at(i)->text == text) {
            d->entries.removeAt(i);
        }
    }
    endResetModel();
}

void CloudAccountsModel::removeAccount(int index)
{
    if (index > -1 && index < d->entries.count()) {
        beginRemoveRows(QModelIndex(), index, index);
        delete (d->entries.takeAt(index));
        endRemoveRows();
        d->saveList();
    }
}

QObject *CloudAccountsModel::accountDetails(int index)
{
    if (index > -1 && index < d->entries.count() - 1) {
        return d->entries.at(index)->accountDetails;
    }
    return nullptr;
}

void CloudAccountsModel::setAccountDetails(int index, QObject *newDetails)
{
    if (index > -1 && index < d->entries.count() - 1) {
        AccountEntry *entry = d->entries.at(index);
        if (newDetails) {
            Q_FOREACH (const QByteArray &name, newDetails->dynamicPropertyNames()) {
                entry->accountDetails->setProperty(name, newDetails->property(name));
            }
            for (int i = newDetails->metaObject()->propertyOffset(); i < newDetails->metaObject()->propertyCount(); ++i) {
                entry->accountDetails->setProperty(newDetails->metaObject()->property(i).name(), newDetails->metaObject()->property(i).read(newDetails));
            }
        }
        d->saveList();
    }
}

/* This file is part of the KDE project
 * Copyright 2014  Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "CloudAccountsModel.h"

struct AccountEntry {
public:
    AccountEntry(QObject* parent)
        : selected(false)
        , accountDetails(new QObject(parent))
    {}
    QString text;
    bool selected;
    QString accountType;
    QString stackComponent;
    QObject* accountDetails;
};

class CloudAccountsModel::Private {
public:
    Private()
    {}
    ~Private()
    {
        qDeleteAll(entries);
    }
    QList<AccountEntry*> entries;
};

CloudAccountsModel::CloudAccountsModel(QObject* parent)
    : QAbstractListModel(parent)
    , d(new Private)
{
    QHash<int, QByteArray> roles;
    roles[TextRole] = "text";
    roles[SelectedRole] = "selected";
    roles[AccountTypeRole] = "accountType";
    roles[StackComponentRole] = "stackComponent";
    roles[AccountDetailsRole] = "accountDetails";
    setRoleNames(roles);

    // the dropbox entry is static, because well, it's sort of always there...
    AccountEntry* dropbox = new AccountEntry(this);
    dropbox->text = "Dropbox";
    dropbox->selected = false;
    dropbox->accountType = "DropBox";
    dropbox->stackComponent = "accountsPageDropbox";
    d->entries.append(dropbox);
}

CloudAccountsModel::~CloudAccountsModel()
{
    delete d;
}

QVariant CloudAccountsModel::data(const QModelIndex& index, int role) const
{
    QVariant result;
    if(index.isValid() && index.row() > -1 && index.row() < d->entries.count())
    {
        AccountEntry* entry = d->entries.at(index.row());
        switch(role)
        {
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
                result = QVariant::fromValue<QObject*>(entry->accountDetails);
                break;
            default:
                break;
        }
    }
    return result;
}

int CloudAccountsModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;
    return d->entries.count();
}

void CloudAccountsModel::selectIndex(int newSelection)
{
    Q_FOREACH(AccountEntry* entry, d->entries) {
        entry->selected = false;
    }
    if(newSelection >= 0 && newSelection < d->entries.count()) {
        d->entries.at(newSelection)->selected = true;
    }
    dataChanged(index(0), index(d->entries.count() - 1));
}

#include "CloudAccountsModel.moc"

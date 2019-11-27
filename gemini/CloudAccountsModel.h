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

#ifndef CLOUDACCOUNTSMODEL_H
#define CLOUDACCOUNTSMODEL_H

#include <QAbstractListModel>

class CloudAccountsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum CloudAccountRoles {
        TextRole = Qt::UserRole + 1,
        SelectedRole,
        AccountTypeRole,
        StackComponentRole,
        AccountDetailsRole
    };
    explicit CloudAccountsModel(QObject* parent = 0);
    ~CloudAccountsModel() override;
    QVariant data(const QModelIndex& index, int role) const override;
    int rowCount(const QModelIndex& parent) const override;
    Q_INVOKABLE void selectIndex(int newSelection);

    /**
     * Add an account to the list. If removeExisting is true, the call will first remove any existing
     * accounts with the same text.
     *
     * @param text           The user visible name of the account shown in the UI
     * @param accountType    The type of account, either DropBox, Git or WebDav
     * @param stackComponent The name of the component instantiated to show the files available through
     *                       the account
     * @param accountDetails A pure QObject with a set of dynamic properties containing information used
     *                       by the service. For example, the local repository location for Git
     * @param removeExisting Whether or not to remove any existing accounts with the given text before
     *                       adding this one. Currently used by DropBox to ensure there is only one
     *                       account at a given moment
     */
    Q_INVOKABLE void addAccount(QString text, QString accountType, QString stackComponent, QObject* accountDetails, bool removeExisting = false);
    Q_INVOKABLE void renameAccount(int index, QString newText);
    Q_INVOKABLE void removeAccountByName(QString text);
    Q_INVOKABLE void removeAccount(int index);

    Q_INVOKABLE QObject* accountDetails(int index);
    Q_INVOKABLE void setAccountDetails(int index, QObject* newDetails);
private:
    class Private;
    Private* d;
};

#endif // CLOUDACCOUNTSMODEL_H

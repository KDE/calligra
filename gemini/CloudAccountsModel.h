/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 */

#ifndef CLOUDACCOUNTSMODEL_H
#define CLOUDACCOUNTSMODEL_H

#include <QAbstractListModel>

class CloudAccountsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum CloudAccountRoles { TextRole = Qt::UserRole + 1, SelectedRole, AccountTypeRole, StackComponentRole, AccountDetailsRole };
    explicit CloudAccountsModel(QObject *parent = nullptr);
    ~CloudAccountsModel() override;

    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;
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
    Q_INVOKABLE void addAccount(QString text, QString accountType, QString stackComponent, QObject *accountDetails, bool removeExisting = false);
    Q_INVOKABLE void renameAccount(int index, QString newText);
    Q_INVOKABLE void removeAccountByName(QString text);
    Q_INVOKABLE void removeAccount(int index);

    Q_INVOKABLE QObject *accountDetails(int index);
    Q_INVOKABLE void setAccountDetails(int index, QObject *newDetails);

private:
    class Private;
    Private *d;
};

#endif // CLOUDACCOUNTSMODEL_H

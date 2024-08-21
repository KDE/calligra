/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 */

#ifndef CHECKOUTCREATOR_H
#define CHECKOUTCREATOR_H

#include <QObject>

class CheckoutCreator : public QObject
{
    Q_OBJECT
public:
    explicit CheckoutCreator(QObject *parent = nullptr);
    virtual ~CheckoutCreator();

    Q_INVOKABLE QString getFile(QString caption, QString filter, QString extraSubDir = QString()) const;
    Q_INVOKABLE QString getDir() const;
    Q_INVOKABLE bool isGitDir(QString directory) const;
    /**
     * Create a clone of the repository at url, in the local directory localPath, using
     * the credentials as represented by credentials (same format as used by the cloud
     * accounts system). The user visible name is used to generate a reasonable clone dir
     * name if one isn't easily guessable from the clone url.
     * @param userVisibleName The user visible name of the account
     * @param url             The clone URL (for example git@server.org:someplace/repositoryname.git
     * @param localPath       The directory into which the clone will be made
     * @param credentials     A QObject with properties set for the appropriate credentials type
     * @return The local directory name in which the clone was eventually created, or empty if it failed
     */
    Q_INVOKABLE QString createClone(QString userVisibleName, QString url, QString localPath, QObject *credentials) const;

Q_SIGNALS:
    void cloneProgress(int progress);

private:
    class Private;
    Private *d;
};

#endif // CHECKOUTCREATOR_H

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

#ifndef CHECKOUTCREATOR_H
#define CHECKOUTCREATOR_H

#include <QObject>

class CheckoutCreator : public QObject
{
    Q_OBJECT
public:
    explicit CheckoutCreator(QObject* parent = 0);
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
    Q_INVOKABLE QString createClone(QString userVisibleName, QString url, QString localPath, QObject* credentials) const;

Q_SIGNALS:
    void cloneProgress(int progress);

private:
    class Private;
    Private* d;
};

#endif // CHECKOUTCREATOR_H

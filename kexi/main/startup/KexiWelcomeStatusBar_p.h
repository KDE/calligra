/* This file is part of the KDE project
   Copyright (C) 2012 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KEXIWELCOMESTATUSBAR_P_H
#define KEXIWELCOMESTATUSBAR_P_H

#include <QObject>

class KJob;

//! Web updater for the status bar GUI
class KexiWelcomeStatusBarGuiUpdater : public QObject
{
    Q_OBJECT
public:
    KexiWelcomeStatusBarGuiUpdater();
    ~KexiWelcomeStatusBarGuiUpdater();

public slots:
    void update();

private slots:
    void slotRedirectLoaded();
    void sendRequestListFilesFinished(KJob* job);
    void filesCopyFinished(KJob* job);

private:
    QString uiPath(const QString &fname) const;
    void checkFile(const QByteArray &hash,
                   const QString &remoteFname,
                   QStringList *fileNamesToUpdate);

    class Private;
    Private * const d;
};

#endif

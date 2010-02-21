/* This file is part of the KDE project
   Copyright (C) 2004 Jarosław Staniek <staniek@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXISHAREDACTIONHOST_P_H
#define KEXISHAREDACTIONHOST_P_H

#include <qobject.h>
#include <q3ptrdict.h>
#include <qsignalmapper.h>

//#include <kxmlguiwindow.h>

#include "kexiactionproxy.h"

class KexiSharedActionHost;

class KexiVolatileActionData
{
public:
    KexiVolatileActionData() {
        plugged = false;
    }
//  KAction *kaction;
    bool plugged;
};

//! @internal
class KexiSharedActionHostPrivate : public QObject
{
    Q_OBJECT

public:
    KexiSharedActionHostPrivate(KexiSharedActionHost *h);

    ~KexiSharedActionHostPrivate();

public slots:
    void slotAction(const QString& act_id);

public:
    QHash<QObject*, KexiActionProxy*> actionProxies;
    KexiMainWindowIface *mainWin;
    QList<KAction*> sharedActions;
    QSignalMapper actionMapper;
    QHash<KAction*, KexiVolatileActionData*> volatileActions;
    QHash<QString, QWidget*> enablers;
    KexiSharedActionHost *host;
};

#endif


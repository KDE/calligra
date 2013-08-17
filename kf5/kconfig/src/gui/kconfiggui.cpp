/*
   This file is part of the KDE libraries
   Copyright (c) 1999 Matthias Ettrich <ettrich@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) version 3, or any
   later version accepted by the membership of KDE e.V. (or its
   successor approved by the membership of KDE e.V.), which shall
   act as a proxy defined in Section 6 of version 3 of the license.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "kconfiggui.h"

#include <QGuiApplication>

#include <kconfig.h>

static KConfig* s_sessionConfig = 0;

KConfig* KConfigGui::sessionConfig()
{
    if (!s_sessionConfig) // create an instance specific config object
        s_sessionConfig = new KConfig( sessionConfigName(), KConfig::SimpleConfig );
    return s_sessionConfig;
}

bool KConfigGui::hasSessionConfig()
{
    return s_sessionConfig != 0;
}

QString KConfigGui::sessionConfigName()
{
#ifdef QT_NO_SESSIONMANAGER
#error QT_NO_SESSIONMANAGER was set, this will not compile. Reconfigure Qt with Session management support.
#endif
    const QString sessionKey = qApp->sessionKey();
    const QString sessionId = qApp->sessionId();
    return QString(QLatin1String("session/%1_%2_%3")).arg(QGuiApplication::applicationName()).arg(sessionId).arg(sessionKey);
}


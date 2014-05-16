/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright 2013  Dan Leinir Turthra Jensen <admin@leinir.dk>
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

#include "kritasteamplugin.h"

#include "StorageListModel.h"

#include <QDeclarative.h>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>

#include "kritasteamclient.h"

void KritaSteamPlugin::registerTypes(const char* uri)
{
    Q_UNUSED(uri)
    Q_ASSERT(uri == QLatin1String("org.krita.steam"));
    qmlRegisterType<StorageListModel>("org.krita.steam", 1, 0, "StorageListModel");
}

void KritaSteamPlugin::initializeEngine(QDeclarativeEngine* engine, const char* uri)
{
    Q_UNUSED(uri)
    Q_ASSERT(uri == QLatin1String("org.krita.steam"));
    engine->rootContext()->setContextProperty("SteamClientInstance", KritaSteamClient::instance());
}

#include "kritasteamplugin.moc"

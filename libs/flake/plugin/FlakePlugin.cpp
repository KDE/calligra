// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "FlakePlugin.h"
#include <KoToolManager.h>
#include <QQmlEngine>

void FlakePlugin::registerTypes(const char* uri)
{
    Q_ASSERT(QLatin1String(uri) == QLatin1String("org.kde.callgra.flake"));
    qmlRegisterSingletonInstance<KoToolManager>(uri, 1, 0, "KoToolManager", KoToolManager::instance());
}

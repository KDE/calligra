/*
 * This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Shantanu Tushar <shantanu@kde.org>
 * SPDX-FileCopyrightText: 2013 Sujith Haridasan <sujith.h@gmail.com>
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef CALLIGRAQTQUICKCOMPONENTSPLUGIN_H
#define CALLIGRAQTQUICKCOMPONENTSPLUGIN_H

#include <QQmlEngineExtensionPlugin>

class CalligraQtQuickComponentsPlugin : public QQmlEngineExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.calligra.CalligraQtQuickComponentsPlugin")

public:
    void registerTypes(const char *uri);
    void initializeEngine(QQmlEngine *engine, const char *uri) override;

private:
};

#endif // CALLIGRAQTQUICKCOMPONENTSPLUGIN_H

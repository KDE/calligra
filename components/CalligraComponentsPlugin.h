/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#ifndef CALLIGRACOMPONENTSPLUGIN_H
#define CALLIGRACOMPONENTSPLUGIN_H

#include <QQmlExtensionPlugin>

class QQmlEngine;
class QJSEngine;
class CalligraComponentsPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri) override;
};

#endif // CALLIGRACOMPONENTSPLUGIN_H

class QQmlEngine;

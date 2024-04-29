/*
 * This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef CQPLUGINLOADERPROXY_H
#define CQPLUGINLOADERPROXY_H

#include <QObject>

class CQPluginLoaderProxy : public QObject
{
    Q_OBJECT

public:
    explicit CQPluginLoaderProxy(QObject *parent = nullptr);
    ~CQPluginLoaderProxy();

    Q_INVOKABLE void loadPlugins();
};

#endif // CQPLUGINLOADERPROXY_H

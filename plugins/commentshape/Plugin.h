/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef PLUGIN_H
#define PLUGIN_H

#include <QObject>
#include <QVariantList>

class Plugin : public QObject
{
    Q_OBJECT
public:
    Plugin(QObject *parent, const QVariantList &);
};

#endif // PLUGIN_H

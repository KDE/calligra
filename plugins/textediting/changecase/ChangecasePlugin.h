/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef CHANGECASEPLUGIN_H
#define CHANGECASEPLUGIN_H

#include <QObject>
#include <QVariant>

class ChangecasePlugin : public QObject
{
    Q_OBJECT

public:
    ChangecasePlugin(QObject *parent, const QVariantList &);
    ~ChangecasePlugin() override = default;
};

#endif

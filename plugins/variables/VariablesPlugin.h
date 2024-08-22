/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOVARIABLESPLUGIN_H
#define KOVARIABLESPLUGIN_H

#include <QObject>
#include <QVariantList>

class VariablesPlugin : public QObject
{
    Q_OBJECT

public:
    VariablesPlugin(QObject *parent, const QVariantList &);
    ~VariablesPlugin() override = default;
};

#endif

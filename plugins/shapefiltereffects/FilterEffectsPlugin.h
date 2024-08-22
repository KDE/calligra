/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef FILTEREFFECTSPLUGIN_H
#define FILTEREFFECTSPLUGIN_H

#include <QObject>

class FilterEffectsPlugin : public QObject
{
    Q_OBJECT
public:
    FilterEffectsPlugin(QObject *parent, const QList<QVariant> &);
    ~FilterEffectsPlugin() override = default;
};

#endif // FILTEREFFECTSPLUGIN_H

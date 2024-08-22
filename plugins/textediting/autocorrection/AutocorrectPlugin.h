/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef AUTOCORRECTPLUGIN_H
#define AUTOCORRECTPLUGIN_H

#include <QObject>
#include <QVariant>

class AutocorrectPlugin : public QObject
{
    Q_OBJECT

public:
    AutocorrectPlugin(QObject *parent, const QVariantList &);
    ~AutocorrectPlugin() override = default;
};

#endif

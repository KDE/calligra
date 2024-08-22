/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef TEXTPLUGIN_H
#define TEXTPLUGIN_H

#include <QObject>
#include <QVariantList>

class TextPlugin : public QObject
{
    Q_OBJECT

public:
    TextPlugin(QObject *parent, const QVariantList &);
    ~TextPlugin() override = default;
};
#endif

/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SPELLCHECKPLUGIN_H
#define SPELLCHECKPLUGIN_H

#include <QObject>
#include <QVariant>

class SpellCheckPlugin : public QObject
{
    Q_OBJECT

public:
    SpellCheckPlugin(QObject *parent, const QVariantList &);
};

#endif

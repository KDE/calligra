/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef THESAURUSPLUGIN_H
#define THESAURUSPLUGIN_H

#include <QObject>
#include <QVariant>

class ThesaurusPlugin : public QObject
{
    Q_OBJECT

public:
    ThesaurusPlugin(QObject *parent, const QVariantList &);
    ~ThesaurusPlugin() override = default;
};

#endif

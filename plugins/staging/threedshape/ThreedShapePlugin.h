/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef THREEDSHAPE_PLUGIN_H
#define THREEDSHAPE_PLUGIN_H

// Qt
#include <QObject>
#include <QVariantList>

class ThreedShapePlugin : public QObject
{
    Q_OBJECT
public:
    ThreedShapePlugin(QObject *parent, const QVariantList &);
};

#endif

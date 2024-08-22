/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2009 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef VECTORSHAPE_PLUGIN_H
#define VECTORSHAPE_PLUGIN_H

// Qt
#include <QObject>
#include <QVariantList>

class VectorShapePlugin : public QObject
{
    Q_OBJECT

public:
    VectorShapePlugin(QObject *parent, const QVariantList &);
    ~VectorShapePlugin() override = default;
};

#endif

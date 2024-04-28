/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef TEMPLATESHAPE_PLUGIN_H
#define TEMPLATESHAPE_PLUGIN_H

// Qt
#include <QObject>
#include <QVariantList>

class TemplateShapePlugin : public QObject
{
    Q_OBJECT
public:
    TemplateShapePlugin(QObject *parent, const QVariantList &);
};

#endif

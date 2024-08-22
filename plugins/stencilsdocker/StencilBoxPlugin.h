/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef STENCIL_BOX_PLUGIN_H
#define STENCIL_BOX_PLUGIN_H

#include <QObject>
#include <QVariantList>

class StencilBoxPlugin : public QObject
{
    Q_OBJECT

public:
    StencilBoxPlugin(QObject *parent, const QVariantList &);
    ~StencilBoxPlugin() override = default;
};

#endif

/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KPRFACTORYINIT_H
#define KPRFACTORYINIT_H

#include <KPrFactory.h>

class KPrFactoryInit : public KPrFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KPluginFactory_iid FILE "stagepart.json")
    Q_INTERFACES(KPluginFactory)

public:
    explicit KPrFactoryInit()
        : KPrFactory()
    {
    }
    ~KPrFactoryInit() override = default;
};

#endif

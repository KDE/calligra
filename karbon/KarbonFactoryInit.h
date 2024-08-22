/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2005 David Faure <faure@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef __KARBON_FACTORY_INIT_H__
#define __KARBON_FACTORY_INIT_H__

#include "ui/KarbonFactory.h"

class KarbonFactoryInit : public KarbonFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KPluginFactory_iid FILE "karbonpart.json")
    Q_INTERFACES(KPluginFactory)

public:
    explicit KarbonFactoryInit()
        : KarbonFactory()
    {
    }
    ~KarbonFactoryInit() override = default;
};

#endif

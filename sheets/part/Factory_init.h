/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef FACTORYINIT_H
#define FACTORYINIT_H

#include "Factory.h"

namespace Calligra
{
namespace Sheets
{
class FactoryInit : public Factory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KPluginFactory_iid FILE "sheetspart.json")
    Q_INTERFACES(KPluginFactory)

public:
    explicit FactoryInit()
        : Factory()
    {
    }
    ~FactoryInit() override = default;
};
}
}
#endif

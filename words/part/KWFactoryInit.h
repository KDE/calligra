/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KWFACTORYINIT_H
#define KWFACTORYINIT_H

#include "KWFactory.h"

class KWFactoryInit : public KWFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KPluginFactory_iid FILE "wordspart.json")
    Q_INTERFACES(KPluginFactory)

public:
    explicit KWFactoryInit()
        : KWFactory()
    {
    }
    ~KWFactoryInit() override = default;
};

#endif // KWFACTORYINIT_H

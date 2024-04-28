/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KWRDFDOCKERFACTORY_H
#define KWRDFDOCKERFACTORY_H

#include <KoDockFactoryBase.h>

class KWRdfDockerFactory : public KoDockFactoryBase
{
public:
    KWRdfDockerFactory();

    virtual QString id() const;
    virtual QDockWidget *createDockWidget();
    DockPosition defaultDockPosition() const
    {
        return DockMinimized;
    }
};

#endif

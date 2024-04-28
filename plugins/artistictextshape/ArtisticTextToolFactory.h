/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef ARTISTICTEXTTOOLFACTORY_H
#define ARTISTICTEXTTOOLFACTORY_H

#include <KoToolFactoryBase.h>

class ArtisticTextToolFactory : public KoToolFactoryBase
{
public:
    ArtisticTextToolFactory();
    ~ArtisticTextToolFactory() override;

    KoToolBase *createTool(KoCanvasBase *canvas) override;
};

#endif // ARTISTICTEXTTOOLFACTORY_H

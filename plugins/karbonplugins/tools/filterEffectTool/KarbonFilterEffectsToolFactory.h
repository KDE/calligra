/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef KARBONFILTEREFFECTSTOOLFACTORY_H
#define KARBONFILTEREFFECTSTOOLFACTORY_H

#include <KoToolFactoryBase.h>

class KarbonFilterEffectsToolFactory : public KoToolFactoryBase
{
public:
    KarbonFilterEffectsToolFactory();
    ~KarbonFilterEffectsToolFactory() override;
    KoToolBase *createTool(KoCanvasBase *canvas) override;
};

#endif // KARBONFILTEREFFECTSTOOLFACTORY_H

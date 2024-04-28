/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef _KARBONGRADIENTTOOLFACTORY_H_
#define _KARBONGRADIENTTOOLFACTORY_H_

#include <KoToolFactoryBase.h>

class KarbonGradientToolFactory : public KoToolFactoryBase
{
public:
    KarbonGradientToolFactory();
    ~KarbonGradientToolFactory() override;

    KoToolBase *createTool(KoCanvasBase *canvas) override;
};

#endif // _KARBONGRADIENTTOOLFACTORY_H_

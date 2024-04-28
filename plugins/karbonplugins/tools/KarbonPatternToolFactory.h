/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef _KARBONPATTERNTOOLFACTORY_H_
#define _KARBONPATTERNTOOLFACTORY_H_

#include <KoToolFactoryBase.h>

class KarbonPatternToolFactory : public KoToolFactoryBase
{
public:
    KarbonPatternToolFactory();
    ~KarbonPatternToolFactory() override;

    KoToolBase *createTool(KoCanvasBase *canvas) override;
};

#endif // _KARBONPATTERNTOOLFACTORY_H_

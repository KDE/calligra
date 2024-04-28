/* This file is part of the KDE project
 *
 *  SPDX-FileCopyrightText: 2008 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPRANIMATIONTOOLFACTORY_H
#define KPRANIMATIONTOOLFACTORY_H

#include <KoToolFactoryBase.h>

/// Factory for the KPrAnimationTool
class KPrAnimationToolFactory : public KoToolFactoryBase
{
public:
    /// constructor
    KPrAnimationToolFactory();
    ~KPrAnimationToolFactory() override;

    KoToolBase *createTool(KoCanvasBase *canvas) override;
};
#endif

/* This file is part of the KDE project
 *
 *  SPDX-FileCopyrightText: 2008 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KWPAGETOOLFACTORY_H
#define KWPAGETOOLFACTORY_H

#include <KoToolFactoryBase.h>

/// Factory for the KWPageTool
class KWPageToolFactory : public KoToolFactoryBase
{
public:
    /// constructor
    KWPageToolFactory();
    ~KWPageToolFactory() override;

    KoToolBase *createTool(KoCanvasBase *canvas) override;
};
#endif

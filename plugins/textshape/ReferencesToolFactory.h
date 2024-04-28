/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef REFERENCESTOOLFACTORY_H
#define REFERENCESTOOLFACTORY_H

#include <KoToolFactoryBase.h>

class ReferencesToolFactory : public KoToolFactoryBase
{
public:
    ReferencesToolFactory();
    ~ReferencesToolFactory() override;

    KoToolBase *createTool(KoCanvasBase *canvas) override;
};

#endif

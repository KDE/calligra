/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Carlos Licea <carlos.licea@kdemail.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPABACKGROUNDTOOLFACTORY_H
#define KOPABACKGROUNDTOOLFACTORY_H

#include "tools/backgroundTool/KoPABackgroundTool.h"
#include <KoToolFactoryBase.h>

class KoPABackgroundToolFactory : public KoToolFactoryBase
{
public:
    KoPABackgroundToolFactory();
    /// reimplemented from KoToolFactoryBase
    ~KoPABackgroundToolFactory() override;
    /// reimplemented from KoToolFactoryBase
    KoToolBase *createTool(KoCanvasBase *canvas) override;
};

#endif // KOPABACKGROUNDTOOLFACTORY_H

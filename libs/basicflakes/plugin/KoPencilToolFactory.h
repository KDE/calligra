/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef _KOPENCILTOOLFACTORY_H_
#define _KOPENCILTOOLFACTORY_H_

#include <KoToolFactoryBase.h>

class KoPencilToolFactory : public KoToolFactoryBase
{
public:
    KoPencilToolFactory();
    ~KoPencilToolFactory() override;

    KoToolBase *createTool(KoCanvasBase *canvas) override;
};

#endif // _KOPENCILTOOLFACTORY_H_

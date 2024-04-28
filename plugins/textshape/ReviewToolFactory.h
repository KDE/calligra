/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Pierre Stirnweiss \pierre.stirnweiss_calligra@gadz.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef REVIEWTOOLFACTORY_H
#define REVIEWTOOLFACTORY_H

#include <KoToolFactoryBase.h>

class ReviewToolFactory : public KoToolFactoryBase
{
public:
    ReviewToolFactory();
    ~ReviewToolFactory() override;

    KoToolBase *createTool(KoCanvasBase *canvas) override;
};

#endif

/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2007 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef CHARTTOOLFACTORY_H
#define CHARTTOOLFACTORY_H

#include <KoToolFactoryBase.h>

namespace KoChart
{

class ChartToolFactory : public KoToolFactoryBase
{
public:
    ChartToolFactory();
    ~ChartToolFactory();

    KoToolBase *createTool(KoCanvasBase *canvas) override;
};

} // namespace KoChart

#endif

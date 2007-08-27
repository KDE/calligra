/* This file is part of the KDE project
 * Copyright (C) 2007      Inge Wallin <inge@lysator.liu.se>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "ChartTypeToolFactory.h"
#include "ChartTypeTool.h"
#include "ChartShape.h"

#include <klocale.h>


using namespace KChart;


ChartTypeToolFactory::ChartTypeToolFactory(QObject *parent)
    : KoToolFactory(parent, "ChartTypeToolFactory_ID", i18n("ChartType tool"))
{
    setToolTip (i18n("ChartType editing tool"));
    setToolType (dynamicToolType());
    //setIcon ("");
    setPriority (1);
    setActivationShapeId (ChartShapeId);
}

ChartTypeToolFactory::~ChartTypeToolFactory()
{
}

KoTool * ChartTypeToolFactory::createTool(KoCanvasBase *canvas)
{
    return new ChartTypeTool(canvas);
}


#include "ChartTypeToolFactory.moc"

/* This file is part of the KDE project
 *
 * Copyright 2007 Johannes Simon <johannes.simon@gmail.com>
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

#include "ChartDatasetToolFactory.h"
#include "ChartDatasetTool.h"
#include "ChartShape.h"

#include <KLocale>

using namespace KChart;


ChartDatasetToolFactory::ChartDatasetToolFactory( QObject *parent )
    : KoToolFactory( parent, "ChartLegendDatasetFactory_ID", i18n( "Diagram Dataset Editor" ) )
{
    setToolTip( i18n( "Diagram Dataset Editor" ) );
    setToolType( dynamicToolType() );
    // TODO: We need an icon
    //setIcon ("");
    setPriority( 3 );
    setActivationShapeId( ChartShapeId );
}

ChartDatasetToolFactory::~ChartDatasetToolFactory()
{
}

KoTool * ChartDatasetToolFactory::createTool( KoCanvasBase *canvas )
{
    return new ChartDatasetTool( canvas );
}

#include "ChartDatasetToolFactory.moc"

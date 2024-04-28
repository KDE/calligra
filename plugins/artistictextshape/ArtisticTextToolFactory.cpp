/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ArtisticTextToolFactory.h"
#include "ArtisticTextTool.h"
#include "ArtisticTextShape.h"

#include <KoIcon.h>
#include <KLocalizedString>

ArtisticTextToolFactory::ArtisticTextToolFactory()
    : KoToolFactoryBase("ArtisticTextToolFactoryID")
{
    setToolTip( i18n("Artistic text editing") );
    setToolType( dynamicToolType() );
    setIconName(koIconName("artistictext-tool"));
    setPriority( 1 );
    setActivationShapeId( ArtisticTextShapeID );
}

ArtisticTextToolFactory::~ArtisticTextToolFactory()
{
}

KoToolBase * ArtisticTextToolFactory::createTool( KoCanvasBase * canvas )
{
    return new ArtisticTextTool( canvas );
}


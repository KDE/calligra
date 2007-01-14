/* This file is part of the KDE project
   Copyright 2007 Montel Laurent <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include <klocale.h>

#include "VideoShape.h"
#include "VideoTool.h"

#include "VideoToolFactory.h"


VideoToolFactory::VideoToolFactory( QObject* parent, const QStringList& )
    : KoToolFactory( parent, "VideoToolFactoryId", i18n( "Video Tool" ) )
{
    setToolTip( i18n( "Video editing tool" ) );
    setIcon( "videoshape" );
    setToolType( dynamicToolType() );
    setPriority( 1 );
    setActivationShapeID( VideoShapeId );
}

VideoToolFactory::~VideoToolFactory()
{
}

KoTool* VideoToolFactory::createTool( KoCanvasBase* canvas )
{
    return new VideoTool( canvas );
}

#include "VideoToolFactory.moc"


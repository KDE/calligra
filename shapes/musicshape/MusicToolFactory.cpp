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

#include "MusicShape.h"
#include "MusicTool.h"

#include "MusicToolFactory.h"


MusicToolFactory::MusicToolFactory( QObject* parent, const QStringList& )
    : KoToolFactory( parent, "MusicToolFactoryId", i18n( "Music Tool" ) )
{
    setToolTip( i18n( "Music editing tool" ) );
    setIcon( "musicshape" );
    setToolType( dynamicToolType() );
    setPriority( 1 );
    setActivationShapeId( MusicShapeId );
}

MusicToolFactory::~MusicToolFactory()
{
}

KoTool* MusicToolFactory::createTool( KoCanvasBase* canvas )
{
    return new MusicTool( canvas );
}

#include "MusicToolFactory.moc"


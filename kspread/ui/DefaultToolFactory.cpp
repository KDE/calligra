/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

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

#include "DefaultTool.h"

#include "DefaultToolFactory.h"

using namespace KSpread;

DefaultToolFactory::DefaultToolFactory( QObject* parent )
    : KoToolFactory( parent, KSPREAD_DEFAULT_TOOL_ID, i18n( "KSpread default tool" ) )
{
    setToolTip( i18n( "KSpread default tool" ) );
    setIcon( "kspread" );
    setToolType( mainToolType() );
    setPriority( 1 );
    setActivationShapeId( "flake/always" );
}

DefaultToolFactory::~DefaultToolFactory()
{
}

KoTool* DefaultToolFactory::createTool( KoCanvasBase* canvas )
{
    return new DefaultTool( canvas );
}

#include "DefaultToolFactory.moc"

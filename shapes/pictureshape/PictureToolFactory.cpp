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

#include "PictureShape.h"
#include "PictureTool.h"

#include "PictureToolFactory.h"


PictureToolFactory::PictureToolFactory( QObject* parent, const QStringList& )
    : KoToolFactory( parent, "PictureToolFactoryId", i18n( "Picture Tool" ) )
{
    setToolTip( i18n( "Picture editing tool" ) );
    setIcon( "pictureshape" );
    setToolType( dynamicToolType() );
    setPriority( 1 );
    setActivationShapeId( PictureShapeId );
}

PictureToolFactory::~PictureToolFactory()
{
}

KoTool* PictureToolFactory::createTool( KoCanvasBase* canvas )
{
    return new PictureTool( canvas );
}

#include "PictureToolFactory.moc"



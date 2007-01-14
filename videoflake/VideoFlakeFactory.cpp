/* This file is part of the KDE project
   Copyright 2007 Montel Laurent <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; only
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#include <QStringList>

#include <kgenericfactory.h>
#include <klocale.h>

#include <KoProperties.h>
#include <KoToolRegistry.h>
#include <KoShapeRegistry.h>

#include "VideoFlake.h"
#include "VideoToolFactory.h"

#include "VideoFlakeFactory.h"

K_EXPORT_COMPONENT_FACTORY( videoflake, KGenericFactory<VideoFlakePlugin>( "VideoFlake" ) )

VideoFlakePlugin::VideoFlakePlugin( QObject * parent,  const QStringList & list )
{
    KoShapeRegistry::instance()->add( new VideoFlakeFactory( parent, list ) );
    KoToolRegistry::instance()->add( new VideoToolFactory( parent, list ) );
}


VideoFlakeFactory::VideoFlakeFactory( QObject* parent, const QStringList& list )
    : KoShapeFactory( parent, VideoFlakeId, i18n( "Video Flake" ) )
{
    setToolTip( i18n( "A flake which display a video" ) );
    setIcon( "videoflake" );

}

KoShape* VideoFlakeFactory::createDefaultShape() const
{
    VideoFlake* shape = new VideoFlake();
    shape->setShapeId( shapeId() );
    return shape;
}

KoShape* VideoFlakeFactory::createShape( const KoProperties* params ) const
{
    VideoFlake* shape = new VideoFlake();
    shape->setShapeId( shapeId() );
    return shape;
}

#include "VideoFlakeFactory.moc"

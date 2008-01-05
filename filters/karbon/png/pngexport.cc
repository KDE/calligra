/* This file is part of the KDE project
   Copyright (C) 2002-2004 Rob Buis <buis@kde.org>
   Copyright (C) 2002 Lennart Kudling <kudling@kde.org>
   Copyright (C) 2002 Werner Trobin <trobin@kde.org>
   Copyright (C) 2004 Nicolas Goutte <nicolasg@snafu.de>
   Copyright (C) 2005 Tim Beaulen <tbscope@gmail.com>
   Copyright (C) 2005 Thomas Zander <zander@kde.org>
   Copyright (C) 2005-2006 David Faure <faure@kde.org>
   Copyright (C) 2006 Inge Wallin <inge@lysator.liu.se>
   Copyright (C) 2006 Laurent Montel <montel@kde.org>
   Copyright (C) 2006 Christian Mueller <cmueller@gmx.de>
   Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>

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
 * Boston, MA 02110-1301, USA.
*/

#include "pngexport.h"

#include <KarbonDocument.h>
#include <karbon_part.h>
#include <KoShapePainter.h>

#include <KoFilter.h>
#include <KoFilterChain.h>

#include <kgenericfactory.h>

#include <QImage>

typedef KGenericFactory<PngExport> PngExportFactory;
K_EXPORT_COMPONENT_FACTORY( libkarbonpngexport, PngExportFactory( "kofficefilters" ) )


PngExport::PngExport( QObject*parent, const QStringList& )
    : KoFilter(parent)
{
}

KoFilter::ConversionStatus
PngExport::convert( const QByteArray& from, const QByteArray& to )
{
    if ( to != "image/png" || from != "application/vnd.oasis.opendocument.graphics" )
    {
        return KoFilter::NotImplemented;
    }

    KoDocument * document = m_chain->inputDocument();
    if( ! document )
        return KoFilter::ParsingError;

    KarbonPart * karbonPart = dynamic_cast<KarbonPart*>( document );
    if( ! karbonPart )
        return KoFilter::WrongFormat;

    KoShapePainter painter;
    painter.setShapes( karbonPart->document().shapes() );

    QRectF shapesRect = painter.contentRect();
    QImage image( shapesRect.size().toSize(), QImage::Format_RGB32 );

    // draw the background of the thumbnail
    image.fill( QColor( Qt::white).rgb() );

    if( ! painter.paintShapes( image ) )
        return KoFilter::CreationError;

    image.save( m_chain->outputFile(), "PNG" );

    return KoFilter::OK;
}

#include "pngexport.moc"


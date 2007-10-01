/* This file is part of the KDE project
 * Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
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

#include "KarbonPatternItem.h"
#include <KoPattern.h>
#include <KoResourceChooser.h>

#define THUMB_SIZE 30

KarbonPatternItem::KarbonPatternItem( KoPattern * pattern )
    : m_pattern( pattern)
{
    Q_ASSERT( m_pattern );
    setIcon( QIcon( QPixmap::fromImage( thumbnail( QSize( THUMB_SIZE, THUMB_SIZE ) ) ) ) );
}

QImage KarbonPatternItem::thumbnail( const QSize &thumbSize ) const
{
    QSize imgSize = m_pattern->img().size();

    double scaleW = static_cast<double>( thumbSize.width() ) / static_cast<double>( imgSize.width() );
    double scaleH = static_cast<double>( thumbSize.height() ) / static_cast<double>( imgSize.height() );

    double scale = qMin( scaleW, scaleH );

    int thumbW = static_cast<int>( imgSize.width() * scale );
    int thumbH = static_cast<int>( imgSize.height() * scale );

    return m_pattern->img().scaled( thumbW, thumbH, Qt::IgnoreAspectRatio );
}

QVariant KarbonPatternItem::data ( int role ) const
{
    if( role == KoResourceChooser::LargeThumbnailRole )
    {
        return thumbnail( QSize( 100,100 ) );
    }
    else
        return QTableWidgetItem::data( role );
}

KoPattern * KarbonPatternItem::pattern()
{
    return m_pattern;
}

/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2006 Gary Cramblitt <garycramblitt@comcast.net>

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

// Qt includes.
#include <QRect>
#include <QBrush>
#include <QPainter>

// KDE includes.
#include <klocale.h>

// KOffice includes.
#include <KoGlobal.h>
#include <KoUnit.h>

// KWord includes.
#include "KWPagePreview.h"

/******************************************************************/
/* class KWPagePreview                                            */
/******************************************************************/

/*===================== Constructor ==============================*/
KWPagePreview::KWPagePreview( QWidget* parent, const KoPageLayout& layout )
    : QGroupBox( i18n( "Page Preview" ), parent )
{
    m_columns = 1;
    setPageLayout( layout );
    setMinimumSize( 150, 150 );
}

/*=================== Set layout =================================*/
void KWPagePreview::setPageLayout( const KoPageLayout &layout )
{
    m_layout = layout;
    updateZoomedSize();
    update();
}

void KWPagePreview::updateZoomedSize()
{
    // resolution[XY] is in pixel per pt
    double resolutionX = POINT_TO_INCH( static_cast<double>(KoGlobal::dpiX()) );
    double resolutionY = POINT_TO_INCH( static_cast<double>(KoGlobal::dpiY()) );

    m_pageWidth = m_layout.width * resolutionX;
    m_pageHeight = m_layout.height * resolutionY;

    QRect cr = contentsRect();

    double zh = (cr.height()-30) / m_pageHeight;
    double zw = (cr.width()-30) / m_pageWidth;
    double z = qMin( zw, zh );

    m_pageWidth *= z;
    m_pageHeight *= z;

    m_textFrameX = m_layout.left * resolutionX * z;
    m_textFrameY = m_layout.top * resolutionY * z;
    m_textFrameWidth = m_pageWidth - ( m_layout.left + m_layout.right ) * resolutionX * z;
    m_textFrameHeight = m_pageHeight - ( m_layout.top + m_layout.bottom ) * resolutionY * z;
}

/*=================== Set columns =================================*/
void KWPagePreview::setPageColumns( const KoColumns &columns )
{
    m_columns = columns.columns;
    update();
}

/*======================== Draw contents =========================*/

void KWPagePreview::paintEvent( QPaintEvent * event )
{
    QGroupBox::paintEvent( event );
    QPainter painter( this );

    double cw = m_textFrameWidth;
    if(m_columns!=1)
        cw/=static_cast<double>(m_columns);

    painter.setBrush( Qt::white );
    painter.setPen( QPen( Qt::black ) );

    QRect cr = contentsRect();

    int x=static_cast<int>( cr.left() + ( cr.width() - m_pageWidth ) * 0.5 );
    int y=static_cast<int>( cr.top() + ( cr.height() - m_pageHeight ) * 0.5 );
    int w=static_cast<int>(m_pageWidth);
    int h=static_cast<int>(m_pageHeight);
    //painter->drawRect( x + 1, y + 1, w, h);
    painter.drawRect( x, y, w, h );

    painter.setBrush( QBrush( Qt::black, Qt::HorPattern ) );
    if ( m_textFrameWidth == m_pageWidth || m_textFrameHeight == m_pageHeight )
        painter.setPen( Qt::NoPen );
    else
        painter.setPen( Qt::lightGray );

    for ( int i = 0; i < m_columns; ++i )
        painter.drawRect( x + static_cast<int>(m_textFrameX) + static_cast<int>(i * cw),
                          y + static_cast<int>(m_textFrameY), static_cast<int>(cw),
                          static_cast<int>(m_textFrameHeight) );
}

void KWPagePreview::resizeEvent ( QResizeEvent * event )
{
    Q_UNUSED(event);
    updateZoomedSize();
}

#include <KWPagePreview.moc>

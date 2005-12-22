/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "kivio_fill_style.h"
#include "kivio_line_style.h"
#include "kivio_painter.h"
#include "kivio_point.h"

KivioPainter::KivioPainter()
    : m_pFillStyle(NULL)
{
    m_pFillStyle = new KivioFillStyle();
    m_pLineStyle = new KivioLineStyle();
}

KivioPainter::~KivioPainter()
{
    if( m_pFillStyle )
    {
        delete m_pFillStyle;
        m_pFillStyle = NULL;
    }

    if( m_pLineStyle )
    {
        delete m_pLineStyle;
        m_pLineStyle = NULL;
    }
}


void KivioPainter::setFGColor( const QColor &c )
{
    m_pLineStyle->setColor(c);
}

QColor KivioPainter::fgColor() const
{
    return m_pLineStyle->color();
}

float KivioPainter::lineWidth() const
{
    return m_pLineStyle->width();
}

void KivioPainter::setLineWidth( const float &f )
{
    m_pLineStyle->setWidth(f);
}

void KivioPainter::setLineStyle( KivioLineStyle *pStyle )
{
    pStyle->copyInto( m_pLineStyle );
}

void KivioPainter::setFillStyle( KivioFillStyle * s )
{
  s->copyInto( m_pFillStyle );
}

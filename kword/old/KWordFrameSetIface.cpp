/* This file is part of the KDE project
   Copyright (C) 2002 Laurent MONTEL <lmontel@mandrakesoft.com>

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

#include "KWordFrameSetIface.h"
#include "KWFrameSet.h"
#include "KWDocument.h"
#include <dcopclient.h>


KWordFrameSetIface::KWordFrameSetIface( KWFrameSet *_frame )
    : DCOPObject( _frame->name().toUtf8() )
{
   m_frame = _frame;
}

bool KWordFrameSetIface::isAHeader() const
{
    return m_frame->isAHeader();
}

bool KWordFrameSetIface::isAFooter() const
{
    return m_frame->isAFooter();
}

bool KWordFrameSetIface::isHeaderOrFooter() const
{
    return m_frame->isHeaderOrFooter();
}

bool KWordFrameSetIface::isMainFrameset() const
{
    return m_frame->isMainFrameset();
}

bool KWordFrameSetIface::isMoveable() const
{
    return m_frame->isMoveable();
}

bool KWordFrameSetIface::isVisible() const
{
    return m_frame->isVisible();
}

bool KWordFrameSetIface::isFloating() const
{
    return m_frame->isFloating();
}

double KWordFrameSetIface::ptWidth() const
{
    return m_frame->frame(0)->size().width();
}

double KWordFrameSetIface::ptHeight() const
{
    return m_frame->frame(0)->size().height();
}

double KWordFrameSetIface::ptPosX() const
{
    return m_frame->frame(0)->position().x();
}

double KWordFrameSetIface::ptPosY() const
{
    return m_frame->frame(0)->position().y();
}

int KWordFrameSetIface::zOrder() const
{
    return m_frame->frame(0)->zOrder();
}

int KWordFrameSetIface::pageNumber() const
{
    return m_frame->frame(0)->pageNumber();
}

QBrush KWordFrameSetIface::backgroundColor() const
{
    return m_frame->frame(0)->background();
}

void KWordFrameSetIface::setBackgroundColor( const QString &_color )
{
    QBrush brush= m_frame->frame(0)->background();
    brush.setColor( QColor( _color ));
    m_frame->frame(0)->setBackground( brush );
}

double KWordFrameSetIface::ptMarginLeft()const
{
    return m_frame->frame(0)->paddingLeft();
}

double KWordFrameSetIface::ptMarginRight()const
{
    return m_frame->frame(0)->paddingRight();
}

double KWordFrameSetIface::ptMarginTop()const
{
    return m_frame->frame(0)->paddingTop();
}

double KWordFrameSetIface::ptMarginBottom()const
{
    return m_frame->frame(0)->paddingBottom();
}

bool KWordFrameSetIface::isCopy()const
{
    return m_frame->frame(0)->isCopy();
}

void KWordFrameSetIface::setPtMarginLeft(double val)
{
    m_frame->frame( 0 )->setPaddingLeft(val);
    m_frame->kWordDocument()->layout();
}

void KWordFrameSetIface::setPtMarginRight(double val)
{
    m_frame->frame( 0 )->setPaddingRight(val);
    m_frame->kWordDocument()->layout();

}
void KWordFrameSetIface::setPtMarginTop(double val)
{
    m_frame->frame( 0 )->setPaddingTop(val);
    m_frame->kWordDocument()->layout();
}

void KWordFrameSetIface::setPtMarginBottom(double val)
{
    m_frame->frame( 0 )->setPaddingBottom(val);
    m_frame->kWordDocument()->layout();
}

bool KWordFrameSetIface::isProtectSize()const
{
    return m_frame->isProtectSize();
}

void KWordFrameSetIface::setProtectSize( bool _b )
{
    m_frame->setProtectSize( _b );
}


bool KWordFrameSetIface::isFootEndNote() const
{
    return m_frame->isFootEndNote();
}

QString KWordFrameSetIface::frameSetInfo() const
{
    switch( m_frame->frameSetInfo() )
    {
    case KWFrameSet::FI_BODY:
        return QString("body");
        break;
    case KWFrameSet::FI_FIRST_HEADER:
        return QString("First header");
        break;
    case KWFrameSet::FI_ODD_HEADER:
        return QString("First even header");
        break;
    case KWFrameSet::FI_EVEN_HEADER:
        return QString("First odd header");
        break;
    case KWFrameSet::FI_FIRST_FOOTER:
        return QString("First footer");
        break;
    case KWFrameSet::FI_EVEN_FOOTER:
        return QString("Odd footer");
        break;
    case KWFrameSet::FI_ODD_FOOTER:
        return QString("Even footer");
        break;
    case KWFrameSet::FI_FOOTNOTE:
        return QString("FootNote");
        break;
    default:
        return QString();
        break;
    }
}

void KWordFrameSetIface::setFrameSetInfo( const QString & _type)
{
    if ( _type.lower() =="body")
    {
        m_frame->setFrameSetInfo( KWFrameSet::FI_BODY );
    }
    else if ( _type.lower() =="first header")
    {
        m_frame->setFrameSetInfo( KWFrameSet::FI_FIRST_HEADER );
    }
    else if ( _type.lower() =="first even header")
    {
        m_frame->setFrameSetInfo( KWFrameSet::FI_ODD_HEADER );
    }
    else if ( _type.lower() =="first odd header")
    {
        m_frame->setFrameSetInfo( KWFrameSet::FI_EVEN_HEADER );
    }
    else if ( _type.lower() =="first footer")
    {
        m_frame->setFrameSetInfo( KWFrameSet::FI_FIRST_FOOTER );
    }
    else if ( _type.lower() =="odd footer")
    {
        m_frame->setFrameSetInfo( KWFrameSet::FI_EVEN_FOOTER );
    }
    else if ( _type.lower() =="even footer")
    {
        m_frame->setFrameSetInfo( KWFrameSet::FI_ODD_FOOTER );
    }
    else if ( _type.lower() =="even footer")
    {
        m_frame->setFrameSetInfo( KWFrameSet::FI_ODD_FOOTER );
    }
    else if ( _type.lower() =="footnote")
    {
        m_frame->setFrameSetInfo( KWFrameSet::FI_FOOTNOTE );
    }
    else
    {
        kDebug()<<" Error in setFrameSetInfo() :"<<_type<<endl;
    }
}


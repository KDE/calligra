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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "KWordFrameSetIface.h"
#include "kwframe.h"
#include "kwview.h"
#include "kwdoc.h"
#include <kapplication.h>
#include <dcopclient.h>


KWordFrameSetIface::KWordFrameSetIface( KWFrameSet *_frame )
    : DCOPObject()
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
    return m_frame->frame(0)->normalize().width();
}

double KWordFrameSetIface::ptHeight() const
{
    return m_frame->frame(0)->normalize().height();
}

double KWordFrameSetIface::ptPosX() const
{
    return m_frame->frame(0)->normalize().x();
}

double KWordFrameSetIface::ptPosY() const
{
    return m_frame->frame(0)->normalize().y();
}

int KWordFrameSetIface::zOrder() const
{
    return m_frame->frame(0)->zOrder();
}

int KWordFrameSetIface::pageNum() const
{
    return m_frame->frame(0)->pageNum();
}

QBrush KWordFrameSetIface::backgroundColor() const
{
    return m_frame->frame(0)->backgroundColor();
}

double KWordFrameSetIface::ptMarginLeft()const
{
    return m_frame->frame(0)->bLeft();
}

double KWordFrameSetIface::ptMarginRight()const
{
    return m_frame->frame(0)->bRight();
}

double KWordFrameSetIface::ptMarginTop()const
{
    return m_frame->frame(0)->bTop();
}

double KWordFrameSetIface::ptMarginBottom()const
{
    return m_frame->frame(0)->bBottom();
}

bool KWordFrameSetIface::isCopy()const
{
    return m_frame->frame(0)->isCopy();
}

void KWordFrameSetIface::setPtMarginLeft(double val)
{
    m_frame->frame( 0 )->setBLeft(val);
    m_frame->kWordDocument()->layout();
}

void KWordFrameSetIface::setPtMarginRight(double val)
{
    m_frame->frame( 0 )->setBRight(val);
    m_frame->kWordDocument()->layout();

}
void KWordFrameSetIface::setPtMarginTop(double val)
{
    m_frame->frame( 0 )->setBTop(val);
    m_frame->kWordDocument()->layout();
}

void KWordFrameSetIface::setPtMarginBottom(double val)
{
    m_frame->frame( 0 )->setBBottom(val);
    m_frame->kWordDocument()->layout();
}

QColor KWordFrameSetIface::leftBorderColor() const
{
    return m_frame->frame( 0 )->leftBorder().color;
}

QColor KWordFrameSetIface::rightBorderColor() const
{
    return m_frame->frame( 0 )->rightBorder().color;
}

QColor KWordFrameSetIface::topBorderColor() const
{
    return m_frame->frame( 0 )->topBorder().color;
}

QColor KWordFrameSetIface::bottomBorderColor() const
{
    return m_frame->frame( 0 )->bottomBorder().color;
}


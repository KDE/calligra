/* This file is part of the KDE libraries
   Copyright (c) 1998 Stefan Taferner
                      2001/2003 thierry lorthiois (lorthioist@wanadoo.fr)

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kowmfstack.h"
#include "kowmfread.h"

void KoWmfBrushHandle::apply( KoWmfRead *p ) {
    p->setBrush( brush );
}

void KoWmfPenHandle::apply( KoWmfRead *p ) {
    p->setPen( pen );
}

void KoWmfPatternBrushHandle::apply( KoWmfRead *p ) {
    p->setBrush( brush );
}

void KoWmfFontHandle::apply( KoWmfRead *p ) {
    p->setFont( font );
}


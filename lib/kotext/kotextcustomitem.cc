/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>

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

#include "qrichtext_p.h"
#include "kozoomhandler.h"
#include <kdebug.h>
#include <kcommand.h>

int KoTextCustomItem::index() const
{
    Q_ASSERT( paragraph() );
    KoTextParag * parag = paragraph();
    return parag->findCustomItem( this );
}

KoTextFormat * KoTextCustomItem::format() const
{
    KoTextParag * parag = paragraph();
    //kdDebug() << "KoTextCustomItem::format index=" << index() << " format=" << parag->at( index() )->format() << endl;
    return parag->at( index() )->format();
}


void KoTextCustomItem::draw(QPainter* p, int x, int _y, int cx, int cy, int cw, int ch, const QColorGroup& cg, bool selected )
{
    KoZoomHandler *zh=textDocument()->paintingZoomHandler();
    //kdDebug()<<" x :"<<x<<" y :"<<y<<" cx :"<<cx<<" cy :"<<cy<<" ch :"<<ch<<" cw :"<<cw<<endl;

    x=zh->layoutUnitToPixelX(x) + paragraph()->at( index() )->pixelxadj;
    int y=zh->layoutUnitToPixelY(_y);
    cx=zh->layoutUnitToPixelX(cx);
    cy=zh->layoutUnitToPixelY(_y,cy);
    ch=zh->layoutUnitToPixelY(_y,ch);
    cw=zh->layoutUnitToPixelX(cw);
    //kdDebug()<<"After  x :"<<x<<" y :"<<y<<" cx :"<<cx<<" cy :"<<cy<<" ch :"<<ch<<" cw :"<<cw<<endl;

    KoTextFormat * fmt = format();

    //bool forPrint = ( p->device()->devType() == QInternal::Printer );
    p->setFont( fmt->screenFont( zh ) );

    int offset=0;
    if ( fmt->vAlign() == KoTextFormat::AlignSuperScript )
    {
        int h = zh->layoutUnitToPixelY( _y, height );
        offset = -( h - p->fontMetrics().height() );
    }

    drawCustomItem(p, x, y, cx, cy, cw, ch, cg, selected, offset);
}

////////////////

void CustomItemsMap::insertItems( const KoTextCursor & startCursor, int size )
{
    if ( isEmpty() )
        return;

    KoTextCursor cursor( startCursor );
    for ( int i = 0; i < size; ++i )
    {
        CustomItemsMap::Iterator it = find( i );
        if ( it != end() )
        {
            kdDebug() << "CustomItemsMap::insertItems setting custom item " << it.data() << endl;
            cursor.parag()->setCustomItem( cursor.index(), it.data(), 0 );
            it.data()->setDeleted( false );
        }
        cursor.gotoRight();
    }
}

void CustomItemsMap::deleteAll( KMacroCommand *macroCmd )
{
    Iterator it = begin();
    for ( ; it != end(); ++it )
    {
        KoTextCustomItem * item = it.data();
        KCommand * itemCmd = item->deleteCommand();
        if ( itemCmd && macroCmd )
        {
            macroCmd->addCommand( itemCmd );
            itemCmd->execute(); // the item-specific delete stuff hasn't been done
        }
        item->setDeleted( true );
    }
}

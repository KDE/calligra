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

#include "kwtextparag.h"
#include "kwtextdocument.h"
#include "kwdoc.h"
#include "kwtextframeset.h"
#include "kwformat.h"
#include <kcommand.h>
#include <kdebug.h>

KoTextDocument::KoTextDocument( KoZoomHandler * zoomHandler, QTextDocument *p, KWTextFormatCollection *fc )
    : QTextDocument( p, fc ), m_zoomHandler( zoomHandler ), m_bDestroying( false )
{
    setAddMargins( true );                 // top margin and bottom are added, not max'ed
    QTextFormatter * formatter = new QTextFormatterBreakWords;
    formatter->setAllowBreakInWords( true ); // Necessary for lines without a single space
    setFormatter( formatter );

    setY( 0 );
    setLeftMargin( 0 );
    setRightMargin( 0 );

    // TODO clear(true) here if kpresenter uses kotextdocument directly,
    // otherwise in the derived constructor [virtual method call is the problem]
}

KoTextDocument::~KoTextDocument()
{
    m_bDestroying = true;
    clear( false );
}

QTextParag * KoTextDocument::createParag( QTextDocument *d, QTextParag *pr, QTextParag *nx, bool updateIds )
{
    return new KoTextParag( d, pr, nx, updateIds );
}

bool KoTextDocument::visitSelection( int selectionId, KWParagVisitor* visitor, bool forward )
{
    QTextCursor c1 = selectionStartCursor( selectionId );
    QTextCursor c2 = selectionEndCursor( selectionId );
    if ( c1 == c2 )
        return true;
    return visitFromTo( c1.parag(), c1.index(), c2.parag(), c2.index(), visitor, forward );
}

bool KoTextDocument::visitDocument( KWParagVisitor *visitor, bool forward )
{
    return visitFromTo( firstParag(), 0, lastParag(), lastParag()->length()-1, visitor, forward );
}

bool KoTextDocument::visitFromTo( QTextParag *firstParag, int firstIndex, QTextParag* lastParag, int lastIndex, KWParagVisitor* visitor, bool forw )
{
    if ( firstParag == lastParag )
    {
        return visitor->visit( firstParag, firstIndex, lastIndex );
    }
    else
    {
        bool ret = true;
        if ( forw )
        {
            // the -1 is for the trailing space
            ret = visitor->visit( firstParag, firstIndex, firstParag->length() - 1 );
            if (!ret) return false;
        }
        else
        {
            ret = visitor->visit( lastParag, 0, lastIndex );
            if (!ret) return false;
        }

        QTextParag* currentParag = forw ? firstParag->next() : lastParag->prev();
        QTextParag * endParag = forw ? lastParag : firstParag;
        while ( currentParag && currentParag != endParag )
        {
            ret = visitor->visit( currentParag, 0, currentParag->length() - 1 );
            if (!ret) return false;
            currentParag = forw ? currentParag->next() : currentParag->prev();
        }
        ASSERT( currentParag );
        ASSERT( endParag == currentParag );
        if ( forw )
            ret = visitor->visit( lastParag, 0, lastIndex );
        else
            ret = visitor->visit( currentParag, firstIndex, currentParag->length() - 1 );
        return ret;
    }
}

////

KWTextDocument::KWTextDocument( KWTextFrameSet * textfs, QTextDocument *p, KWTextFormatCollection *fc )
    : KoTextDocument( textfs->kWordDocument(), p, fc ), m_textfs( textfs )
{
    init();
}

KWTextDocument::KWTextDocument( KoZoomHandler * zoomHandler )
    : KoTextDocument( zoomHandler, 0, new KWTextFormatCollection( QFont("helvetica") /*unused*/ ) ),
      m_textfs( 0 )
{
    init();
    setWidth( 1000 );
}

void KWTextDocument::init()
{
    // QTextDocument::QTextDocument creates a parag, but too early for our createParag to get called !
    // So we have to get rid of it.
    clear( true );
    // Using clear( false ) is a bit dangerous, since we don't always check cursor->parag() for != 0
}

QTextParag * KWTextDocument::createParag( QTextDocument *d, QTextParag *pr, QTextParag *nx, bool updateIds )
{
    return new KWTextParag( d, pr, nx, updateIds );
}

int KWTextCustomItem::index() const
{
    ASSERT( paragraph() );
    KWTextParag * parag = static_cast<KWTextParag *>( paragraph() );
    return parag->findCustomItem( this );
}

QTextFormat * KWTextCustomItem::format() const
{
    QTextParag * parag = paragraph();
    //kdDebug() << "KWTextCustomItem::format index=" << index() << " format=" << parag->at( index() )->format() << endl;
    return parag->at( index() )->format();
}

void CustomItemsMap::insertItems( const QTextCursor & startCursor, int size )
{
    if ( isEmpty() )
        return;

    QTextCursor cursor( startCursor );
    for ( int i = 0; i < size; ++i )
    {
        CustomItemsMap::Iterator it = find( i );
        if ( it != end() )
        {
            kdDebug() << "CustomItemsMap::insertItems setting custom item " << it.data() << endl;
            static_cast<KWTextParag *>(cursor.parag())->setCustomItem( cursor.index(), it.data(), 0 );
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
        KWTextCustomItem * item = it.data();
        KCommand * itemCmd = item->deleteCommand();
        if ( itemCmd && macroCmd )
        {
            macroCmd->addCommand( itemCmd );
            itemCmd->execute(); // the item-specific delete stuff hasn't been done
        }
        item->setDeleted( true );
    }
}

#include "kwtextdocument.moc"

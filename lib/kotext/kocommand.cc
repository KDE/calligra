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

#include "kocommand.h"
#include "kotextobject.h"
#include "kotextparag.h"
#include <kdebug.h>

// This is automatically called by KCommandHistory's redo action when redo is activated
void KoTextCommand::execute()
{
    m_textobj->redo();
}

// This is automatically called by KCommandHistory's undo action when undo is activated
void KoTextCommand::unexecute()
{
    m_textobj->undo();
}

KoTextDeleteCommand::KoTextDeleteCommand(
    QTextDocument *d, int i, int idx, const QArray<QTextStringChar> &str,
    const CustomItemsMap & customItemsMap,
    const QValueList<KoParagLayout> &oldParagLayouts )
    : QTextDeleteCommand( d, i, idx, str,
                          QValueList< QVector<QStyleSheetItem> >(),
                          QValueList<QStyleSheetItem::ListStyle>(),
                          QArray<int>() ),
      m_oldParagLayouts( oldParagLayouts ),
      m_customItemsMap( customItemsMap )
{
    // Note that we don't pass aligns and liststyles to QTextDeleteCommand.
    // We'll handle them here, as part of the rest, since they are in the paraglayouts
}

QTextCursor * KoTextDeleteCommand::execute( QTextCursor *c )
{
    QTextParag *s = doc ? doc->paragAt( id ) : parag;
    if ( !s ) {
        qWarning( "can't locate parag at %d, last parag: %d", id, doc->lastParag()->paragId() );
        return 0;
    }
    cursor.setParag( s );
    cursor.setIndex( index );
    int len = text.size();
    // Detach from custom items. They are already in the map, and we don't
    // want them to be deleted
    for ( int i = 0; i < len; ++i )
    {
        QTextStringChar * ch = cursor.parag()->at( cursor.index() );
        if ( ch->isCustom() )
        {
            static_cast<KoTextCustomItem *>( ch->customItem() )->setDeleted( true );
            static_cast<KoTextParag*>(cursor.parag())->removeCustomItem(cursor.index());
        }
        cursor.gotoRight();
    }

    return QTextDeleteCommand::execute(c);
}

QTextCursor * KoTextDeleteCommand::unexecute( QTextCursor *c )
{
    // Let QRichText re-create the text and formatting
    QTextCursor * cr = QTextDeleteCommand::unexecute(c);

    QTextParag *s = doc ? doc->paragAt( id ) : parag;
    if ( !s ) {
        qWarning( "can't locate parag at %d, last parag: %d", id, doc->lastParag()->paragId() );
        return 0;
    }
    cursor.setParag( s );
    cursor.setIndex( index );
    // Set any custom item that we had
    m_customItemsMap.insertItems( cursor, text.size() );

    // Now restore the parag layouts (i.e. libkotext specific stuff)
    QValueList<KoParagLayout>::Iterator lit = m_oldParagLayouts.begin();
    kdDebug() << "KoTextDeleteCommand::unexecute " << m_oldParagLayouts.count() << " parag layouts. First parag=" << s->paragId() << endl;
    ASSERT( id == s->paragId() );
    QTextParag *p = s;
    while ( p ) {
        if ( lit != m_oldParagLayouts.end() )
        {
            kdDebug() << "KoTextDeleteCommand::unexecute applying paraglayout to parag " << p->paragId() << endl;
            static_cast<KoTextParag*>(p)->setParagLayout( *lit );
        }
        else
            break;
        //if ( s == cr->parag() )
        //    break;
        p = p->next();
        ++lit;
    }
    return cr;
}

KoTextParagCommand::KoTextParagCommand( QTextDocument *d, int fParag, int lParag,
                                        const QValueList<KoParagLayout> &oldParagLayouts,
                                        KoParagLayout newParagLayout,
                                        int flags,
                                        QStyleSheetItem::Margin margin )
    : QTextCommand( d ), firstParag( fParag ), lastParag( lParag ), m_oldParagLayouts( oldParagLayouts ),
      m_newParagLayout( newParagLayout ), m_flags( flags ), m_margin( margin )
{
}

QTextCursor * KoTextParagCommand::execute( QTextCursor *c )
{
    //kdDebug() << "KoTextParagCommand::execute" << endl;
    KoTextParag *p = static_cast<KoTextParag *>(doc->paragAt( firstParag ));
    if ( !p )
    {
        kdWarning() << "KoTextParagCommand::execute paragraph " << firstParag << "not found" << endl;
        return c;
    }
    while ( p ) {
        if ( ( m_flags & KoParagLayout::Margins ) && m_margin != (QStyleSheetItem::Margin)-1 ) // all
            p->setMargin( static_cast<QStyleSheetItem::Margin>(m_margin), m_newParagLayout.margins[m_margin] );
        else
        {
            p->setParagLayout( m_newParagLayout, m_flags );
        }
        if ( p->paragId() == lastParag )
            break;
        p = static_cast<KoTextParag *>(p->next());
    }
    //kdDebug() << "KoTextParagCommand::execute done" << endl;
    // Set cursor to end of selection. Like in QTextFormatCommand::[un]execute...
    c->setParag( p );
    c->setIndex( p->length()-1 );
    return c;
}

QTextCursor * KoTextParagCommand::unexecute( QTextCursor *c )
{
    KoTextParag *p = static_cast<KoTextParag *>(doc->paragAt( firstParag ));
    if ( !p )
    {
        kdDebug() << "KoTextParagCommand::unexecute paragraph " << firstParag << "not found" << endl;
        return c;
    }
    QValueList<KoParagLayout>::Iterator lit = m_oldParagLayouts.begin();
    while ( p ) {
        if ( lit == m_oldParagLayouts.end() )
        {
            kdDebug() << "KoTextParagCommand::unexecute m_oldParagLayouts not big enough!" << endl;
            break;
        }
        if ( m_flags & KoParagLayout::Margins && m_margin != (QStyleSheetItem::Margin)-1 ) // just one
            p->setMargin( static_cast<QStyleSheetItem::Margin>(m_margin), (*lit).margins[m_margin] );
        else
        {
            p->setParagLayout( *lit, m_flags );
        }
        if ( p->paragId() == lastParag )
            break;
        p = static_cast<KoTextParag *>(p->next());
        ++lit;
    }
    // Set cursor to end of selection. Like in QTextFormatCommand::[un]execute...
    c->setParag( p );
    c->setIndex( p->length()-1 );
    return c;
}

//////////

KoParagFormatCommand::KoParagFormatCommand( QTextDocument *d, int fParag, int lParag,
                                                          const QValueList<QTextFormat *> &oldFormats,
                                                          QTextFormat * newFormat )
    : QTextCommand( d ), firstParag( fParag ), lastParag( lParag ), m_oldFormats( oldFormats ),
      m_newFormat( newFormat )
{
    QValueList<QTextFormat *>::Iterator lit = m_oldFormats.begin();
    for ( ; lit != m_oldFormats.end() ; ++lit )
        (*lit)->addRef();
}

KoParagFormatCommand::~KoParagFormatCommand()
{
    QValueList<QTextFormat *>::Iterator lit = m_oldFormats.begin();
    for ( ; lit != m_oldFormats.end() ; ++lit )
        (*lit)->removeRef();
}

QTextCursor * KoParagFormatCommand::execute( QTextCursor *c )
{
    KoTextParag *p = static_cast<KoTextParag *>(doc->paragAt( firstParag ));
    if ( !p )
    {
        kdDebug() << "KoTextParagCommand::execute paragraph " << firstParag << "not found" << endl;
        return c;
    }
    while ( p ) {
        p->setFormat( m_newFormat );
        p->invalidate(0);
        if ( p->paragId() == lastParag )
            break;
        p = static_cast<KoTextParag *>(p->next());
    }
    return c;
}

QTextCursor * KoParagFormatCommand::unexecute( QTextCursor *c )
{
    QTextParag *p = doc->paragAt( firstParag );
    if ( !p )
    {
        kdDebug() << "KoParagFormatCommand::unexecute paragraph " << firstParag << "not found" << endl;
        return c;
    }
    QValueList<QTextFormat *>::Iterator lit = m_oldFormats.begin();
    while ( p ) {
        if ( lit == m_oldFormats.end() )
        {
            kdDebug() << "KoParagFormatCommand::unexecute m_oldFormats not big enough!" << endl;
            break;
        }
        p->setFormat( (*lit) );
        if ( p->paragId() == lastParag )
            break;
        p = p->next();
        ++lit;
    }
    return c;
}

KoTextFormatCommand::KoTextFormatCommand(QTextDocument *d, int sid, int sidx, int eid, int eidx, const QMemArray<QTextStringChar> &old, QTextFormat *f, int fl )
    : QTextFormatCommand(d, sid, sidx, eid, eidx, old, f, fl)
{
}


KoTextFormatCommand::~KoTextFormatCommand()
{
}

void KoTextFormatCommand::resizeCustomItems()
{
    QTextParag *sp = doc->paragAt( startId );
    QTextParag *ep = doc->paragAt( endId );
    if ( !sp || !ep )
        return;

    QTextCursor start( doc );
    start.setParag( sp );
    start.setIndex( startIndex );
    QTextCursor end( doc );
    end.setParag( ep );
    end.setIndex( endIndex );

    doc->setSelectionStart( QTextDocument::Temp, &start );
    doc->setSelectionEnd( QTextDocument::Temp, &end );

    // TODO use the visitor pattern (some 'ResizeCustomItemVisitor')

    if ( start.parag() == end.parag() )
    {
        QString text = start.parag()->string()->toString().mid( start.index(), end.index() - start.index() );
        for ( int i = start.index(); i < end.index(); ++i )
        {
            if( start.parag()->at(i)->isCustom())
            {
                static_cast<KoTextCustomItem *>( start.parag()->at(i)->customItem() )->resize();
            }
        }
    }
    else
    {
        int i;
        QString text = start.parag()->string()->toString().mid( start.index(), start.parag()->length() - 1 - start.index() );
        for ( i = start.index(); i < start.parag()->length(); ++i )
            if( start.parag()->at(i)->isCustom())
            {
                static_cast<KoTextCustomItem *>( start.parag()->at(i)->customItem() )->resize();
            }

        QTextParag *p = start.parag()->next();
        while ( p && p != end.parag() )
        {
            text = p->string()->toString().left( p->length() - 1 );
            for ( i = 0; i < p->length(); ++i )
            {
               if( p->at(i)->isCustom())
               {
                   static_cast<KoTextCustomItem *>(p->at(i)->customItem() )->resize();
               }
            }
            p = p->next();
        }
        text = end.parag()->string()->toString().left( end.index() );
        for ( i = 0; i < end.index(); ++i )
        {
            if( end.parag()->at(i)->isCustom())
            {
                static_cast<KoTextCustomItem *>( end.parag()->at(i)->customItem() )->resize();
            }
        }
    }
}

QTextCursor *KoTextFormatCommand::execute( QTextCursor *c )
{
    c = QTextFormatCommand::execute( c );
    resizeCustomItems();
    return c;
}

QTextCursor *KoTextFormatCommand::unexecute( QTextCursor *c )
{
    c = QTextFormatCommand::unexecute( c );
    resizeCustomItems();
    return c;
}

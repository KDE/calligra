/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include <qlist.h>

#include <koRuler.h>

#include "contents.h"
#include "kwdoc.h"
#include "kwframe.h"
#include "kwtextframeset.h"

#include <klocale.h>
#include <kdebug.h>

/******************************************************************
 *
 * Class: KWContents
 *
 ******************************************************************/

/*================================================================*/
KWContents::KWContents( KWDocument *doc )
    : m_doc( doc )
{
}

/*================================================================*/
void KWContents::createContents()
{
    KWTextFrameSet *fs = dynamic_cast<KWTextFrameSet *>(m_doc->getFrameSet( 0 ));
    ASSERT(fs); if (!fs) return;

    QTextDocument * textdoc = fs->textDocument();

    // Remove existing table of contents, using m_paragIds
    if ( !m_paragIds.isEmpty() ) {
        QList<QTextParag> paragList;     // list of paragraphs to delete
        for ( QTextParag *p = textdoc->firstParag(); p ; p = p->next() )
        {
            if ( m_paragIds.contains( p->paragId() ) )
            {
                m_paragIds.remove( p->paragId() );
                kdDebug() << "KWContents::createContents will delete paragraph " << p->paragId() << "   " << p << endl;
                paragList.append( p );
                if ( m_paragIds.isEmpty() )   // done
                    break;
            }
            // Note that we skip paragraphs that have been manually added in the middle of the TOC
        }
        ASSERT( m_paragIds.isEmpty() );

        kdDebug() << "KWContents::createContents " << paragList.count() << " parags to delete" << endl;

        // Now delete them (we can't do that above, it shiftes the Ids)
        QTextCursor cursor( textdoc );
        QListIterator<QTextParag> it( paragList );
        for ( ; it.current() ; ++it )
        {
            kdDebug() << "KWContents::createContents Deleting paragraph " << it.current() << endl;
            // This paragraph is part of the TOC -> remove
            cursor.setParag( it.current() );
            cursor.setIndex( 0 );
            textdoc->setSelectionStart( QTextDocument::Temp, &cursor );
            cursor.setIndex( it.current()->string()->length() );
            textdoc->setSelectionEnd( QTextDocument::Temp, &cursor );
            textdoc->removeSelectedText( QTextDocument::Temp, &cursor );
        }
    }

    m_paragIds.clear();

#if 0
    QList<KoTabulator> tabList;
    KoTabulator *tab = new KoTabulator;
    tab->ptPos = fs->getFrame( 0 )->width() - 10;
    tab->mmPos = POINT_TO_MM( tab->ptPos );
    tab->inchPos = POINT_TO_INCH( tab->ptPos );
    tab->type = T_RIGHT;
    tabList.append( tab );
#endif

    // Create new very first paragraph
    KWTextParag *parag = static_cast<KWTextParag *>( textdoc->createParag( textdoc, 0, textdoc->firstParag(), true ) );
    parag->append( i18n( "Table of Contents" ) );
    //parag->setInfo( KWParag::PI_CONTENTS );
    KWStyle * style = m_doc->findStyle( "Contents Title" );
    if ( style )
    {
        parag->setParagLayout( style->paragLayout() );
        parag->setFormat( 0, parag->string()->length(), &style->format() );
    }
    m_paragIds.append( parag->paragId() );

    // Insert table and THEN set page numbers
    // Otherwise the page numbers are incorrect

    KWTextParag *p = static_cast<KWTextParag *>(parag->next());
    KWTextParag *prevTOCParag = parag;
    KWTextParag *body = p;
    QMap<KWTextParag *, KWTextParag *> paragMap;     // Associate a paragraph form the TOC with the real one from the body
    while ( p ) {
        // We recognize headers by the "numbering" property of the paragraph
        // This way, we don't rely on a particular name (breaks when renaming)
        // nor on whether the user used styles or not.
        // It even lets a user create two styles for the same level of chapter.
        if ( p->counter() && p->counter()->m_numbering == Counter::NUM_CHAPTER )
        {
            parag = static_cast<KWTextParag *>(textdoc->createParag( textdoc, prevTOCParag /*prev*/, body /*next*/, true ));
            //parag->setInfo( KWParag::PI_CONTENTS );
            QString txt = p->string()->toString();
            txt.prepend( p->counter()->text(p) );
            parag->append( txt );
            prevTOCParag = parag;

            m_paragIds.append( parag->paragId() );
            paragMap.insert( parag, p );
        }
        p = static_cast<KWTextParag *>(p->next());
    }

    // Now add the page numbers, and apply the style
    QMap<KWTextParag *, KWTextParag *>::Iterator mapIt = paragMap.begin();
    for ( ; mapIt != paragMap.end() ; ++mapIt )
    {
        KWTextParag * parag = mapIt.key(); // Parag in the TOC
        KWTextParag * p = mapIt.data();    // Parag in the body
        // TODO pl->setTabList( &tabList );
        // TODO parag->insertTab( txt.length() );

        // Find page number for paragraph - maybe not the best way !
        int pgNum = fs->internalToContents( QPoint(0, p->rect().top()) ).y() / m_doc->ptPaperWidth();
        parag->append( "         " ); // HACK, should be a tab
        parag->append( QString::number( pgNum ) );

        // Apply style
        int depth = p->counter()->m_depth;    // we checked for p->counter() before putting in the map
        KWStyle * tocStyle
            = m_doc->findStyle( QString( "Contents Head %1" ).arg( depth ) );
        parag->setParagLayout( tocStyle->paragLayout() );
        parag->setFormat( 0, parag->string()->length(), &tocStyle->format() );
    }

    // Is that "jump to next page" ? We need that then.
    //if ( parag->getNext() )
    //    parag->getNext()->setHardBreak( TRUE );
}

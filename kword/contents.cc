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
        QTextParag *start = textdoc->firstParag();
        QTextParag *p = start;
        while ( p ) {
            if ( m_paragIds.contains( p->paragId() ) )
            {
                m_paragIds.remove( p->paragId() );
                paragList.append( p );
                if ( m_paragIds.isEmpty() )   // done
                    break;
            }
            // Note that we skip paragraphs that have been manually added in the middle of the TOC
            p = p->next();
        }
        ASSERT( m_paragIds.isEmpty() );
        // Now delete them (we can't do that above, it shiftes the Ids)

        p = start;
        QTextCursor cursor( textdoc );
        for ( QTextParag *p = paragList.first() ; p ; p = paragList.next() );
        {
            // This paragraph is part of the TOC -> remove
            cursor.setParag( p );
            cursor.setIndex( 0 );
            textdoc->setSelectionStart( QTextDocument::Temp, &cursor );
            cursor.setIndex( p->string()->length() );
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

    KWTextParag *p = static_cast<KWTextParag *>(parag->next());
    KWTextParag *begin = parag;
    KWTextParag *body = p;
    while ( p ) {
        if ( p->styleName().contains( "Head" ) &&
             !p->styleName().contains( "Contents" ) ) {
#if 0 // TODO
            int depth = ///p->getParagLayout()->getName().right( 1 ).toInt();
            KWStyle * tocStyle
                = m_doc->findStyle( QString( "Contents Head %1" ).arg( depth ) );
            pl->setTabList( &tabList );
            parag = new KWParag( fs, doc, begin, body, pl );
            parag->setInfo( KWParag::PI_CONTENTS );
            QString txt = p->getKWString()->toString();
            txt.prepend( p->getCounterText() + " " );
            parag->insertText( 0,  txt );
            parag->setFormat( 0, txt.length(), pl->getFormat() );
            parag->insertTab( txt.length() );
            int pgNum = p->getStartPage();
            if ( pgNum == dec )
                pgNum -= dec - 1;
            else
                pgNum -= dec;
            txt = QString::number( pgNum );
            int i = parag->getTextLen();
            parag->insertText( i, txt );
            parag->setFormat( i, txt.length(), pl->getFormat() );
            begin = parag;
#endif
            m_paragIds.append( parag->paragId() );
        }
        p = static_cast<KWTextParag *>(p->next());
    }

    // Is that "jump to next page" ?
    //if ( parag->getNext() )
    //    parag->getNext()->setHardBreak( TRUE );

    //end = parag;
}

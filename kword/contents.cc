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

    // Remove existing table of contents, based on the style

    QTextCursor start( textdoc );
    QTextCursor end( textdoc );
    for ( QTextParag *p = textdoc->firstParag(); p ; p = p->next() )
    {
        KWTextParag * parag = static_cast<KWTextParag *>(p);
        if ( parag->styleName().startsWith( "Contents Head" ) ||
            parag->styleName() == "Contents Title" )
        {
            kdDebug() << "KWContents::createContents Deleting paragraph " << p << endl;
            // This paragraph is part of the TOC -> remove
            start.setParag( p );
            start.setIndex( 0 );
            textdoc->setSelectionStart( QTextDocument::Temp, &start );
            ASSERT( p->next() );
            end.setParag( p->next() );
            end.setIndex( 0 );
            textdoc->setSelectionEnd( QTextDocument::Temp, &end );
            textdoc->removeSelectedText( QTextDocument::Temp, &end );
        }
    }

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
    KWStyle * style = findOrCreateTOCStyle( -1 ); // "Contents Title"
    parag->setParagLayout( style->paragLayout() );
    parag->setFormat( 0, parag->string()->length(), &style->format() );

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
        if ( p->counter() && p->counter()->numbering() == Counter::NUM_CHAPTER )
        {
            parag = static_cast<KWTextParag *>(textdoc->createParag( textdoc, prevTOCParag /*prev*/, body /*next*/, true ));
            //parag->setInfo( KWParag::PI_CONTENTS );
            QString txt = p->string()->toString();
            txt.prepend( p->counter()->text(p) );
            parag->append( txt );
            prevTOCParag = parag;

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

        // Find page number for paragraph
        QPoint pt;
        KWFrame * frame = fs->internalToContents( QPoint(0, p->rect().top()), pt );
        if ( frame ) // let's be safe
        {
            parag->append( "         " ); // HACK, should be a tab
            parag->append( QString::number( frame->getPageNum() ) );
        }

        // Apply style
        int depth = p->counter()->depth();    // we checked for p->counter() before putting in the map
        KWStyle * tocStyle = findOrCreateTOCStyle( depth );
        parag->setParagLayout( tocStyle->paragLayout() );
        parag->setFormat( 0, parag->string()->length(), &tocStyle->format() );
    }

    // Is that "jump to next page" ? We need that then.
    //if ( parag->getNext() )
    //    parag->getNext()->setHardBreak( TRUE );
}

#if 0
void KWContents::restoreParagList( QValueList<int> paragIds )
{
    KWTextFrameSet *fs = dynamic_cast<KWTextFrameSet *>(m_doc->getFrameSet( 0 ));
    ASSERT(fs); if (!fs) return;
    QTextDocument * textdoc = fs->textDocument();

    for ( QTextParag *p = textdoc->firstParag(); p ; p = p->next() )
    {
        if ( paragIds.contains( p->paragId() ) )
        {
            paragIds.remove( p->paragId() );
            m_paragList.append( p );
            if ( m_paragIds.isEmpty() )   // done
                break;
        }
        // Note that we skip paragraphs that have been manually added in the middle of the TOC
    }
    ASSERT( paragIds.isEmpty() );
}

QValueList<int> saveParagList() const
{
    QValueList<int> result;
    QListIterator<QTextParag> it( m_paragList );
    for ( ; it.current() ; ++it )
    {
        result.append( it.current()->paragId() );
    }
    return result;
}
#endif

KWStyle * KWContents::findOrCreateTOCStyle( int depth )
{
    // Determine style name.
    // NOTE: don't add i18n here ! This is translated using
    // the i18n calls in stylenames.cc !
    QString name;
    if ( depth >= 0 )
        name = QString( "Contents Head %1" ).arg( depth+1 );
    else
        name = "Contents Title";
    KWStyle * style = m_doc->findStyle( name, true );
    if ( !style )
    {
        style = new KWStyle( name );
        style->format().setBold(true);
        style->format().setPointSize( depth==-1 ? 20 : depth==0 ? 16 : 12 );
        if ( depth == -1 )
        {
            style->paragLayout().topBorder = Border( Qt::black, Border::SOLID, 1 );
            style->paragLayout().bottomBorder = Border( Qt::black, Border::SOLID, 1 );
            style->paragLayout().alignment = Qt::AlignCenter;
        }
        m_doc->addStyleTemplate( style );             // register the new style
        m_doc->updateAllStyleLists();                 // show it in the UI
    }
    return style;
}

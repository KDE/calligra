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
#include "kwstyle.h"
#include "kwcommand.h"
#include "counter.h"
#include "kwtextframeset.h"

#include <kcommand.h>
#include <klocale.h>
#include <kdebug.h>

KWInsertTOCCommand::KWInsertTOCCommand( KWTextFrameSet * fs )
    : QTextCommand( fs->textDocument() )
{
}

QTextCursor * KWInsertTOCCommand::execute( QTextCursor *c )
{
    KWTextDocument * textdoc = static_cast<KWTextDocument *>(doc);
    KWTextFrameSet * fs = textdoc->textFrameSet();

    fs->kWordDocument()->renameButtonTOC(i18n("Update Table of &Contents"));

    KWTextParag *body = static_cast<KWTextParag *>( textdoc->firstParag() );
    // Create new very first paragraph
    KWTextParag *parag = static_cast<KWTextParag *>( textdoc->createParag( textdoc, 0, textdoc->firstParag(), true ) );
    parag->append( i18n( "Table of Contents" ) );
    KWStyle * style = findOrCreateTOCStyle( fs, -1 ); // "Contents Title"
    parag->setParagLayout( style->paragLayout() );
    parag->setFormat( 0, parag->string()->length(), textdoc->formatCollection()->format( &style->format() ) );

    // Insert table and THEN set page numbers
    // Otherwise the page numbers are incorrect

    KWTextParag *p = static_cast<KWTextParag *>(parag->next());
    ASSERT( p == body );
    KWTextParag *prevTOCParag = parag;
    QMap<KWTextParag *, KWTextParag *> paragMap;     // Associate a paragraph form the TOC with the real one from the body
    while ( p ) {
        // We recognize headers by the "numbering" property of the paragraph
        // This way, we don't rely on a particular name (breaks when renaming)
        // nor on whether the user used styles or not.
        // It even lets a user create two styles for the same level of chapter.
        if ( p->counter() && p->counter()->numbering() == Counter::NUM_CHAPTER )
        {
            parag = static_cast<KWTextParag *>(textdoc->createParag( textdoc, prevTOCParag /*prev*/, body /*next*/, true ));
            QString txt = p->string()->toString();
            txt = txt.left( txt.length() - 1 ); // remove trailing space
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

        // Find page number for paragraph
        QPoint pt;
        KWFrame * frame = fs->internalToNormal( QPoint(0, p->rect().top()), pt );
        if ( frame ) // let's be safe
        {
            parag->append( "\t" );
            parag->append( QString::number( frame->pageNum() ) );
        }

        // Apply style
        int depth = p->counter()->depth();    // we checked for p->counter() before putting in the map
        KWStyle * tocStyle = findOrCreateTOCStyle( fs, depth );
        parag->setParagLayout( tocStyle->paragLayout() );
        KWTextFormat * newFormat = fs->zoomFormatFont( & tocStyle->format() );
        parag->setFormat( 0, parag->string()->length(), newFormat );
    }
    // Set a hard frame break after the last TOC parag
    prevTOCParag->setPageBreaking( prevTOCParag->pageBreaking() | KWParagLayout::HardFrameBreakAfter );
    return c;
}

QTextCursor *KWInsertTOCCommand::unexecute( QTextCursor *c )
{
    KWTextDocument * textdoc = static_cast<KWTextDocument *>(doc);
    KWTextFrameSet * fs = textdoc->textFrameSet();

    removeTOC( fs, c, 0L );
    fs->kWordDocument()->renameButtonTOC(i18n("Table of &Contents"));
    return c;
}

void KWInsertTOCCommand::removeTOC( KWTextFrameSet *fs, QTextCursor *cursor, KMacroCommand *macroCmd )
{
    KWTextDocument * textdoc = fs->textDocument();
    // Remove existing table of contents, based on the style
    QTextCursor start( textdoc );
    QTextCursor end( textdoc );
    // We start from the end, to avoid the parag shifting problem
    QTextParag *p = textdoc->lastParag();
    while ( p )
    {
        KWTextParag * parag = static_cast<KWTextParag *>(p);
        if ( parag->style() && ( parag->style()->name().startsWith( "Contents Head" ) ||
            parag->style()->name() == "Contents Title" ) )
        {
            kdDebug() << "KWContents::createContents Deleting paragraph " << p << " " << p->paragId() << endl;
            // This paragraph is part of the TOC -> remove

            /* This method aims to provide an "undo" that restores the previous version of the TOC.
               Problem is, it screws up the parag style (due to removeSelectedText calling join),
               and the first parag of the body ends up with the Contents Title style.
            start.setParag( p );
            start.setIndex( 0 );
            textdoc->setSelectionStart( QTextDocument::Temp, &start );
            ASSERT( p->next() );
            end.setParag( p->next() );
            end.setIndex( 0 );
            textdoc->setSelectionEnd( QTextDocument::Temp, &end );
            KCommand * cmd = fs->removeSelectedTextCommand( cursor, QTextDocument::Temp );
            if ( macroCmd )
                macroCmd->addCommand( cmd );
            */

            // So instead, we do things by hand, and without undo....

            QTextParag *prev = p->prev();
            QTextParag *next = p->next();
            // Move cursor out
            if ( cursor->parag() == p )
                cursor->setParag( next ? next : prev );
            delete p;
            kdDebug() << "KWInsertTOCCommand::removeTOC " << p << " deleted" << endl;
            p = next;
            kdDebug() << "KWInsertTOCCommand::removeTOC prev=" << prev << " p=" << p << endl;
            // Fix parag chain
            if ( prev )
                prev->setNext( p );
            else
                textdoc->setFirstParag( p );
            p->setPrev( prev );

            // ### TODO Parag IDs !
        }
        p = p->prev();
    }
    textdoc->invalidate();
}

KWStyle * KWInsertTOCCommand::findOrCreateTOCStyle( KWTextFrameSet *fs, int depth )
{
    // Determine style name.
    // NOTE: don't add i18n here ! This is translated using
    // the i18n calls in stylenames.cc !
    QString name;
    if ( depth >= 0 )
        name = QString( "Contents Head %1" ).arg( depth+1 );
    else
        name = "Contents Title";
    KWStyle * style = fs->kWordDocument()->findStyle( name );
    if ( !style )
    {
        style = new KWStyle( name );
        style->format().setBold(true);
        style->format().setPointSize( depth==-1 ? 20 : depth==0 ? 16 : 12 );
        if ( depth == -1 )
        {
            style->paragLayout().topBorder = Border( Qt::black, Border::SOLID, 1 );
            style->paragLayout().bottomBorder = Border( Qt::black, Border::SOLID, 1 );
            // Old kword had only top and bottom. But borders are drawn differently now
            // (not the whole line anymore), so we need the 4 borders.
            style->paragLayout().leftBorder = Border( Qt::black, Border::SOLID, 1 );
            style->paragLayout().rightBorder = Border( Qt::black, Border::SOLID, 1 );
            style->paragLayout().alignment = Qt::AlignCenter;
        }
        else
        {
            KoTabulatorList tabList;
            KoTabulator tab;
            tab.ptPos = fs->getFrame( 0 )->width() - 10; // not sure why that much is necessary....
            tab.type = T_RIGHT;
            tabList.append( tab );
            style->paragLayout().setTabList( tabList );
        }
        fs->kWordDocument()->addStyleTemplate( style );             // register the new style
        fs->kWordDocument()->updateAllStyleLists();                 // show it in the UI
    }
    return style;
}

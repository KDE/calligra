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
#include "kword_doc.h"
#include "parag.h"
#include "kword_frame.h"
#include "format.h"
#include "paraglayout.h"

/******************************************************************
 *
 * Class: KWContents
 *
 ******************************************************************/

/*================================================================*/
KWContents::KWContents( KWordDocument *doc_ )
    : doc( doc_ ), end( 0 )
{
}

/*================================================================*/
void KWContents::createContents()
{
    int dec = 0;
    KWTextFrameSet *fs = (KWTextFrameSet*)doc->getFrameSet( 0 );

    if ( end && !parags.isEmpty() ) {
        dec = end->getStartPage();
        KWParag *p = fs->getFirstParag();
        while ( p ) {
            if ( parags.contains( p->getParagName() ) ) {
                KWParag *prev = p->getPrev();
                KWParag *next = p->getNext();
                if ( prev )
                    prev->setNext( next );
                if ( next )
                    next->setPrev( prev );
                delete p;
                if ( prev && !prev->getPrev() )
                    fs->setFirstParag( prev );
                if ( next && !next->getPrev() )
                    fs->setFirstParag( next );
                p = next;
            } else
                p = p->getNext();
        }
    }

    parags.clear();
    end = 0;

    QList<KoTabulator> tabList;
    KoTabulator *tab = new KoTabulator;
    tab->ptPos = fs->getFrame( 0 )->width() - 10;
    tab->mmPos = POINT_TO_MM( tab->ptPos );
    tab->inchPos = POINT_TO_INCH( tab->ptPos );
    tab->type = T_RIGHT;
    tabList.append( tab );

    KWParag *parag = new KWParag( fs, doc, 0, fs->getFirstParag(),
                                  doc->findParagLayout( "Contents Title" ) );
    parag->insertText( 0, i18n( "Table of Contents" ) );
    parag->setInfo( KWParag::PI_CONTENTS );
    parag->setFormat( 0, i18n( "Table of Contents" ).length(),
                      doc->findParagLayout( "Contents Title" )->getFormat() );
    parags.append( parag->getParagName() );

    KWParag *p = parag->getNext();
    KWParag *begin = parag;
    KWParag *body = parag->getNext();
    while ( p ) {
        if ( p->getParagLayout()->getName().contains( "Head" ) &&
             !p->getParagLayout()->getName().contains( "Contents" ) ) {

            int depth = p->getParagLayout()->getName().right( 1 ).toInt();
            KWParagLayout *pl
                = doc->findParagLayout( QString( "Contents Head %1" ).arg( depth ) );
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
            txt = QString( "%1" ).arg( pgNum );
            int i = parag->getTextLen();
            parag->insertText( i, txt );
            parag->setFormat( i, txt.length(), pl->getFormat() );
            begin = parag;
            parags.append( parag->getParagName() );
        }
        p = p->getNext();
    }

    if ( parag->getNext() )
        parag->getNext()->setHardBreak( TRUE );
    end = parag;
}

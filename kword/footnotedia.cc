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

#include "kword_doc.h"
#include "kword_page.h"
#include "footnotedia.h"
#include "footnotedia.moc"
#include "footnote.h"

#include <qlist.h>
#include <qwidget.h>

#include <klocale.h>

/******************************************************************/
/* Class: KWFootNoteDia                                           */
/******************************************************************/

/*================================================================*/
KWFootNoteDia::KWFootNoteDia( QWidget *parent, const char *name, KWordDocument *_doc, KWPage *_page, int _start )
    : QTabDialog( parent, name, true ), start( _start )
{
    doc = _doc;
    page = _page;

    setupTab1();

    setCancelButton( i18n( "Cancel" ) );
    setOkButton( i18n( "OK" ) );

    resize( 300, 250 );
}

/*================================================================*/
void KWFootNoteDia::setupTab1()
{
    tab1 = new QWidget( this );

    addTab( tab1, i18n( "Configure Footnote/Endnote" ) );

    QLabel *l = new QLabel( i18n( "Currently there is nothing to configure for\n"
                                  "footnotes/endnotes. Click ok to insert one!" ), tab1 );
    l->resize( l->sizeHint() );
    l->move( 5, 5 );

    connect( this, SIGNAL( applyButtonPressed() ), this, SLOT( insertFootNote() ) );

    resize(minimumSize());
}

/*================================================================*/
void KWFootNoteDia::insertFootNote()
{
    KWFootNote::KWFootNoteInternal *fi = new KWFootNote::KWFootNoteInternal;
    fi->from = start;
    fi->to = -1;
    fi->space = "-";

    QList<KWFootNote::KWFootNoteInternal> *lfi = new QList<KWFootNote::KWFootNoteInternal>();
    lfi->setAutoDelete( false );
    lfi->append( fi );

    KWFootNote *fn = new KWFootNote( doc, lfi );
    fn->setBefore( "[ " );
    fn->setAfter( " ]" );

    page->insertFootNote( fn );
}

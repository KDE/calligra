// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2002 Toshitaka Fujioka <fujioka@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KPrPresDurationDia.h"
#include "KPrCanvas.h"
#include "KPrPage.h"
#include "KPrDocument.h"
#include <qlabel.h>
#include <qheader.h>
#include <qvbox.h>
#include <qlayout.h>
#include <klistview.h>
#include <kdebug.h>
#include <klocale.h>

KPrPresDurationDia::KPrPresDurationDia( QWidget *parent, const char *name,
                                      KPrDocument *_doc, QStringList _durationListString,
                                      const QString &_durationString )
    : KDialogBase( parent, name, false, "", KDialogBase::Close ),
      doc( _doc )
{
    m_durationListString = _durationListString;
    m_durationString = _durationString;

    QWidget *page = new QWidget( this );
    setMainWidget( page );
    QVBoxLayout *topLayout = new QVBoxLayout( page, 2 );

    setupSlideList( page );
    topLayout->addWidget( slides );
    slides->setSelected( slides->firstChild(), true );

    label = new QLabel( i18n( "Presentation duration: " ) + _durationString, page );
    label->setAlignment( AlignVCenter );
    topLayout->addWidget( label );

    setMinimumSize( 600, 400 );
    connect( this, SIGNAL( closeClicked() ), this, SLOT( slotCloseDialog() ) );
}


void KPrPresDurationDia::setupSlideList( QWidget *_page )
{
    slides = new KListView( _page );
    slides->addColumn( i18n( "No." ) );
    slides->addColumn( i18n( "Display Duration" ) );
    slides->addColumn( i18n( "Slide Title" ) );
    slides->header()->setMovingEnabled( false );
    slides->setAllColumnsShowFocus( true );
    slides->setRootIsDecorated( false );
    slides->setSorting( -1 );

    for ( int i = doc->getPageNums() - 1; i >= 0; --i ) {
        if ( doc->pageList().at( i )->isSlideSelected() ) {
            KListViewItem *item = new KListViewItem( slides );
            item->setPixmap( 0, KPBarIcon( "slide" ) );
            item->setText( 0, QString( "%1" ).arg( i + 1 ) );
            item->setText( 1, *m_durationListString.at( i ) );
            item->setText( 2, doc->pageList().at( i )->pageTitle() );
        }
    }
}

#include "KPrPresDurationDia.moc"

// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KPrGotoPage.h"

#include <QLabel>
#include <q3listbox.h>
#include <QLayout>
#include <QPushButton>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3ValueList>
#include <Q3PtrList>

#include <klocale.h>
#include <kdialog.h>

#include "KPrDocument.h"
#include "KPrPage.h"

KPrGotoPage::KPrGotoPage( const KPrDocument *doc,
                        const Q3ValueList<int> &slides, int start,
                        QWidget *parent, const char *name )
    : KDialog( parent),
      oldPage(start)
{

    setCaption( i18n("Goto Slide...") );
    setButtons( Ok|Cancel );

    QWidget *page = new QWidget( this );
    setMainWidget(page);
    Q3VBoxLayout *ml = new Q3VBoxLayout( page, KDialog::marginHint(),
                                       KDialog::spacingHint());
    QLabel *label = new QLabel( i18n( "Go to slide:" ), page );
    ml->addWidget( label );
    spinbox = new Q3ListBox( page );
    connect( spinbox, SIGNAL(doubleClicked( Q3ListBoxItem* )),
             this, SLOT(accept()) );
    connect( spinbox, SIGNAL(returnPressed( Q3ListBoxItem* )),
             this, SLOT(accept()) );
    ml->addWidget( spinbox );

    Q3PtrList<KPrPage> pageList = doc->getPageList(); // because of const doc, we can't do doc->pageList()->at()
    Q3ValueList<int>::ConstIterator it = slides.begin();
    for ( ; it != slides.end(); ++it ) {
        QString t( pageList.at( (*it) - 1 )->pageTitle() );
        // cut ultra long titles...
        if(t.length() > 30) {
            t.truncate(30);
            t+="...";
        }
        spinbox->insertItem( QString( "%1 - %2" ).arg( *it ).arg( t ), -1 );
        if ( *it == start )
            spinbox->setCurrentItem( spinbox->count()-1 );
    }

    if ( parent )
        parent->setCursor( Qt::ForbiddenCursor );
}

int KPrGotoPage::gotoPage( const KPrDocument *doc,
                          const Q3ValueList<int> &slides, int start,
                          QWidget *parent)
{
    KPrGotoPage dia( doc, slides, start,parent, 0L );
    dia.exec();
    dia.resetCursor();
    return dia.page();
}

int KPrGotoPage::page() const {
    if(result()==QDialog::Accepted)
        return spinbox->currentText().left( spinbox->currentText().find( "-" ) - 1 ).toInt();
    return oldPage;
}

void KPrGotoPage::resetCursor() {
    if ( parentWidget() )
        parentWidget()->setCursor( Qt::BlankCursor );
}

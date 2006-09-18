/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
		 2006 Martin Pfeiffer <hubipete@gmx.net>

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
   Boston, MA 02110-1301, USA.
*/

#include "KFormulaConfigDialog.h"
#include "KFormulaPartView.h"

#include <kiconloader.h>
#include <kinstance.h>
#include <klocale.h>
#include <kicon.h>

#include <QLabel>

KFormulaConfigDialog::KFormulaConfigDialog( KFormulaPartView* parent )
                     : KPageDialog( parent )
{
    setFaceType( List );
    setCaption( i18n( "Configure KFormula") );
    setButtons( Ok | Apply | Cancel | Default );
    setDefaultButton( Ok );

    QLabel* label = new QLabel( "Test Page" );
    KPageWidgetItem* page = new KPageWidgetItem( label, i18n( "Test Page" ) );
    page->setHeader(  i18n( "Formula Settings" ) );
    page->setIcon( KIcon( "kformula" ) );
/*    _page = new KFormula::ConfigurePage( parent->document()->getDocument(), this,
                                         KFormulaFactory::global()->config(),
                                         page1 );*/
    addPage(page);
    connect( this, SIGNAL( okClicked() ), this, SLOT( slotApply() ) );
}

void KFormulaConfigDialog::slotApply()
{
//    _page->apply();
}

void KFormulaConfigDialog::slotDefault()
{
/*    if ( currentPage() == p1 )
        _page->slotDefault();*/
}

#include "KFormulaConfigDialog.moc"

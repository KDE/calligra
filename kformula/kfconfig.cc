/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

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

#include <kdebug.h>
#include <kiconloader.h>
#include <kinstance.h>
#include <klocale.h>

#include "kfconfig.h"
#include "kformula_factory.h"
#include "kformula_doc.h"
#include "kformula_view.h"


KFConfig::KFConfig( KFormulaPartView* parent )
    : KPageDialog( parent )
{
    setFaceType( List );
    setCaption( i18n( "Configure KFormula") );
    setButtons( Ok | Apply | Cancel | Default );
    setDefaultButton( Ok );
    //kDebug( 40000 ) << "KFConfig::KFConfig" << endl;
    KVBox* page1 = new KVBox();
    p1 = addPage( page1, i18n( "Formula" ) );
    p1->setHeader(  i18n( "Formula Settings" ) );
    p1->setIcon( BarIcon( "kformula", K3Icon::SizeMedium ) );
    _page = new KFormula::ConfigurePage( parent->document()->getDocument(), this,
                                         KFormulaFactory::global()->config(),
                                         page1 );
//     QVBox* page2 = addVBoxPage( i18n( "Math Fonts" ), i18n( "Math Fonts Settings" ),
//                                BarIcon( "kformula", K3Icon::SizeMedium ) );
//     _mathFontPage = new KFormula::MathFontsConfigurePage( parent->document()->getDocument(), this,
//                                          KFormulaFactory::global()->config(),
//                                          page2 );

    connect( this, SIGNAL( okClicked() ), this, SLOT( slotApply() ) );
}

void KFConfig::slotApply()
{
//     _mathFontPage->apply();

    // The "normal" page triggers the recalc and must be called last
    // for this reason.
    _page->apply();
}

void KFConfig::slotDefault()
{
    if ( currentPage() == p1 )
        _page->slotDefault();
}

#include "kfconfig.moc"

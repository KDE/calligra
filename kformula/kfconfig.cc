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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
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
    : KDialogBase( KDialogBase::IconList, i18n( "Configure KFormula" ),
                   KDialogBase::Ok | KDialogBase::Apply | KDialogBase::Cancel | KDialogBase::Default,
                   KDialogBase::Ok )
{
    //kdDebug( 40000 ) << "KFConfig::KFConfig" << endl;
    QVBox* page = addVBoxPage( i18n( "Formula" ), i18n( "Formula settings" ),
                               BarIcon( "settings", KIcon::SizeMedium ) );
    _page = new KFormula::ConfigurePage( parent->document()->getDocument(), this,
                                         KFormulaFactory::global()->config(),
                                         page );

    connect( this, SIGNAL( okClicked() ), this, SLOT( slotApply() ) );
}

void KFConfig::slotApply()
{
    _page->apply();
}

void KFConfig::slotDefault()
{
    switch(activePageIndex())
    {
    case 0:
        _page->slotDefault();
        break;
    default:
        break;
    }
}

#include "kfconfig.moc"

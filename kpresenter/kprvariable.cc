/* This file is part of the KDE project
   Copyright (C) 2001 Laurent MONTEL <lmontel@mandrakesoft.com>

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

#include "kprvariable.h"
#include <koVariable.h>
#include <koUtils.h>
#include "kpresenter_doc.h"

#include <klocale.h>
#include <kdebug.h>


KPrVariableCollection::KPrVariableCollection()
    : KoVariableCollection()
{
}

KoVariable *KPrVariableCollection::createVariable( int type, int subtype, KoVariableFormatCollection * coll, KoVariableFormat *varFormat,KoTextDocument *textdoc, KoDocument * doc )
{
    KPresenterDoc*m_doc=static_cast<KPresenterDoc*>(doc);
    KoVariable *var=0L;
    if(type ==VT_PGNUM)
    {
        var = new KPrPgNumVariable( textdoc,subtype, coll->format( "NUMBER" ),this,m_doc  );
    }
    else
        var = KoVariableCollection::createVariable( type, subtype,  coll,varFormat, textdoc,doc);
    return var;
}

/******************************************************************/
/* Class: KPrPgNumVariable                                         */
/******************************************************************/
KPrPgNumVariable::KPrPgNumVariable( KoTextDocument *textdoc, int subtype, KoVariableFormat *varFormat ,KoVariableCollection *_varColl, KPresenterDoc *doc  )
    : KoPgNumVariable( textdoc, subtype, varFormat ,_varColl ),m_doc(doc)
{
}

void KPrPgNumVariable::recalc()
{
    if ( m_subtype == VST_PGNUM_CURRENT )
    {
        //see KPTextObject::recalcPageNum( KPresenterDoc *doc )
    }
    else
    {
        m_pgNum = m_doc->getPageNums()+m_varColl->variableSetting()->startingPage()-1;
    }
    resize();
}


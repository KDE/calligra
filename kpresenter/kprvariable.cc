// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
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
#include <kovariable.h>
#include "kpresenter_doc.h"
#include "kprcommand.h"
#include <klocale.h>
#include <kdebug.h>


KPrVariableCollection::KPrVariableCollection(KoVariableSettings *_setting, KoVariableFormatCollection* coll)
    : KoVariableCollection(_setting, coll)
{
}

KoVariable *KPrVariableCollection::createVariable( int type, short int subtype, KoVariableFormatCollection * coll,
                                                   KoVariableFormat *varFormat,KoTextDocument *textdoc,
                                                   KoDocument * doc,  int _correct, bool _forceDefaultFormat, bool /*loadFootNote*/ )
{
    KPresenterDoc*m_doc=static_cast<KPresenterDoc*>(doc);
    KoVariable * var = 0L;
    switch(type) {
    case VT_PGNUM:
    {
        kdDebug(33001)<<" subtype == KoPageVariable::VST_CURRENT_SECTION :"<<(subtype == KPrPgNumVariable::VST_CURRENT_SECTION)<<endl;
        kdDebug(33001)<<" varFormat :"<<varFormat<<endl;
        if ( !varFormat )
            varFormat = (subtype == KPrPgNumVariable::VST_CURRENT_SECTION) ? coll->format("STRING") : coll->format("NUMBER");
        var = new KPrPgNumVariable( textdoc,subtype, varFormat,this,m_doc  );
        break;
    }
    default:
        return KoVariableCollection::createVariable( type, subtype, coll, varFormat, textdoc,
                                                     doc, _correct, _forceDefaultFormat);
    }
    return var;
}


KPrPgNumVariable::KPrPgNumVariable( KoTextDocument *textdoc, short int subtype, KoVariableFormat *varFormat,
                                    KoVariableCollection *_varColl, KPresenterDoc *doc  )
    : KoPageVariable( textdoc, subtype, varFormat ,_varColl ),m_doc(doc)
{
}

void KPrPgNumVariable::recalc()
{
    if ( m_subtype == VST_PGNUM_TOTAL )
    {
        m_varValue = QVariant( (int)(m_doc->getPageNums()+m_varColl->variableSetting()->startingPage()-1));
        resize();
    }
    // But we don't want to keep a width of -1 ...
    if ( width == -1 )
        width = 0;
}

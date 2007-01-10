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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KPrVariableCollection.h"
#include <KoVariable.h>
#include "KPrDocument.h"
#include "KPrCommand.h"
#include <klocale.h>
#include <kdebug.h>
#include <KoXmlNS.h>
#include <KoDom.h>
#include "KPrTextObject.h"
#include "KPrPage.h"

KPrVariableCollection::KPrVariableCollection(KoVariableSettings *_setting, KoVariableFormatCollection* coll)
    : KoVariableCollection(_setting, coll)
{
}

KoVariable* KPrVariableCollection::loadOasisField( KoTextDocument* textdoc, const QDomElement& tag, KoOasisContext& context )
{
    const QString localName( tag.localName() );
    const bool isTextNS = tag.namespaceURI() == KoXmlNS::text;
    if ( isTextNS )
    {
        if ( localName == "object-count"  ||
                  localName == "picture-count"  ||
                  localName == "paragraph-count"  ||
                  localName == "word-count"  ||
                  localName == "character-count" ||
                  localName == "sentence-count" ||
                  localName == "line-count" ||
                  localName == "frame-count"  ||
                  localName == "non-whitespace-character-count" ||
                  localName == "syllable-count" )
        {
            QString key = "NUMBER";
            int type = VT_STATISTIC;
            return loadOasisFieldCreateVariable( textdoc, tag, context, key, type );
        }
        else
            return KoVariableCollection::loadOasisField( textdoc, tag, context );
    }
    else
        return KoVariableCollection::loadOasisField( textdoc, tag, context );
}

KoVariable *KPrVariableCollection::createVariable( int type, short int subtype, KoVariableFormatCollection * coll,
                                                   KoVariableFormat *varFormat,KoTextDocument *textdoc,
                                                   KoDocument * doc,  int _correct, bool _forceDefaultFormat, bool /*loadFootNote*/ )
{
    KPrDocument*m_doc=static_cast<KPrDocument*>(doc);
    KoVariable * var = 0L;
    switch(type) {
    case VT_PGNUM:
    {
        kDebug(33001)<<" subtype == KoPageVariable::VST_CURRENT_SECTION :"<<(subtype == KPrPgNumVariable::VST_CURRENT_SECTION)<<endl;
        kDebug(33001)<<" varFormat :"<<varFormat<<endl;
        if ( !varFormat )
            varFormat = (subtype == KPrPgNumVariable::VST_CURRENT_SECTION) ? coll->format("STRING") : coll->format("NUMBER");
        var = new KPrPgNumVariable( textdoc,subtype, varFormat,this,m_doc  );
        break;
    }
    case VT_STATISTIC:
        if ( !varFormat )
            varFormat = coll->format("NUMBER");
        var = new KPrStatisticVariable( textdoc, subtype, varFormat, this, m_doc );
        break;
    default:
        return KoVariableCollection::createVariable( type, subtype, coll, varFormat, textdoc,
                                                     doc, _correct, _forceDefaultFormat);
    }
    return var;
}


KPrPgNumVariable::KPrPgNumVariable( KoTextDocument *textdoc, short int subtype, KoVariableFormat *varFormat,
                                    KoVariableCollection *_varColl, KPrDocument *doc  )
    : KoPageVariable( textdoc, subtype, varFormat ,_varColl ),m_doc(doc)
{
}

void KPrPgNumVariable::recalc()
{
    if ( m_subtype == VST_PGNUM_TOTAL )
    {
        m_varValue = QVariant( (int)(m_doc->getPageNums()+m_varColl->variableSetting()->startingPageNumber()-1));
        resize();
    }
    // But we don't want to keep a width of -1 ...
    if ( width == -1 )
        width = 0;
}

KPrStatisticVariable::KPrStatisticVariable( KoTextDocument *textdoc,  int subtype, KoVariableFormat *varFormat,KoVariableCollection *_varColl, KPrDocument *doc )
    : KoStatisticVariable( textdoc, subtype, varFormat, _varColl ),
      m_doc(doc)
{
}

void KPrStatisticVariable::recalc()
{
    int nb = 0;
    ulong charsWithSpace = 0L;
    ulong charsWithoutSpace = 0L;
    ulong words = 0L;
    ulong sentences = 0L;
    ulong lines = 0L;
    ulong syllables = 0L;
    bool frameInfo = ( m_subtype == VST_STATISTIC_NB_WORD ||
                       m_subtype == VST_STATISTIC_NB_SENTENCE ||
                       m_subtype == VST_STATISTIC_NB_LINES ||
                       m_subtype == VST_STATISTIC_NB_CHARACTERE);
    KPrPage *page = m_doc->activePage();
    if( !page) //When we load activePage is null
	    return;
    Q3PtrListIterator<KPrObject> objIt( page->objectList() );

    for ( objIt.toFirst(); objIt.current(); ++objIt )
    {
        KPrObject *obj = objIt.current();
        if ( m_subtype == VST_STATISTIC_NB_FRAME )
            ++nb;
        else if( m_subtype == VST_STATISTIC_NB_PICTURE && obj->getType() == OT_PICTURE)
        {
            ++nb;
        }
        else if( m_subtype == VST_STATISTIC_NB_EMBEDDED && obj->getType() == OT_PART )
        {
            ++nb;
        }
        if ( frameInfo )
        {
            KPrTextObject *textObj = dynamic_cast<KPrTextObject *>( obj );
            if ( textObj )
                textObj->textObject()->statistics( 0L, charsWithSpace, charsWithoutSpace, words, sentences, syllables, lines, false );
        }
    }
    if ( frameInfo )
    {
        if( m_subtype == VST_STATISTIC_NB_WORD )
        {
            nb = words;
        }
        else if( m_subtype == VST_STATISTIC_NB_SENTENCE )
        {
            nb = sentences;
        }
        else if( m_subtype == VST_STATISTIC_NB_LINES )
        {
                nb = lines;
        }
        else if ( m_subtype == VST_STATISTIC_NB_CHARACTERE )
        {
            nb = charsWithSpace;
        }
        else if ( m_subtype ==VST_STATISTIC_NB_NON_WHITESPACE_CHARACTERE )
        {
            nb = charsWithoutSpace;
        }
        else if ( m_subtype ==VST_STATISTIC_NB_SYLLABLE )
        {
            nb = syllables;
        }
        else
            nb = 0;
    }

    m_varValue = QVariant(nb);
    resize();
    if ( width == -1 )
        width = 0;
}

QString KPrStatisticVariable::text(bool realValue)
{
    if (m_varColl->variableSetting()->displayFieldCode()&& !realValue)
        return fieldCode();
    else
        return m_varFormat->convert( m_varValue );
}



/* This file is part of the KDE project
   Copyright (C) 2002, Laurent MONTEL <lmontel@mandrakesoft.com>

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

#include "kprautoformat.h"
#include "kprvariable.h"
#include <koVariable.h>
#include "kpresenter_doc.h"
#include <kdebug.h>
#include <kotextformat.h>
#include <kotextparag.h>
#include <kotextobject.h>
#include <klocale.h>

KPrAutoFormat::KPrAutoFormat(KPresenterDoc *doc)
    : KoAutoFormat(doc)
{
    m_doc=doc;
}

void KPrAutoFormat::doAutoDetectUrl( QTextCursor *textEditCursor, KoTextParag *parag,int index, const QString & word, KoTextObject *txtObj )
{
    if (word.find("http://")!=-1 || word.find("mailto:")!=-1
        || word.find("ftp://")!=-1 || word.find("file:")!=-1)
    {
        unsigned int length = word.length();
        int start = index - length;
        QTextCursor cursor( parag->document() );
        KoTextDocument * textdoc = parag->textDocument();
        cursor.setParag( parag );
        cursor.setIndex( start );
        textdoc->setSelectionStart( KoTextObject::HighlightSelection, &cursor );
        cursor.setIndex( start + length );
        textdoc->setSelectionEnd( KoTextObject::HighlightSelection, &cursor );
        KoVariable *var=new KoLinkVariable( textdoc, word, word ,m_doc->variableFormatCollection()->format( "STRING" ), m_doc->getVariableCollection() );

        CustomItemsMap customItemsMap;
        customItemsMap.insert( 0, var );
        KoTextFormat * lastFormat = static_cast<KoTextFormat *>(parag->at( parag->length() - 1 )->format());
        txtObj->insert( textEditCursor, lastFormat, KoTextObject::customItemChar(), false, true, i18n("Insert Variable"), customItemsMap,KoTextObject::HighlightSelection );
        txtObj->emitHideCursor();
        textEditCursor->gotoRight();
        txtObj->emitShowCursor();
    }
}

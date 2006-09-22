// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2001 Laurent Montel <lmontel@mandrakesoft.com>

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

#include "KPrTextDocument.h"

#include "KPrDocument.h"
#include "KPrTextObject.h"
#include <KoTextParag.h>
#include <kdebug.h>

#include "KPrVariableCollection.h"

#include <KoOasisContext.h>
#include <KoDom.h>
#include <KoXmlNS.h>

KPrTextDocument::KPrTextDocument( KPrTextObject * textobj, KoTextFormatCollection *fc, KoTextFormatter *formatter )
    : KoTextDocument( textobj->kPresenterDocument()->zoomHandler(), fc, formatter, true ), m_textobj( textobj )
{
    //kdDebug(33001) << "KPrTextDocument constructed " << this << "  KPrTextObject:" << textobj << endl;
}

KPrTextDocument::~KPrTextDocument()
{
}


bool KPrTextDocument::loadSpanTag( const QDomElement& tag, KoOasisContext& context,
                                  KoTextParag* parag, uint pos,
                                  QString& textData, KoTextCustomItem* & customItem )
{
    const QString localName( tag.localName() );
    const bool isTextNS = tag.namespaceURI() == KoXmlNS::text;
    kdDebug( 32500 ) << "KPrTextDocument::loadSpanTag: " << localName << endl;

    if ( isTextNS )
    {
        if ( localName == "a" )
        {
            QString href( tag.attributeNS( KoXmlNS::xlink, "href", QString::null ) );
            if ( href.startsWith( "#" ) )
            {
                context.styleStack().save();
                // We have a reference to a bookmark (### TODO)
                // As we do not support it now, treat it as a <span> without formatting
                parag->loadOasisSpan( tag, context, pos ); // recurse
                context.styleStack().restore();
            }
            else
            {
                // The text is contained in a <span> inside the <a> element. In theory
                // we could have multiple spans there, but OO ensures that there is always only one,
                // splitting the hyperlink if necessary (at format changes).
                // Note that we ignore the formatting of the span.
                QDomElement spanElem = KoDom::namedItemNS( tag, KoXmlNS::text, "span" );
                QString text;
                if ( spanElem.isNull() )
                    text = tag.text();
                if ( spanElem.isNull() )
                    text = tag.text();
                else {
                    // The save/restore of the stack is done by the caller (KoTextParag::loadOasisSpan)
                    // This allows to use the span's format for the variable.
                    //kdDebug(32500) << "filling stack with " << spanElem.attributeNS( KoXmlNS::text, "style-name", QString::null ) << endl;
                    context.fillStyleStack( spanElem, KoXmlNS::text, "style-name", "text" );
                    text = spanElem.text();
                }
                textData = KoTextObject::customItemChar(); // hyperlink placeholder
                // unused tag.attributeNS( KoXmlNS::office, "name", QString::null )
                KoVariableCollection& coll = context.variableCollection();
                customItem = new KoLinkVariable( this, text, href,
                        coll.formatCollection()->format( "STRING" ),
                        &coll );
            }
            return true;
        }
    }
    else // non "text:" tags
    {
        kdDebug()<<"Extension found tagName : "<< localName <<endl;
    }
    return false;
}


#include "KPrTextDocument.moc"

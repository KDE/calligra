/* This file is part of the KDE project
   Copyright (C) 2005 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "KoOasisLoadingContext.h"
#include <koOasisStore.h>
#include <koOasisStyles.h>
#include <koxmlns.h>
#include <kdebug.h>
#include <kodom.h>

KoOasisLoadingContext::KoOasisLoadingContext( KoDocument* doc,
                                              KoOasisStyles& styles, KoStore* store )
    : m_doc( doc ), m_store( store ), m_styles( styles ),
      m_metaXmlParsed( false )
{
    // Ideally this should be done by KoDocument and passed as argument here...
    KoOasisStore oasisStore( store );
    QString dummy;
    (void)oasisStore.loadAndParse( "tar:/META-INF/manifest.xml", m_manifestDoc, dummy );
}


KoOasisLoadingContext::~KoOasisLoadingContext()
{

}

void KoOasisLoadingContext::fillStyleStack( const QDomElement& object, const char* nsURI, const char* attrName )
{
    // find all styles associated with an object and push them on the stack
    // OoImpressImport has more tests here, but I don't think they're relevant to OoWriterImport
    // ### TODO check the above comment, now that things are in kotext...
    if ( object.hasAttributeNS( nsURI, attrName ) ) {
        const QString styleName = object.attributeNS( nsURI, attrName, QString::null );
        const QDomElement* style = m_styles.styles()[styleName];
        if ( style )
            addStyles( style );
        else
            kdWarning(32500) << "fillStyleStack: no style named " << styleName << " found." << endl;
    }
}

void KoOasisLoadingContext::addStyles( const QDomElement* style )
{
    Q_ASSERT( style );
    if ( !style ) return;
    // this recursive function is necessary as parent styles can have parents themselves
    if ( style->hasAttributeNS( KoXmlNS::style, "parent-style-name" ) ) {
        const QString parentStyleName = style->attributeNS( KoXmlNS::style, "parent-style-name", QString::null );
        QDomElement* parentStyle = m_styles.styles()[ parentStyleName ];
        if ( parentStyle )
            addStyles( parentStyle );
        else
            kdWarning(32500) << "Parent style not found: " << parentStyleName << endl;
    }
    else {
        QString family = style->attributeNS( KoXmlNS::style, "family", QString::null );
        if ( !family.isEmpty() ) {
            QDomElement* def = m_styles.defaultStyle( family );
            if ( def ) { // on top of all, the default style for this family
                //kdDebug(32500) << "pushing default style " << style->attributeNS( KoXmlNS::style, "name", QString::null ) << endl;
                m_styleStack.push( *def );
            }
        }
    }

    //kdDebug(32500) << "pushing style " << style->attributeNS( KoXmlNS::style, "name", QString::null ) << endl;
    m_styleStack.push( *style );
}

QString KoOasisLoadingContext::generator() const
{
    parseMeta();
    return m_generator;
}

void KoOasisLoadingContext::parseMeta() const
{
    if ( !m_metaXmlParsed && m_store )
    {
        QDomDocument metaDoc;
        KoOasisStore oasisStore( m_store );
        QString errorMsg;
        if ( oasisStore.loadAndParse( "meta.xml", metaDoc, errorMsg ) ) {
            QDomNode meta   = KoDom::namedItemNS( metaDoc, KoXmlNS::office, "document-meta" );
            QDomNode office = KoDom::namedItemNS( meta, KoXmlNS::office, "meta" );
            QDomElement generator = KoDom::namedItemNS( office, KoXmlNS::meta, "generator" );
            if ( !generator.isNull() )
                m_generator = generator.text();
        }
        m_metaXmlParsed = true;
    }
}

/* This file is part of the KDE project
   Copyright (C) 2004 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "koOasisStyles.h"
#include <koxmlwriter.h>
#include <kdebug.h>
#include <qdom.h>
#include "kodom.h"
#include "koxmlns.h"
#include "koGenStyles.h"
#include <qbuffer.h>
#include <kglobal.h>
#include <klocale.h>

KoOasisStyles::KoOasisStyles()
{

}

KoOasisStyles::~KoOasisStyles()
{

}

void KoOasisStyles::createStyleMap( const QDomDocument& doc )
{
    const QDomElement docElement  = doc.documentElement();
    // We used to have the office:version check here, but better let the apps do that
    QDomElement fontStyles = KoDom::namedItemNS( docElement, KoXmlNS::office, "font-decls" );

    if ( !fontStyles.isNull() ) {
        //kdDebug(30003) << "Starting reading in font-decl..." << endl;
        insertStyles( fontStyles );
    }// else
    //   kdDebug(30003) << "No items found" << endl;

    //kdDebug(30003) << "Starting reading in office:automatic-styles" << endl;

    QDomElement autoStyles = KoDom::namedItemNS( docElement, KoXmlNS::office, "automatic-styles" );
    if ( !autoStyles.isNull() ) {
        insertStyles( autoStyles );
    }// else
    //    kdDebug(30003) << "No items found" << endl;


    //kdDebug(30003) << "Reading in master styles" << endl;

    QDomNode masterStyles = KoDom::namedItemNS( docElement, KoXmlNS::office, "master-styles" );

    if ( !masterStyles.isNull() ) {
        QDomElement master;
        forEachElement( master, masterStyles )
        {
            if ( master.localName() == "master-page" &&
                 master.namespaceURI() == KoXmlNS::style ) {
                const QString name = master.attributeNS( KoXmlNS::style, "name", QString::null );
                kdDebug(30003) << "Master style: '" << name << "' loaded " << endl;
                m_masterPages.insert( name, new QDomElement( master ) );
            } else
                // OASIS docu mentions style:handout-master and draw:layer-set here
                kdWarning(30003) << "Unknown tag " << master.tagName() << " in office:master-styles" << endl;
        }
    }


    kdDebug(30003) << "Starting reading in office:styles" << endl;

    const QDomElement officeStyle = KoDom::namedItemNS( docElement, KoXmlNS::office, "styles" );

    if ( !officeStyle.isNull() ) {
        m_officeStyle = officeStyle;
        insertOfficeStyles( m_officeStyle );

    }

    //kdDebug(30003) << "Styles read in." << endl;
}

QValueVector<QDomElement> KoOasisStyles::userStyles() const
{
    QValueVector<QDomElement> vec;
    // Collect user styles
    unsigned int i = 0;
    QDomElement e;
    forEachElement( e, m_officeStyle )
    {
        if ( e.localName() == "style" &&
             e.namespaceURI() == KoXmlNS::style )
        {
            vec.resize( i+1 );
            vec[i++] = e;
        }
    }
    return vec;
}

void KoOasisStyles::insertOfficeStyles( const QDomElement& styles )
{
    QDomElement e;
    forEachElement( e, styles )
    {
        const QString localName = e.localName();
        const QString ns = e.namespaceURI();
        if ( ( ns == KoXmlNS::svg && (
                   localName == "linearGradient"
                   || localName == "radialGradient" ) )
             || ( ns == KoXmlNS::draw && (
                      localName == "gradient"
                      || localName == "hatch"
                      || localName == "fill-image"
                      || localName == "marker"
                      || localName == "stroke-dash"
                      || localName == "opacity" ) )
             )
        {
            const QString name = e.attributeNS( KoXmlNS::draw, "name", QString::null );
            Q_ASSERT( !name.isEmpty() );
            QDomElement* ep = new QDomElement( e );
            m_drawStyles.insert( name, ep );
        }
        else
            insertStyle( e );
    }
}


void KoOasisStyles::insertStyles( const QDomElement& styles )
{
    //kdDebug(30003) << "Inserting styles from " << styles.tagName() << endl;
    QDomElement e;
    forEachElement( e, styles )
        insertStyle( e );
}

void KoOasisStyles::insertStyle( const QDomElement& e )
{
    const QString localName = e.localName();
    const QString ns = e.namespaceURI();

    const QString name = e.attributeNS( KoXmlNS::style, "name", QString::null );
    if ( ns == KoXmlNS::style && (
                localName == "style"
             || localName == "page-master" // OO-1.1 compatibility (probably not useful)
             || localName == "page-layout"
             || localName == "font-decl"
             || localName == "presentation-page-layout" ) )
    {
        QDomElement* ep = new QDomElement( e );
        m_styles.insert( name, ep );
        //kdDebug(30003) << "Style: '" << name << "' loaded " << endl;
    } else if ( localName == "default-style" && ns == KoXmlNS::style ) {
        m_defaultStyle = e;
    } else if ( localName == "list-style" && ns == KoXmlNS::text ) {
        QDomElement* ep = new QDomElement( e );
        m_listStyles.insert( name, ep );
        //kdDebug(30003) << "List style: '" << name << "' loaded " << endl;
    } else if ( ns == KoXmlNS::number && (
                   localName == "number-style"
                || localName == "currency-style"
                || localName == "percentage-style"
                || localName == "boolean-style"
                || localName == "text-style"
                || localName == "date-style"
                || localName == "time-style" ) ) {
        importDataStyle( e );
    }
    // The rest (text:*-configuration and text:outline-style) is to be done by the apps.
}

// OO spec 2.5.4. p68. Conversion to Qt format: see qdate.html
// OpenCalcImport::loadFormat has similar code, but slower, intermixed with other stuff,
// lacking long-textual forms.
void KoOasisStyles::importDataStyle( const QDomElement& parent )
{
    QString format;
    int precision = 0;
    int leadingZ  = 1;
    bool thousandsSep = false;
    //todo negred
    bool negRed = false;
    bool ok = false;
    int i = 0;
    QDomElement e;
    forEachElement( e, parent )
    {
        if ( e.namespaceURI() != KoXmlNS::number )
            continue;
        QString localName = e.localName();
        const QString numberStyle = e.attributeNS( KoXmlNS::number, "style", QString::null );
        const bool shortForm = numberStyle == "short" || numberStyle.isEmpty();
        if ( localName == "day" ) {
            format += shortForm ? "d" : "dd";
        } else if ( localName == "day-of-week" ) {
            format += shortForm ? "ddd" : "dddd";
        } else if ( localName == "month" ) {
            // TODO the spec has a strange mention of number:format-source
            if ( e.attributeNS( KoXmlNS::number, "textual", QString::null ) == "true" ) {
                format += shortForm ? "MMM" : "MMMM";
            } else { // month number
                format += shortForm ? "M" : "MM";
            }
        } else if ( localName == "year" ) {
            format += shortForm ? "yy" : "yyyy";
        } else if ( localName == "era" ) {
            //todo I don't know what is it... (define into oo spec)
        } else if ( localName == "week-of-year" || localName == "quarter") {
            // ### not supported in Qt
        } else if ( localName == "hours" ) {
            format += shortForm ? "h" : "hh";
        } else if ( localName == "minutes" ) {
            format += shortForm ? "m" : "mm";
        } else if ( localName == "seconds" ) {
            format += shortForm ? "s" : "ss";
        } else if ( localName == "am-pm" ) {
            format += "ap";
        } else if ( localName == "text" ) { // litteral
            format += e.text();
        } else if ( localName == "currency-symbol" ) {
            format += e.text();
            //TODO
            // number:language="de" number:country="DE">€</number:currency-symbol>
        } else if ( localName == "number" ) {
            // TODO: number:grouping="true"
            if ( e.hasAttributeNS( KoXmlNS::number, "decimal-places" ) )
            {
                int d = e.attributeNS( KoXmlNS::number, "decimal-places", QString::null ).toInt( &ok );
                if ( ok )
                    precision = d;
            }
            if ( e.hasAttributeNS( KoXmlNS::number, "min-integer-digits" ) )
            {
                int d = e.attributeNS( KoXmlNS::number, "min-integer-digits", QString::null ).toInt( &ok );
                if ( ok )
                    leadingZ = d;
            }
            if ( thousandsSep && leadingZ <= 3 )
            {
                format += "#,";
                for ( i = leadingZ; i <= 3; ++i )
                    format += '#';
            }
            for ( i = 1; i <= leadingZ; ++i )
            {
                format +=  '0';
                if ( ( i % 3 == 0 ) && thousandsSep )
                    format =+ ',' ;
            }
            format += '.';
            for ( i = 0; i < precision; ++i )
                format += '0';
        }
        else if ( localName == "scientific-number" ) {
            int exp = 2;

            if ( e.hasAttributeNS( KoXmlNS::number, "decimal-places" ) )
            {
                int d = e.attributeNS( KoXmlNS::number, "decimal-places", QString::null ).toInt( &ok );
                if ( ok )
                    precision = d;
            }

            if ( e.hasAttributeNS( KoXmlNS::number, "min-integer-digits" ) )
            {
                int d = e.attributeNS( KoXmlNS::number, "min-integer-digits", QString::null ).toInt( &ok );
                if ( ok )
                    leadingZ = d;
            }

            if ( e.hasAttributeNS( KoXmlNS::number, "min-exponent-digits" ) )
            {
                int d = e.attributeNS( KoXmlNS::number, "min-exponent-digits", QString::null ).toInt( &ok );
                if ( ok )
                    exp = d;
                if ( exp <= 0 )
                    exp = 1;
            }

            if ( thousandsSep && leadingZ <= 3 )
            {
                format += "#,";
                for ( i = leadingZ; i <= 3; ++i )
                    format += '#';
            }

            for ( i = 1; i <= leadingZ; ++i )
            {
                format+='0';
                if ( ( i % 3 == 0 ) && thousandsSep )
                    format+=',';
            }

            format+='.';
            for ( i = 0; i < precision; ++i )
                format+='0';

            format+="E+";
            for ( i = 0; i < exp; ++i )
                format+='0';
        } else if ( localName == "fraction" ) {
                int integer = 0;
                int numerator = 1;
                int denominator = 1;

                if ( e.hasAttributeNS( KoXmlNS::number, "min-integer-digits" ) )
                {
                    int d = e.attributeNS( KoXmlNS::number, "min-integer-digits", QString::null ).toInt( &ok );
                    if ( ok )
                        integer = d;
                }
                if ( e.hasAttributeNS( KoXmlNS::number, "min-numerator-digits" ) )
                {
                    int d = e.attributeNS( KoXmlNS::number, "min-numerator-digits", QString::null ).toInt( &ok );
                    if ( ok )
                        numerator = d;
                }
                if ( e.hasAttributeNS( KoXmlNS::number, "min-denominator-digits" ) )
                {
                    int d = e.attributeNS( KoXmlNS::number, "min-denominator-digits", QString::null ).toInt( &ok );
                    if ( ok )
                        denominator = d;
                }

                for ( i = 0; i <= integer; ++i )
                    format+='#';

                format+=' ';

                for ( i = 0; i <= numerator; ++i )
                    format+='?';

                format+='/';

                for ( i = 0; i <= denominator; ++i )
                    format+='?';
            }
        // Not needed:
        //  <style:map style:condition="value()&gt;=0" style:apply-style-name="N106P0"/>
        // we handle painting negative numbers in red differently

    }

    const QString styleName = parent.attributeNS( KoXmlNS::style, "name", QString::null );
    kdDebug(30518) << "datetime style: " << styleName << " qt format=" << format << endl;
    m_dataFormats.insert( styleName, format );
}

#define addTextNumber( text, elementWriter ) { \
        if ( !text.isEmpty() ) \
        { \
            elementWriter.startElement( "number:text" ); \
            elementWriter.addTextNode( text ); \
            elementWriter.endElement(); \
            text=""; \
        } \
}

void KoOasisStyles::parseOasisTimeKlocale(KoXmlWriter &elementWriter, QString & format, QString & text )
{
    kdDebug()<<"parseOasisTimeKlocale(KoXmlWriter &elementWriter, QString & format, QString & text ) :"<<format<<endl;
    do
    {
        if ( !saveOasisKlocaleTimeFormat( elementWriter, format, text ) )
        {
            text += format[0];
            format = format.remove( 0, 1 );
        }
    }
    while ( format.length() > 0 );
    addTextNumber( text, elementWriter );
}

bool KoOasisStyles::saveOasisKlocaleTimeFormat( KoXmlWriter &elementWriter, QString & format, QString & text )
{
    bool changed = false;
    if ( format.startsWith( "%H" ) ) //hh
    {
        //hour in 24h
        addTextNumber( text, elementWriter );

        elementWriter.startElement( "number:hours" );
        elementWriter.addAttribute( "number:style", "long" );
        elementWriter.endElement();
        format = format.remove( 0, 2 );
        changed = true;
    }
    else if ( format.startsWith( "%k" ) )//h
    {
        addTextNumber( text, elementWriter );

        elementWriter.startElement( "number:hours" );
        elementWriter.addAttribute( "number:style", "short" );
        elementWriter.endElement();
        format = format.remove( 0, 2 );
        changed = true;
    }
    else if ( format.startsWith( "%I" ) )// ?????
    {
        //todo hour in 12h
        changed = true;
    }
    else if ( format.startsWith( "%l" ) )
    {
        //todo hour in 12h with 1 digit
        changed = true;
    }
    else if ( format.startsWith( "%M" ) )// mm
    {
        addTextNumber( text, elementWriter );

        elementWriter.startElement( "number:minutes" );
        elementWriter.addAttribute( "number:style", "long" );
        elementWriter.endElement();
        format = format.remove( 0, 2 );
        changed = true;

    }
    else if ( format.startsWith( "%S" ) ) //ss
    {
        addTextNumber( text, elementWriter );

        elementWriter.startElement( "number:seconds" );
        elementWriter.addAttribute( "number:style", "long" );
        elementWriter.endElement();
        format = format.remove( 0, 2 );
        changed = true;
    }
    else if ( format.startsWith( "%p" ) )
    {
        //todo am or pm
        addTextNumber( text, elementWriter );

        elementWriter.startElement( "number:am-pm" );
        elementWriter.endElement();
        format = format.remove( 0, 2 );
        changed = true;
    }
    return changed;
}


bool KoOasisStyles::saveOasisTimeFormat( KoXmlWriter &elementWriter, QString & format, QString & text, bool &antislash )
{
    bool changed = false;
    //we can also add time to date.
    if ( antislash )
    {
        text+=format[0];
        format = format.remove( 0, 1 );
        antislash = false;
        changed = true;
    }
    else if ( format.startsWith( "hh" ) )
    {
        addTextNumber( text, elementWriter );

        elementWriter.startElement( "number:hours" );
        elementWriter.addAttribute( "number:style", "long" );
        elementWriter.endElement();
        format = format.remove( 0, 2 );
        changed = true;
    }
    else if ( format.startsWith( "h" ) )
    {
        addTextNumber( text, elementWriter );

        elementWriter.startElement( "number:hours" );
        elementWriter.addAttribute( "number:style", "short" );
        elementWriter.endElement();
        format = format.remove( 0, 1 );
        changed = true;
    }
    else if ( format.startsWith( "mm" ) )
    {
        addTextNumber( text, elementWriter );

        elementWriter.startElement( "number:minutes" );
        elementWriter.addAttribute( "number:style", "long" );
        elementWriter.endElement();
        format = format.remove( 0, 2 );
        changed = true;
    }
    else if ( format.startsWith( "m" ) )
    {
        addTextNumber( text, elementWriter );

        elementWriter.startElement( "number:minutes" );
        elementWriter.addAttribute( "number:style", "short" );
        elementWriter.endElement();
        format = format.remove( 0, 1 );
        changed = true;
    }
    else if ( format.startsWith( "ss" ) )
    {
        addTextNumber( text, elementWriter );

        elementWriter.startElement( "number:seconds" );
        elementWriter.addAttribute( "number:style", "long" );
        elementWriter.endElement();
        format = format.remove( 0, 2 );
        changed = true;
    }
    else if ( format.startsWith( "s" ) )
    {
        addTextNumber( text, elementWriter );

        elementWriter.startElement( "number:seconds" );
        elementWriter.addAttribute( "number:style", "short" );
        elementWriter.endElement();
        format = format.remove( 0, 1 );
        changed = true;
    }
    else if ( format.startsWith( "ap" ) )
    {
        addTextNumber( text, elementWriter );

        elementWriter.startElement( "number:am-pm" );
        elementWriter.endElement();
        format = format.remove( 0, 2 );
        changed = true;
    }
    return changed;
}

QString KoOasisStyles::saveOasisTimeStyle( KoGenStyles &mainStyles, const QString & _format, bool klocaleFormat )
{
    kdDebug()<<"QString KoOasisStyles::saveOasisTimeStyle( KoGenStyles &mainStyles, const QString & _format ) :"<<_format<<endl;
    QString format( _format );
    KoGenStyle currentStyle( KoGenStyle::STYLE_NUMERIC_TIME );
    QBuffer buffer;
    buffer.open( IO_WriteOnly );
    KoXmlWriter elementWriter( &buffer );  // TODO pass indentation level
    QString text;
    if ( klocaleFormat )
    {
        parseOasisTimeKlocale( elementWriter, format, text );
    }
    else
    {
        bool antislash = false;
        do
        {
            if ( !saveOasisTimeFormat( elementWriter, format, text, antislash ) )
            {
                QString elem( format[0] );
                format = format.remove( 0, 1 );
                if ( elem == "\\" )
                {
                     antislash = true;
                }
                else
                {
                    text += elem;
                    antislash = false;
                }
            }
        }
        while ( format.length() > 0 );
        addTextNumber( text, elementWriter );
    }
    QString elementContents = QString::fromUtf8( buffer.buffer(), buffer.buffer().size() );
    currentStyle.addChildElement( "number", elementContents );
    return mainStyles.lookup( currentStyle, "N" );
}

//convert klocale string to good format
void KoOasisStyles::parseOasisDateKlocale(KoXmlWriter &elementWriter, QString & format, QString & text )
{
    kdDebug()<<"KoOasisStyles::parseOasisDateKlocale(KoXmlWriter &elementWriter, QString & format, QString & text ) :"<<format<<endl;
    do
    {
        if ( format.startsWith( "%Y" ) )
        {
            addTextNumber( text, elementWriter );
            elementWriter.startElement( "number:year" );
            elementWriter.addAttribute( "number:style", "long" );
            elementWriter.endElement();
            format = format.remove( 0, 2 );
        }
        else if ( format.startsWith( "%y" ) )
        {

            addTextNumber( text, elementWriter );

            elementWriter.startElement( "number:year" );
            elementWriter.addAttribute( "number:style", "short" );
            elementWriter.endElement();
            format = format.remove( 0, 2 );
        }
        else if ( format.startsWith( "%n" ) )
        {
            addTextNumber( text, elementWriter );
            elementWriter.startElement( "number:month" );
            elementWriter.addAttribute( "number:style", "short" );
            elementWriter.addAttribute( "number:textual", "false");
            elementWriter.endElement();
            format = format.remove( 0, 2 );
        }
        else if ( format.startsWith( "%m" ) )
        {
            addTextNumber( text, elementWriter );
            elementWriter.startElement( "number:month" );
            elementWriter.addAttribute( "number:style", "long" );
            elementWriter.addAttribute( "number:textual", "false"); //not necessary remove it
            elementWriter.endElement();
            format = format.remove( 0, 2 );
        }
        else if ( format.startsWith( "%e" ) )
        {
            addTextNumber( text, elementWriter );

            elementWriter.startElement( "number:day" );
            elementWriter.addAttribute( "number:style", "short" );
            elementWriter.endElement();
            format = format.remove( 0, 2 );
        }
        else if ( format.startsWith( "%d" ) )
        {
            addTextNumber( text, elementWriter );

            elementWriter.startElement( "number:day" );
            elementWriter.addAttribute( "number:style", "long" );
            elementWriter.endElement();
            format = format.remove( 0, 2 );
        }
        else if ( format.startsWith( "%b" ) )
        {
            addTextNumber( text, elementWriter );
            elementWriter.startElement( "number:month" );
            elementWriter.addAttribute( "number:style", "short" );
            elementWriter.addAttribute( "number:textual", "true");
            elementWriter.endElement();
            format = format.remove( 0, 2 );
        }
        else if ( format.startsWith( "%B" ) )
        {
            addTextNumber( text, elementWriter );
            elementWriter.startElement( "number:month" );
            elementWriter.addAttribute( "number:style", "long" );
            elementWriter.addAttribute( "number:textual", "true");
            elementWriter.endElement();
            format = format.remove( 0, 2 );
        }
        else if ( format.startsWith( "%a" ) )
        {
            addTextNumber( text, elementWriter );
            elementWriter.startElement( "number:day-of-week" );
            elementWriter.addAttribute( "number:style", "short" );
            elementWriter.endElement();

            format = format.remove( 0, 2 );
        }
        else if ( format.startsWith( "%A" ) )
        {
            addTextNumber( text, elementWriter );
            elementWriter.startElement( "number:day-of-week" );
            elementWriter.addAttribute( "number:style", "long" );
            elementWriter.endElement();
            format = format.remove( 0, 2 );
        }
        else
        {
            if ( !saveOasisKlocaleTimeFormat( elementWriter, format, text ) )
            {
                text += format[0];
                format = format.remove( 0, 1 );
            }
        }
    }
    while ( format.length() > 0 );
    addTextNumber( text, elementWriter );
}

QString KoOasisStyles::saveOasisDateStyle( KoGenStyles &mainStyles, const QString & _format, bool klocaleFormat )
{
    kdDebug()<<"QString KoOasisStyles::saveOasisDateStyle( KoGenStyles &mainStyles, const QString & _format ) :"<<_format<<endl;
    QString format( _format );

    // Not supported into Qt: "era" "week-of-year" "quarter"

    KoGenStyle currentStyle( KoGenStyle::STYLE_NUMERIC_DATE );
    QBuffer buffer;
    buffer.open( IO_WriteOnly );
    KoXmlWriter elementWriter( &buffer );  // TODO pass indentation level
    QString text;
    if ( klocaleFormat )
    {
        parseOasisDateKlocale( elementWriter, format, text );
    }
    else
    {
        bool antislash = false;
        do
        {
            if ( antislash )
            {
                text+=format[0];
                format = format.remove( 0, 1 );
            }
            else if ( format.startsWith( "MMMM" ) )
            {
                addTextNumber( text, elementWriter );
                elementWriter.startElement( "number:month" );
                elementWriter.addAttribute( "number:style", "long" );
                elementWriter.addAttribute( "number:textual", "true");
                elementWriter.endElement();
                format = format.remove( 0, 4 );
            }
            else if ( format.startsWith( "MMM" ) )
            {
                addTextNumber( text, elementWriter );
                elementWriter.startElement( "number:month" );
                elementWriter.addAttribute( "number:style", "short" );
                elementWriter.addAttribute( "number:textual", "true");
                elementWriter.endElement();
                format = format.remove( 0, 3 );
            }
            else if ( format.startsWith( "MM" ) )
            {
                addTextNumber( text, elementWriter );
                elementWriter.startElement( "number:month" );
                elementWriter.addAttribute( "number:style", "long" );
                elementWriter.addAttribute( "number:textual", "false"); //not necessary remove it
                elementWriter.endElement();
                format = format.remove( 0, 2 );
            }
            else if ( format.startsWith( "M" ) )
            {
                addTextNumber( text, elementWriter );
                elementWriter.startElement( "number:month" );
                elementWriter.addAttribute( "number:style", "short" );
                elementWriter.addAttribute( "number:textual", "false");
                elementWriter.endElement();
                format = format.remove( 0, 1 );
            }
            else if ( format.startsWith( "dddd" ) )
            {
                addTextNumber( text, elementWriter );

                elementWriter.startElement( "number:day-of-week" );
                elementWriter.addAttribute( "number:style", "long" );
                elementWriter.endElement();
                format = format.remove( 0, 4 );
            }
            else if ( format.startsWith( "ddd" ) )
            {
                addTextNumber( text, elementWriter );

                elementWriter.startElement( "number:day-of-week" );
                elementWriter.addAttribute( "number:style", "short" );
                elementWriter.endElement();
                format = format.remove( 0, 3 );
            }
            else if ( format.startsWith( "dd" ) )
            {
                addTextNumber( text, elementWriter );

                elementWriter.startElement( "number:day" );
                elementWriter.addAttribute( "number:style", "long" );
                elementWriter.endElement();
                format = format.remove( 0, 2 );
            }
            else if ( format.startsWith( "d" ) )
            {
                addTextNumber( text, elementWriter );

                elementWriter.startElement( "number:day" );
                elementWriter.addAttribute( "number:style", "short" );
                elementWriter.endElement();
                format = format.remove( 0, 1 );
            }
            else if ( format.startsWith( "yyyy" ) )
            {
                addTextNumber( text, elementWriter );

                elementWriter.startElement( "number:year" );
                elementWriter.addAttribute( "number:style", "long" );
                elementWriter.endElement();
                format = format.remove( 0, 4 );
            }
            else if ( format.startsWith( "yy" ) )
            {
                addTextNumber( text, elementWriter );

                elementWriter.startElement( "number:year" );
                elementWriter.addAttribute( "number:style", "short" );
                elementWriter.endElement();
                format = format.remove( 0, 2 );
            }
            else
            {
                if ( !saveOasisTimeFormat( elementWriter, format, text,antislash ) )
                {
                    QString elem( format[0] );
                    format = format.remove( 0, 1 );
                    if ( elem == "\\" )
                    {
                        antislash = true;
                    }
                    else
                    {
                        text += elem;
                        antislash = false;
                    }
                }
            }
        }
        while ( format.length() > 0 );
        addTextNumber( text, elementWriter );
    }

    QString elementContents = QString::fromUtf8( buffer.buffer(), buffer.buffer().size() );
    currentStyle.addChildElement( "number", elementContents );
    return mainStyles.lookup( currentStyle, "N" );
}

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
#include <kdebug.h>
#include <qdom.h>

KoOasisStyles::KoOasisStyles()
{

}

KoOasisStyles::~KoOasisStyles()
{

}

void KoOasisStyles::createStyleMap( const QDomDocument& doc )
{
    QDomElement docElement  = doc.documentElement();
    // We used to have the office:version check here, but better let the apps do that
    QDomNode fontStyles = docElement.namedItem( "office:font-decls" );

    if ( !fontStyles.isNull() ) {
        //kdDebug(30003) << "Starting reading in font-decl..." << endl;
        insertStyles( fontStyles.toElement() );
    }// else
    //   kdDebug(30003) << "No items found" << endl;

    //kdDebug(30003) << "Starting reading in office:automatic-styles" << endl;

    QDomNode autoStyles = docElement.namedItem( "office:automatic-styles" );
    if ( !autoStyles.isNull() ) {
        insertStyles( autoStyles.toElement() );
    }// else
    //    kdDebug(30003) << "No items found" << endl;


    //kdDebug(30003) << "Reading in master styles" << endl;

    QDomNode masterStyles = docElement.namedItem( "office:master-styles" );

    if ( !masterStyles.isNull() ) {
        QDomElement master = masterStyles.firstChild().toElement();
        for ( ; !master.isNull() ; master = master.nextSibling().toElement() ) {
            if ( master.tagName() ==  "style:master-page" ) {
                QString name = master.attribute( "style:name" );
                //kdDebug(30003) << "Master style: '" << name << "' loaded " << endl;
                m_masterPages.insert( name, new QDomElement( master ) );
            } else
                // OASIS docu mentions style:handout-master and draw:layer-set here
                kdWarning(30003) << "Unknown tag " << master.tagName() << " in office:master-styles" << endl;
        }
    }


    //kdDebug(30003) << "Starting reading in office:styles" << endl;

    QDomElement officeStyle = docElement.namedItem( "office:styles" ).toElement();

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
    QDomNodeList children = m_officeStyle.elementsByTagName( "style:style" );
    vec.resize( children.length() );
    for ( unsigned int i = 0 ; i < children.length(); ++i )
        vec[i] = children.item( i ).toElement();
    return vec;
}

void KoOasisStyles::insertOfficeStyles( const QDomElement& styles )
{
    for ( QDomNode n = styles.firstChild(); !n.isNull(); n = n.nextSibling() )
    {
        QDomElement e = n.toElement();
        QCString tagName = e.tagName().latin1();
        if ( tagName == "draw:gradient"
             || tagName == "svg:linearGradient"
             || tagName == "svg:radialGradient"
             || tagName == "draw:hatch"
             || tagName == "draw:fill-image"
             || tagName == "draw:marker"
             || tagName == "draw:stroke-dash"
             || tagName == "draw:opacity" )
        {
            Q_ASSERT( e.hasAttribute( "draw:name" ) );
            QString name = e.attribute( "draw:name" );
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
    for ( QDomNode n = styles.firstChild(); !n.isNull(); n = n.nextSibling() )
    {
        insertStyle( n.toElement() );
    }
}

void KoOasisStyles::insertStyle( const QDomElement& e )
{
    QCString tagName = e.tagName().latin1();

    QString name = e.attribute( "style:name" );
    if ( tagName == "style:style"
         || tagName == "style:page-master"
         || tagName == "style:font-decl"
         || tagName == "style:presentation-page-layout" )
    {
        QDomElement* ep = new QDomElement( e );
        m_styles.insert( name, ep );
        //kdDebug(30003) << "Style: '" << name << "' loaded " << endl;
    } else if ( tagName == "style:default-style" ) {
        m_defaultStyle = e;
    } else if ( tagName == "text:list-style" ) {
        QDomElement* ep = new QDomElement( e );
        m_listStyles.insert( name, ep );
        //kdDebug(30003) << "List style: '" << name << "' loaded " << endl;
    } else if ( tagName == "number:number-style"
                || tagName == "number:currency-style"
                || tagName == "number:percentage-style"
                || tagName == "number:boolean-style"
                || tagName == "number:text-style"
                || tagName == "number:date-style"
                || tagName == "number:time-style" ) {
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
    for( QDomNode node( parent.firstChild() ); !node.isNull(); node = node.nextSibling() )
    {
        const QDomElement e( node.toElement() );
        QString tagName = e.tagName();
        if ( !tagName.startsWith( "number:" ) )
            continue;
        tagName.remove( 0, 7 );
        const QString numberStyle = e.attribute( "number:style" );
        const bool shortForm = numberStyle == "short" || numberStyle.isEmpty();
        if ( tagName == "day" ) {
            format += shortForm ? "d" : "dd";
        } else if ( tagName == "day-of-week" ) {
            format += shortForm ? "ddd" : "dddd";
        } else if ( tagName == "month" ) {
            // TODO the spec has a strange mention of number:format-source
            if ( e.attribute( "number:textual" ) == "true" ) {
                format += shortForm ? "MMM" : "MMMM";
            } else { // month number
                format += shortForm ? "M" : "MM";
            }
        } else if ( tagName == "year" ) {
            format += shortForm ? "yy" : "yyyy";
        } else if ( tagName == "era" ) {
            //todo I don't know what is it... (define into oo spec)
        } else if ( tagName == "week-of-year" || tagName == "quarter") {
            // ### not supported in Qt
        } else if ( tagName == "hours" ) {
            format += shortForm ? "h" : "hh";
        } else if ( tagName == "minutes" ) {
            format += shortForm ? "m" : "mm";
        } else if ( tagName == "seconds" ) {
            format += shortForm ? "s" : "ss";
        } else if ( tagName == "am-pm" ) {
            format += "ap";
        } else if ( tagName == "text" ) { // litteral
            format += e.text();
        } else if ( tagName == "currency-symbol" ) {
            format += e.text();
            //todo
            // number:language="de" number:country="DE">€</number:currency-symbol>
        } else if ( tagName == "number:number" ) {
            // TODO: number:grouping="true"
            if ( e.hasAttribute( "number:decimal-places" ) )
            {
                int d = e.attribute( "number:decimal-places" ).toInt( &ok );
                if ( ok )
                    precision = d;
            }
            if ( e.hasAttribute( "number:min-integer-digits" ) )
            {
                int d = e.attribute( "number:min-integer-digits" ).toInt( &ok );
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
        else if ( tagName == "scientific-number" ) {
            int exp = 2;

            if ( e.hasAttribute( "number:decimal-places" ) )
            {
                int d = e.attribute( "number:decimal-places" ).toInt( &ok );
                if ( ok )
                    precision = d;
            }

            if ( e.hasAttribute( "number:min-integer-digits" ) )
            {
                int d = e.attribute( "number:min-integer-digits" ).toInt( &ok );
                if ( ok )
                    leadingZ = d;
            }

            if ( e.hasAttribute( "number:min-exponent-digits" ) )
            {
                int d = e.attribute( "number:min-exponent-digits" ).toInt( &ok );
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
        } else if ( tagName == "fraction" ) {
                int integer = 0;
                int numerator = 1;
                int denominator = 1;

                if ( e.hasAttribute( "number:min-integer-digits" ) )
                {
                    int d = e.attribute( "number:min-integer-digits" ).toInt( &ok );
                    if ( ok )
                        integer = d;
                }
                if ( e.hasAttribute( "number:min-numerator-digits" ) )
                {
                    int d = e.attribute( "number:min-numerator-digits" ).toInt( &ok );
                    if ( ok )
                        numerator = d;
                }
                if ( e.hasAttribute( "number:min-denominator-digits" ) )
                {
                    int d = e.attribute( "number:min-denominator-digits" ).toInt( &ok );
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

#if 0
    // QDate doesn't work both ways!!! It can't parse something back from
    // a string and a format (e.g. 01/02/03 and dd/MM/yy, it will assume MM/dd/yy).
    // So we also need to generate a KLocale-like format, to parse the value
    // Update: we don't need to parse the date back.

    QString kdeFormat;
    for( QDomNode node( parent.firstChild() ); !node.isNull(); node = node.nextSibling() )
    {
        const QDomElement e( node.toElement() );
        QString tagName = e.tagName();
        if ( !tagName.startsWith( "number:" ) )
            continue;
        tagName.remove( 0, 7 );
        const QString numberStyle = e.attribute( "number:style" );
        const bool shortForm = numberStyle == "short" || numberStyle.isEmpty();
        if ( tagName == "day" ) {
            kdeFormat += shortForm ? "%e" : "%d";
        } else if ( tagName == "day-of-week" ) {
            kdeFormat += shortForm ? "%a" : "%A";
        } else if ( tagName == "month" ) {
            // TODO the spec has a strange mention of number:format-source
            if ( e.attribute( "number:textual" ) == "true" ) {
                kdeFormat += shortForm ? "%b" : "%B";
            } else { // month number
                kdeFormat += shortForm ? "%n" : "%m";
            }
        } else if ( tagName == "year" ) {
            kdeFormat += shortForm ? "%y" : "%Y";
        } else if ( tagName == "week-of-year" || tagName == "quarter") {
            // ### not supported in KLocale
        } else if ( tagName == "hours" ) {
            kdeFormat += shortForm ? "%k" : "%H"; // TODO should depend on presence of am/pm
        } else if ( tagName == "minutes" ) {
            kdeFormat += shortForm ? "%M" : "%M"; // KLocale doesn't have 1-digit minutes
        } else if ( tagName == "seconds" ) {
            kdeFormat += shortForm ? "%S" : "%S"; // KLocale doesn't have 1-digit seconds
        } else if ( tagName == "am-pm" ) {
            kdeFormat += "%p";
        } else if ( tagName == "text" ) { // litteral
            kdeFormat += e.text();
        } // TODO number:decimal-places
    }
#endif

    QString styleName = parent.attribute( "style:name" );
    kdDebug(30518) << "datetime style: " << styleName << " qt format=" << format << endl;
    m_dataFormats.insert( styleName, format );
}

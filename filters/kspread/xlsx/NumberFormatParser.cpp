/*
 * This file is part of Office 2007 Filters for KOffice
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Christoph Schleifenbaum christoph@kdab.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */
#include "NumberFormatParser.h"

#include <KoGenStyle.h>
#include <KoGenStyles.h>
#include <KoXmlWriter.h>

#include <QBuffer>
#include <QDebug>
#include <QString>

KoGenStyles* NumberFormatParser::styles = 0;
    
void NumberFormatParser::setStyles( KoGenStyles* styles )
{
    NumberFormatParser::styles = styles;
}

#define SET_TYPE_OR_RETURN( TYPE ) { \
if( type == KoGenStyle::StyleNumericDate && TYPE == KoGenStyle::StyleNumericTime )               \
{                                                                                                \
}                                                                                                \
else if( type == KoGenStyle::StyleNumericTime && TYPE == KoGenStyle::StyleNumericDate )          \
{                                                                                                \
    type = TYPE;                                                                                 \
}                                                                                                \
else if( type == KoGenStyle::StyleNumericPercentage && TYPE == KoGenStyle::StyleNumericNumber )  \
{                                                                                                \
}                                                                                                \
else if( type == KoGenStyle::StyleNumericNumber && TYPE == KoGenStyle::StyleNumericPercentage )  \
{                                                                                                \
    type = TYPE;                                                                                 \
}                                                                                                \             
else if( type == KoGenStyle::StyleNumericFraction && TYPE == KoGenStyle::StyleNumericNumber )    \
{                                                                                                \
}                                                                                                \
else if( type == KoGenStyle::StyleNumericNumber && TYPE == KoGenStyle::StyleNumericFraction )    \
{                                                                                                \
    type = TYPE;                                                                                 \
}                                                                                                \             
else if( type != KoGenStyle::StyleAuto && type != TYPE )                                         \
{                                                                                                \
    return KoGenStyle( KoGenStyle::StyleAuto );                                                  \
}                                                                                                \
else                                                                                             \
{                                                                                                \
    type = TYPE;                                                                                 \
}                                                                                                \
}

#define FINISH_PLAIN_TEXT_PART {             \
if( !plainText.isEmpty() )                   \
{                                            \
    hadPlainText = true;                     \
    xmlWriter.startElement( "number:text" ); \
    xmlWriter.addTextNode( plainText );      \
    xmlWriter.endElement();                  \
    plainText.clear();                       \
}                                            \
}

static KoGenStyle styleFromTypeAndBuffer( KoGenStyle::Type type, const QBuffer& buffer )
{
    KoGenStyle result( type );
    
    const QString elementContents = QString::fromUtf8( buffer.buffer(), buffer.buffer().size() );
    result.addChildElement( "number", elementContents );

    return result;
}

KoGenStyle NumberFormatParser::parse( const QString& numberFormat )
{
    QBuffer buffer;
    buffer.open( QIODevice::WriteOnly );
    KoXmlWriter xmlWriter( &buffer );

    KoGenStyle::Type type = KoGenStyle::StyleAuto;

    QString plainText;

    QMap< QString, QString > conditions;

    // this is for the month vs. minutes-context
    bool justHadHours = false;
    int justHadMMM = 0; // contains the number of 'm's in case

    bool hadPlainText = false;

    for( int i = 0; i < numberFormat.length(); ++i )
    {
        const char c = numberFormat[ i ].toLatin1();
                
        const bool isLong = numberFormat[ i + 1 ] == c                         && i < numberFormat.length() - 1;
        const bool isLonger = isLong && numberFormat[ i + 2 ] == c             && i < numberFormat.length() - 2;
        const bool isLongest = isLonger && numberFormat[ i + 3 ] == c          && i < numberFormat.length() - 3;
        const bool isWayTooLong = isWayTooLong && numberFormat[ i + 4 ] == c   && i < numberFormat.length() - 4;


        if( justHadMMM > 0 )
        {
            switch( c )
            {
            // m, followed by seconds: must be minutes, then
            case 'S':
            case 's':
                SET_TYPE_OR_RETURN( KoGenStyle::StyleNumericTime )
                FINISH_PLAIN_TEXT_PART;
                xmlWriter.startElement( "number:minutes" );
                if( justHadMMM > 1 )
                    xmlWriter.addAttribute( "number:style", "long" );
                xmlWriter.endElement();
                justHadMMM = 0;
                break;

            // any other, must be the month, then
            case 'A':
            case 'a':
            case 'H':
            case 'h':
            case 'M': // shouldn't happen, though
            case 'm': // ...
            case 'D':
            case 'd':
            case 'Y':
            case 'y':
            case '.':
            case ',':
            case '#':
            case '/':
            case '0':
            case ';':
            case '@':
            case '"':
            case '\\':
                SET_TYPE_OR_RETURN( KoGenStyle::StyleNumericTime )
                FINISH_PLAIN_TEXT_PART;
                xmlWriter.startElement( "number:month" );
                if( justHadMMM > 1 )
                    xmlWriter.addAttribute( "number:style", "long" );
                xmlWriter.endElement();
                justHadMMM = 0;
                break;

            // default is plain text and is getting ignored here
            default:
                break;
            }
        }



        switch( c )
        {
        // condition or color or locale...
        case '[':
            {
                const char ch = numberFormat[ ++i ].toLatin1();
                if( ( ch >= 'a' && ch <= 'z' ) || ( ch >= 'A' && ch <= 'Z' ) )
                {
                    // color code
                    QString colorName;
                    while( i < numberFormat.length() && numberFormat[ i ] != QLatin1Char( ']' ) )
                        colorName += numberFormat[ i++ ];
                    
                }
                else
                {
                    // unknown - no idea, skip
                    while( i < numberFormat.length() && numberFormat[ i ] != QLatin1Char( ']' ) )
                        ++i;
                }
            }
            break;

        // percentage
        case '%':
            SET_TYPE_OR_RETURN( KoGenStyle::StyleNumericPercentage );
            FINISH_PLAIN_TEXT_PART
            xmlWriter.startElement( "number:text" );
            xmlWriter.addTextNode( "%" );
            xmlWriter.endElement();
            break;

        // a number
        case '.':
        case ',':
        case '#':
        case '0':
        case '/':
            SET_TYPE_OR_RETURN( KoGenStyle::StyleNumericNumber )
            FINISH_PLAIN_TEXT_PART
            {
                bool grouping = false;
                bool gotDot = false;
                int decimalPlaces = 0;
                int integerDigits = 0;
                
                char ch = numberFormat[ i ].toLatin1();
                do
                {
                    if( ch == '.' )
                        gotDot = true;
                    else if( ch == ',' )
                        grouping = true;
                    else if( ch == '0' && !gotDot )
                        ++integerDigits;
                    else if( ch == '0' && gotDot )
                        ++decimalPlaces;
                    else if( ch == '?' )
                    { // no idea how to handle... }
                    else if( ch == '/' )
                    {
                        SET_TYPE_OR_RETURN( KoGenStyle::StyleNumericFraction );
                        if( gotDot || grouping )
                            return KoGenStyle();
                    }

                    ch = numberFormat[ ++i ].toLatin1();
                }
                while( i < numberFormat.length() && ( ch == '.' || ch == ',' || ch == '#' || ch == '0' || ch == '/' || ch == '?' ) );
               
                if( !( ch == '.' || ch == ',' || ch == '#' || ch == '0' || ch == '/' || ch == '?' ) )
                    --i;

                xmlWriter.startElement( "number:nmber" );
                if( gotDot )
                    xmlWriter.addAttribute( "number:decimal-places", decimalPlaces );
                xmlWriter.addAttribute( "number:min-integer-digits", integerDigits );
                xmlWriter.addAttribute( "number:grouping", grouping ? "true" : "false" );
                xmlWriter.endElement();
            }
            break;


        // Everything related to date/time
        // AM/PM
        case 'A':
        case 'a':
            if( numberFormat.mid( i, 5 ).toLower() == QLatin1String( "am/pm" ) ||
                numberFormat.mid( i, 3 ).toLower() == QLatin1String( "a/p" ) )
            {
                SET_TYPE_OR_RETURN( KoGenStyle::StyleNumericTime )
                FINISH_PLAIN_TEXT_PART;
                xmlWriter.startElement( "number:am-pm" );
                xmlWriter.endElement();
                if( numberFormat.mid( i, 5 ).toLower() == QLatin1String( "am/pm" ) )
                    i += 2;
                i += 2;
            }
            break;

        
        // hours, long or short
        case 'H':
        case 'h':
            SET_TYPE_OR_RETURN( KoGenStyle::StyleNumericTime )
            FINISH_PLAIN_TEXT_PART;
            xmlWriter.startElement( "number:hours" );
            if( isLong )
            {
                xmlWriter.addAttribute( "number:style", "long" );
                ++i;
            }
            xmlWriter.endElement();
            break;


        // minutes or months, depending on context
        case 'M':
        case 'm':
            // must be month, then, at least three M
            if( isLonger )
            {
                SET_TYPE_OR_RETURN( KoGenStyle::StyleNumericDate )
                FINISH_PLAIN_TEXT_PART;
                xmlWriter.startElement( "number:month" );
                if( isLongest )
                    xmlWriter.addAttribute( "number:style", "long" );
                xmlWriter.addAttribute( "number:textual", "true" );
                xmlWriter.endElement();
                i += isLongest ? ( isWayTooLong ? 4 : 3 ) : 2;
            }
            // depends on the context. After hours and before seconds, it's minutes
            // otherwise it's the month
            else
            {
                if( justHadHours )
                {
                    SET_TYPE_OR_RETURN( KoGenStyle::StyleNumericTime )
                    FINISH_PLAIN_TEXT_PART;
                    xmlWriter.startElement( "number:minutes" );
                    if( isLong )
                        xmlWriter.addAttribute( "number:style", "long" );
                    xmlWriter.endElement();
                }
                else
                {
                    // on the next iteration, we might see wheter there're seconds or something else
                    justHadMMM = isLong ? 2 : 1;
                }
                if( isLong )
                    ++i;
            }
            break;


        // day (of week)
        case 'D':
        case 'd':
            SET_TYPE_OR_RETURN( KoGenStyle::StyleNumericDate )
            FINISH_PLAIN_TEXT_PART;
            if( !isLonger )
            {
                xmlWriter.startElement( "number:day" );
                if( isLong )
                    xmlWriter.addAttribute( "number:style", "long" );
            }
            xmlWriter.endElement();
            if( isLong )
                ++i;
            break;

        // seconds, long or short
        case 'S':
        case 's':
            SET_TYPE_OR_RETURN( KoGenStyle::StyleNumericTime )
            FINISH_PLAIN_TEXT_PART;
            xmlWriter.startElement( "number:seconds" );
            if( isLong )
            {
                xmlWriter.addAttribute( "number:style", "long" );
                ++i;
            }
            xmlWriter.endElement();
            break;

        // year, long or short
        case 'Y':
        case 'y':
            SET_TYPE_OR_RETURN( KoGenStyle::StyleNumericDate )
            FINISH_PLAIN_TEXT_PART;
            xmlWriter.startElement( "number:year" );
            if( isLonger )
            {
                xmlWriter.addAttribute( "number:style", "long" );
                ++i;
            }
            xmlWriter.endElement();
            break; 

        // now it's getting really scarry: semi-colon:
        case ';':
            {
                buffer.close();
                // conditional style with the current format
                const KoGenStyle result = styleFromTypeAndBuffer( type, buffer );
                const QString styleName = NumberFormatParser::styles->lookup( result, "N" );
                const QString condition = QLatin1String( "value()>=0" );
                // start a new style
                buffer.setData( QByteArray() );
                buffer.open( QIODevice::WriteOnly );
                conditions[ condition ] = styleName;
            }
            break;

        // text-content
        case '@':
            FINISH_PLAIN_TEXT_PART;
            xmlWriter.startElement( "number:text-content" );
            xmlWriter.endElement();
            break;

        // quote - plain text block
        case '"':
            while( i < numberFormat.length() - 1 && numberFormat[ ++i ] != QLatin1Char( '"' ) )
                plainText += numberFormat[ i ];
            break; 

        // backslash escapes the next char
        case '\\':
            if( i < numberFormat.length() - 1 )
                plainText += numberFormat[ ++i ];
            break;

        // every other char is just passed
        default:
            plainText += c;
            break;
        }
    

        // for the context-sensitive 'M'
        if( plainText.isEmpty() )
            justHadHours = ( c == 'h' || c == 'H' );
        
    }
            
    FINISH_PLAIN_TEXT_PART;

    if( type == KoGenStyle::StyleAuto && hadPlainText )
        SET_TYPE_OR_RETURN( KoGenStyle::StyleNumericText )

    // add conditional styles:
    for( QMap< QString, QString >::const_iterator it = conditions.begin(); it != conditions.end(); ++it )
    {
        xmlWriter.startElement( "style:map" );
        xmlWriter.addAttribute( "style:condition", it.key() );
        xmlWriter.addAttribute( "style:apply-style-name", it.value() );
        xmlWriter.endElement();
    }

    buffer.close();
    
    qDebug() << buffer.data();

    return styleFromTypeAndBuffer( type, buffer );
}

/* This file is part of the KDE project
   Copyright (C) 2003 Norbert Andres, nandres@web.de

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


#include "kspread_style.h"
#include "kspread_util.h"
#include "kspread_doc.h"

#include <kdebug.h>
#include <koGlobal.h>
#include <klocale.h>
#include <koGenStyles.h>
#include <qdom.h>
#include <qbuffer.h>
#include <koStyleStack.h>
#include <koxmlwriter.h>
#include <koxmlns.h>

static uint calculateValue( QPen const & pen )
{
  uint n = pen.color().red() + pen.color().green() + pen.color().blue();

  n += 1000 * pen.width();
  n += 10000 * (uint) pen.style();

  return n;
}

KSpreadStyle::KSpreadStyle()
  : m_parent( 0 ),
    m_type( AUTO ),
    m_usageCount( 0 ),
    m_featuresSet( 0 ),
    m_alignX( KSpreadFormat::Undefined ),
    m_alignY( KSpreadFormat::Middle ),
    m_floatFormat( KSpreadFormat::OnlyNegSigned ),
    m_floatColor( KSpreadFormat::AllBlack ),
    m_formatType( Generic_format ),
    m_fontFlags( 0 ),
    m_bgColor( Qt::white ),
    m_backGroundBrush( Qt::red, Qt::NoBrush ),
    m_rotateAngle( 0 ),
    m_indent( 0.0 ),
    m_precision( -1 ),
    m_properties( 0 )
{
  QFont f( KoGlobal::defaultFont() );
  m_fontFamily = f.family();
  m_fontSize = f.pointSize();

  QPen pen( Qt::black, 1, Qt::NoPen );

  m_leftBorderPen   = pen;
  m_topBorderPen    = pen;
  m_rightBorderPen  = pen;
  m_bottomBorderPen = pen;
  m_fallDiagonalPen = pen;
  m_goUpDiagonalPen = pen;

  m_leftPenValue    = calculateValue( pen );
  m_topPenValue     = calculateValue( pen );
  m_rightPenValue   = calculateValue( pen );
  m_bottomPenValue  = calculateValue( pen );

  m_currency.type   = 0;
}

KSpreadStyle::KSpreadStyle( KSpreadStyle * style )
  : m_parent( ( style->m_type == BUILTIN || style->m_type == CUSTOM ) ? (KSpreadCustomStyle *) style : 0 ),
    m_type( AUTO ),
    m_usageCount( 1 ),
    m_featuresSet( ( style->m_type == BUILTIN || style->m_type == CUSTOM ) ? 0 : style->m_featuresSet ),
    m_alignX( style->m_alignX ),
    m_alignY( style->m_alignY ),
    m_floatFormat( style->m_floatFormat ),
    m_floatColor( style->m_floatColor ),
    m_formatType( style->m_formatType ),
    m_fontFamily( style->m_fontFamily ),
    m_fontFlags( style->m_fontFlags ),
    m_fontSize( style->m_fontSize ),
    m_textPen( style->m_textPen ),
    m_bgColor( style->m_bgColor ),
    m_rightBorderPen( style->m_rightBorderPen ),
    m_bottomBorderPen( style->m_bottomBorderPen ),
    m_leftBorderPen( style->m_leftBorderPen ),
    m_topBorderPen( style->m_topBorderPen ),
    m_fallDiagonalPen( style->m_fallDiagonalPen ),
    m_goUpDiagonalPen( style->m_goUpDiagonalPen ),
    m_backGroundBrush( style->m_backGroundBrush ),
    m_rotateAngle( style->m_rotateAngle ),
    m_indent( style->m_indent ),
    m_strFormat( style->m_strFormat ),
    m_precision( style->m_precision ),
    m_prefix( style->m_prefix ),
    m_postfix( style->m_postfix ),
    m_currency( style->m_currency ),
    m_properties( style->m_properties )
{
}

KSpreadStyle::~KSpreadStyle()
{
}

void KSpreadStyle::loadOasisStyle( KoOasisStyles& oasisStyles, const QDomElement & element )
{
    kdDebug()<<"void KSpreadStyle::loadOasisStyle( const QDomElement & element )**************: name :"<<endl;
    KoStyleStack styleStack;
    styleStack.push( element );
    styleStack.setTypeProperties( "table-cell" );
    QString str;
    if ( element.hasAttributeNS( KoXmlNS::style, "data-style-name" ) )
    {
        //kdDebug()<<"styleStack.attribute( style:data-style-name ) :"<<element.attributeNS( KoXmlNS::style, "data-style-name", QString::null )<<endl;
        //kdDebug()<< " oasisStyles.dataFormats()[...] :"<< oasisStyles.dataFormats()[element.attributeNS( KoXmlNS::style, "data-style-name" , QString::null)].formatStr<<endl;
        //kdDebug()<< " oasisStyles.dataFormats()[...] prefix :"<< oasisStyles.dataFormats()[element.attributeNS( KoXmlNS::style, "data-style-name" , QString::null)].prefix<<endl;
        //kdDebug()<< " oasisStyles.dataFormats()[...] suffix :"<< oasisStyles.dataFormats()[element.attributeNS( KoXmlNS::style, "data-style-name" , QString::null)].suffix<<endl;

        QString tmp = oasisStyles.dataFormats()[element.attributeNS( KoXmlNS::style, "data-style-name" , QString::null)].prefix;
        if ( !tmp.isEmpty() )
        {
            m_prefix = tmp;
            m_featuresSet |= SPrefix;
        }
        tmp = oasisStyles.dataFormats()[element.attributeNS( KoXmlNS::style, "data-style-name" , QString::null)].suffix;
        if ( !tmp.isEmpty() )
        {
            m_postfix = tmp;
            m_featuresSet |= SPostfix;
        }
        tmp = oasisStyles.dataFormats()[element.attributeNS( KoXmlNS::style, "data-style-name" , QString::null)].formatStr;
        if ( !tmp.isEmpty() )
        {
            m_formatType = KSpreadStyle::formatType( tmp );
            m_featuresSet |= SFormatType;
        }
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::style, "font-name" ) )
    {
        m_fontFamily = styleStack.attributeNS( KoXmlNS::style, "font-name" );
        kdDebug()<<"styleStack.hasAttribute( style:font-name ) :"<<styleStack.hasAttributeNS( KoXmlNS::style, "font-name" )<<endl;
        m_featuresSet |= SFontFamily;
        m_featuresSet |= SFont;
        m_featuresSet |= SFontFlag;
    }
//fo:font-size="13pt" fo:font-style="italic" style:text-underline="double" style:text-underline-color="font-color" fo:font-weight="bold"
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "font-size" ) )
    {
        m_fontSize = KoUnit::parseValue( styleStack.attributeNS( KoXmlNS::fo, "font-size" ),10.0 );
        m_featuresSet |= SFont;
        m_featuresSet |= SFontSize;
        m_featuresSet |= SFontFlag;
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "font-style" ) && styleStack.attributeNS( KoXmlNS::fo, "font-style" ) =="italic")
    {
#if 0
        QDomElement font = format.namedItem( "font" ).toElement();
        if ( !font.isNull() )
        {
            QFont f( util_toFont( font ) );
            m_fontFamily = f.family();
            m_fontSize = f.pointSize();
            if ( f.italic() )
                m_fontFlags |= FItalic;
            if ( f.bold() )
                m_fontFlags |= FBold;
            if ( f.underline() )
                m_fontFlags |= FUnderline;
            if ( f.strikeOut() )
                m_fontFlags |= FStrike;

            m_featuresSet |= SFont;
            m_featuresSet |= SFontFamily;
            m_featuresSet |= SFontFlag;
            m_featuresSet |= SFontSize;
        }

        if ( format.hasAttribute( "font-family" ) )
        {
            m_fontFamily = format.attribute( "font-family" );
            m_featuresSet |= SFont;
            m_featuresSet |= SFontFamily;
        }
#endif
        m_fontFlags |= FItalic;
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "font-weight" ) )
    {
        m_fontFlags |= FBold;
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "text-underline" ) || styleStack.hasAttributeNS( KoXmlNS::style, "text-underline" ))
    {
        m_fontFlags |= FUnderline;
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "color" ) )
    {
        //FIXME
        m_featuresSet |= STextPen;
        m_textPen=QPen( QColor( styleStack.attributeNS( KoXmlNS::fo, "color" ) ) );
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::style, "text-underline-color" ) )
    {
        //TODO
    }

    if ( styleStack.hasAttributeNS( KoXmlNS::style, "text-crossing-out" ) )
    {
        m_fontFlags |= FStrike;
    }

    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "text-align" ) )
    {

        str = styleStack.attributeNS( KoXmlNS::fo, "text-align" );
        kdDebug()<<"str :"<<str<<endl;
        if ( str == "center" )
            m_alignX = KSpreadFormat::Center;
        else if ( str == "end" )
            m_alignX = KSpreadFormat::Right;
        else if ( str == "start" )
            m_alignX = KSpreadFormat::Left;
        else
            m_alignX = KSpreadFormat::Undefined;
        m_featuresSet |= SAlignX;
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::style, "vertical-align" ) )
    {
        str = styleStack.attributeNS( KoXmlNS::style, "vertical-align" );
        if ( str == "bottom" )
            m_alignY = KSpreadFormat::Bottom;
        else if ( str =="top" )
            m_alignY = KSpreadFormat::Top;
        else if ( str =="middle" )//FIXME !!!
            m_alignY = KSpreadFormat::Middle;
        m_featuresSet |= SAlignY;
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "background-color" ) )
    {
        m_bgColor = QColor(  styleStack.attributeNS( KoXmlNS::fo, "background-color" ) );
        m_featuresSet |= SBackgroundColor;
    }

    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "wrap-option" )&&( styleStack.attributeNS( KoXmlNS::fo, "wrap-option" )=="wrap" ) )
    {
        setProperty( PMultiRow );
        m_featuresSet |= SMultiRow;
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::style, "cell-protect" ) )
    {
        str = styleStack.attributeNS( KoXmlNS::style, "cell-protect" );
        if ( str=="hidden-and-protected" )
        {
            setProperty( PHideAll );
            m_featuresSet |= SHideAll;
        }
        else if ( str == "protected formula-hidden" )
        {
            setProperty( PHideFormula );
            m_featuresSet |= SHideFormula;
        }
        else if ( str == "protected" )
        {
            setProperty( PNotProtected );
            m_featuresSet |= SNotProtected;
        }
        else if ( str =="formula-hidden" )
        {
            //FIXME !!!!
#if 0
            setNotProtected( true );
            setHideFormula( true );
            setHideAll( false );
#endif
        }
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::style, "print-content" ) && ( styleStack.attributeNS( KoXmlNS::style, "print-content" )=="false" ) )
    {
        setProperty( PDontPrintText );
        m_featuresSet |= SDontPrintText;

    }
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "direction" ) && ( styleStack.attributeNS( KoXmlNS::fo, "direction" )=="ttb" ) )
    {
        setProperty( PVerticalText );
        m_featuresSet |= SVerticalText;

    }
    if ( styleStack.hasAttributeNS( KoXmlNS::style, "rotation-angle" ) )
    {
        bool ok;
        int a = styleStack.attributeNS( KoXmlNS::style, "rotation-angle" ).toInt( &ok );
        kdDebug()<<" rotation-angle :"<<a<<endl;
        if ( a != 0 )
        {
            m_rotateAngle= ( -a  );
            m_featuresSet |= SAngle;
        }
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "margin-left" ) )
    {
        //todo fix me
        setIndent( KoUnit::parseValue( styleStack.attributeNS( KoXmlNS::fo, "margin-left" ),0.0 ) );
        m_featuresSet |= SIndent;
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "border" ) )
    {
        str=styleStack.attributeNS( KoXmlNS::fo, "border" );
        QPen pen = convertOasisStringToPen( str );
        m_featuresSet |= SLeftBorder;
        m_featuresSet |= SRightBorder;
        m_featuresSet |= STopBorder;
        m_featuresSet |= SBottomBorder;
        m_leftBorderPen = pen;
        m_topBorderPen = pen;
        m_bottomBorderPen = pen;
        m_rightBorderPen = pen;
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "border-left" ) )
    {
        str=styleStack.attributeNS( KoXmlNS::fo, "border-left" );
        m_leftBorderPen = convertOasisStringToPen( str );
        m_featuresSet |= SLeftBorder;
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "border-right" ) )
    {
        str=styleStack.attributeNS( KoXmlNS::fo, "border-right" );
        m_rightBorderPen = convertOasisStringToPen( str );
        m_featuresSet |= SRightBorder;
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "border-top" ) )
    {
        str=styleStack.attributeNS( KoXmlNS::fo, "border-top" );
        m_topBorderPen = convertOasisStringToPen( str );
        m_featuresSet |= STopBorder;
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "border-bottom" ) )
    {
        str=styleStack.attributeNS( KoXmlNS::fo, "border-bottom" );
        m_bottomBorderPen = convertOasisStringToPen( str );
        m_featuresSet |= SBottomBorder;
    }
    if (styleStack.hasAttributeNS( KoXmlNS::style, "diagonal-tl-br" ) )
    {
        str=styleStack.attributeNS( KoXmlNS::style, "diagonal-tl-br" );
        m_fallDiagonalPen = convertOasisStringToPen( str );
        m_featuresSet |= SFallDiagonal;
    }
    if (styleStack.hasAttributeNS( KoXmlNS::style, "diagonal-bl-tr" ) )
    {
        str=styleStack.attributeNS( KoXmlNS::style, "diagonal-bl-tr" );
        m_goUpDiagonalPen = convertOasisStringToPen( str );
        m_featuresSet |= SGoUpDiagonal;
    }

    if ( styleStack.hasAttributeNS( KoXmlNS::draw, "style-name" ) )
    {
        kdDebug()<<" style name :"<<styleStack.attributeNS( KoXmlNS::draw, "style-name" )<<endl;

        QDomElement * style = oasisStyles.styles()[styleStack.attributeNS( KoXmlNS::draw, "style-name" )];
        kdDebug()<<" style :"<<style<<endl;
        KoStyleStack drawStyleStack;
        drawStyleStack.push( *style );
        drawStyleStack.setTypeProperties( "graphic" );
        if ( drawStyleStack.hasAttributeNS( KoXmlNS::draw, "fill" ) )
        {
            const QString fill = drawStyleStack.attributeNS( KoXmlNS::draw, "fill" );
            kdDebug()<<" load object gradient fill type :"<<fill<<endl;

            if ( fill == "solid" || fill == "hatch" )
            {
                kdDebug()<<" Style ******************************************************\n";
                m_backGroundBrush=KoOasisStyles::loadOasisFillStyle( drawStyleStack, fill, oasisStyles );
                m_featuresSet |= SBackgroundBrush;
            }
            else
                kdDebug()<<" fill style not supported into kspread : "<<fill<<endl;
        }
    }

#if 0
    bool ok;
    if ( format.hasAttribute( "type" ) )
    {
        m_type = (StyleType) format.attribute( "type" ).toInt( &ok );
        if ( !ok )
            return false;
    }

    if ( format.hasAttribute( "precision" ) )
    {
        int i = format.attribute( "precision" ).toInt( &ok );
        if ( i < -1 )
        {
            kdDebug(36001) << "Value out of range Cell::precision=" << i << endl;
            return false;
        }
        m_precision = i;
        m_featuresSet |= SPrecision;
    }

    if ( format.hasAttribute( "float" ) )
    {
        KSpreadFormat::FloatFormat a = (KSpreadFormat::FloatFormat)format.attribute( "float" ).toInt( &ok );
        if ( !ok )
            return false;
        if ( (unsigned int) a >= 1 || (unsigned int) a <= 3 )
        {
            m_floatFormat = a;
            m_featuresSet |= SFloatFormat;
        }
    }

    if ( format.hasAttribute( "floatcolor" ) )
    {
        KSpreadFormat::FloatColor a = (KSpreadFormat::FloatColor) format.attribute( "floatcolor" ).toInt( &ok );
        if ( !ok ) return false;
        if ( (unsigned int) a >= 1 || (unsigned int) a <= 2 )
        {
            m_floatColor = a;
            m_featuresSet |= SFloatColor;
        }
    }

    if ( format.hasAttribute( "custom" ) )
    {
        m_strFormat = format.attribute( "custom" );
        m_featuresSet |= SCustomFormat;
    }
    if ( m_formatType == KSpreadFormat::Money )
    {
        if ( format.hasAttribute( "type" ) )
        {
            m_currency.type   = format.attribute( "type" ).toInt( &ok );
            if (!ok)
                m_currency.type = 1;
        }
        if ( format.hasAttribute( "symbol" ) )
        {
            m_currency.symbol = format.attribute( "symbol" );
        }
    }

    // TODO: remove that...
    QDomElement font = format.namedItem( "font" ).toElement();
    if ( !font.isNull() )
    {
        QFont f( util_toFont( font ) );
        m_fontFamily = f.family();
        m_fontSize = f.pointSize();
        if ( f.italic() )
            m_fontFlags |= FItalic;
        if ( f.bold() )
            m_fontFlags |= FBold;
        if ( f.underline() )
            m_fontFlags |= FUnderline;
        if ( f.strikeOut() )
            m_fontFlags |= FStrike;

        m_featuresSet |= SFont;
        m_featuresSet |= SFontFamily;
        m_featuresSet |= SFontFlag;
        m_featuresSet |= SFontSize;
    }

    if ( format.hasAttribute( "font-family" ) )
    {
        m_fontFamily = format.attribute( "font-family" );
        m_featuresSet |= SFont;
        m_featuresSet |= SFontFamily;
    }


    if ( format.hasAttribute( "font-flags" ) )
    {
        m_fontFlags = format.attribute( "font-flags" ).toInt( &ok );
        if ( !ok )
            return false;
        m_featuresSet |= SFont;
        m_featuresSet |= SFontFlag;
    }

    if ( format.hasAttribute( "brushcolor" ) )
    {
        m_backGroundBrush.setColor( QColor( format.attribute( "brushcolor" ) ) );
        m_featuresSet |= SBackgroundBrush;
    }

    if ( format.hasAttribute( "brushstyle" ) )
    {
        m_backGroundBrush.setStyle( (Qt::BrushStyle) format.attribute( "brushstyle" ).toInt( &ok )  );
        if ( !ok )
            return false;
        m_featuresSet |= SBackgroundBrush;
    }

    QDomElement pen = format.namedItem( "pen" ).toElement();
    if ( !pen.isNull() )
    {
        m_textPen = util_toPen( pen );
        m_featuresSet |= STextPen;
    }

    return true;

#endif
}

FormatType KSpreadStyle::formatType( const QString &_format )
{
    if ( _format == "# ?/2" )
        return fraction_half;
    else if ( _format =="# ?/4" )
        return fraction_quarter;
    else if ( _format == "# ?/8" )
        return fraction_eighth;
    else if ( _format == "# ?/16" )
        return fraction_sixteenth;
    else if ( _format == "# ?/10" )
        return fraction_tenth;
    else if ( _format == "# ?/100" )
        return fraction_hundredth;
    else if ( _format == "# ?/?" )
        return fraction_one_digit;
    else if ( _format == "# \?\?/\?\?" )
        return fraction_two_digits;
    else if ( _format == "# \?\?\?/\?\?\?" )
        return fraction_three_digits;
    else if ( _format == "dd-MMM-yy" )
        return date_format1;
    else if ( _format == "dd-MMM-yyyy" )
        return date_format2;
    else if ( _format == "d-MM" )
        return date_format3;
    else if ( _format == "dd-MM" ) //TODO ???????
        return date_format4;
    else if ( _format == "dd/MM/yy" )
        return date_format5;
    else if ( _format == "dd/MM/yyyy" )
        return date_format6;
    else if ( _format == "MMM-yy" )
        return date_format7;
    else if ( _format == "MMMM-yyyy" )
        return date_format9;
    else if ( _format == "MMMMM-yy" )
        return date_format10;
    else if ( _format == "dd/MMM" )
        return date_format11;
    else if ( _format == "dd/MM" )
        return date_format12;
    else if ( _format == "dd/MMM/yyyy" )
        return date_format13;
    else if ( _format == "yyyy/MMM/dd" )
        return date_format14;
    else if ( _format == "yyyy-MMM-dd" )
        return date_format15;
    else if ( _format == "yyyy/MM/dd" )
        return date_format16;
    else if ( _format == "d MMMM yyyy" )
        return date_format17;
    else if ( _format == "MM/dd/yyyy" )
        return date_format18;
    else if ( _format == "MM/dd/yy" )
        return date_format19;
    else if ( _format == "MMM/dd/yy" )
        return date_format20;
    else if ( _format == "MMM/dd/yyyy" )
        return date_format21;
    else if ( _format == "MMM-yyyy" )
        return date_format22;
    else if ( _format == "yyyy" )
        return date_format23;
    else if ( _format == "yy" )
        return date_format24;
    else if ( _format == "yyyy/MM/dd" )
        return date_format25;
    else if ( _format == "yyyy/MMM/dd" )
        return date_format26;
    else if ( _format == KGlobal::locale()->dateFormatShort() ) //TODO FIXME
        return ShortDate_format;
    else if ( _format == KGlobal::locale()->dateFormat() ) //TODO FIXME
        return TextDate_format;
    else if ( _format == "h:mm AP" )
        return Time_format1;
    else if ( _format == "h:mm:ss AP" )
        return Time_format2;
    else if ( _format == "hh \\h mm \\m\\i\\n ss \\s" )
        return Time_format3;
    else if ( _format == "hh:mm" )
        return Time_format4;
    else if ( _format == "hh:mm:ss" )
        return Time_format5;
    else if ( _format == "m:ss" )
        return Time_format6;
    else if ( _format == "h:mm:ss" )
        return Time_format7;
    else if ( _format == "h:mm" )
        return Time_format8;
    else
        return Number_format;
}

QString KSpreadStyle::saveOasisStyleNumeric( KoGenStyles &mainStyles, FormatType _style, const QString &_prefix, const QString &_postfix,int _precision )
{
    QString styleName;
    switch( _style )
    {
    case Number_format:
        styleName = saveOasisStyleNumericNumber( mainStyles,_style, _precision );
        break;
    case Text_format:
        styleName = saveOasisStyleNumericText( mainStyles,_style,_precision );
        break;
    case Money_format:
        styleName = saveOasisStyleNumericMoney( mainStyles,_style,_precision);
        break;
    case Percentage_format:
        styleName = saveOasisStyleNumericPercentage( mainStyles,_style,_precision );
        break;
    case Scientific_format:
        styleName = saveOasisStyleNumericScientific( mainStyles,_style, _prefix, _postfix,_precision );
        break;
    case ShortDate_format:
    case TextDate_format:
        styleName = saveOasisStyleNumericDate( mainStyles,_style );
        break;
    case Time_format:
    case SecondeTime_format:
    case Time_format1:
    case Time_format2:
    case Time_format3:
    case Time_format4:
    case Time_format5:
    case Time_format6:
    case Time_format7:
    case Time_format8:
        styleName = saveOasisStyleNumericTime( mainStyles,_style );
        break;
    case fraction_half:
    case fraction_quarter:
    case fraction_eighth:
    case fraction_sixteenth:
    case fraction_tenth:
    case fraction_hundredth:
    case fraction_one_digit:
    case fraction_two_digits:
    case fraction_three_digits:
        styleName = saveOasisStyleNumericFraction( mainStyles,_style, _prefix, _postfix );
        break;
    case date_format1:
    case date_format2:
    case date_format3:
    case date_format4:
    case date_format5:
    case date_format6:
    case date_format7:
    case date_format8:
    case date_format9:
    case date_format10:
    case date_format11:
    case date_format12:
    case date_format13:
    case date_format14:
    case date_format15:
    case date_format16:
    case date_format17:
    case date_format18:
    case date_format19:
    case date_format20:
    case date_format21:
    case date_format22:
    case date_format23:
    case date_format24:
    case date_format25:
    case date_format26:
        styleName = saveOasisStyleNumericDate( mainStyles,_style );
        break;
    case Custom_format:
        styleName = saveOasisStyleNumericCustom( mainStyles,_style );
        break;
    }
    return styleName;
}

QString KSpreadStyle::saveOasisStyleNumericNumber( KoGenStyles&mainStyles, FormatType _style, int _precision )
{
    return "";
}

QString KSpreadStyle::saveOasisStyleNumericText( KoGenStyles&mainStyles, FormatType _style, int _precision )
{
    return "";
}

QString KSpreadStyle::saveOasisStyleNumericMoney( KoGenStyles&mainStyles, FormatType _style, int _precision )
{
    QString format;
    return KoOasisStyles::saveOasisCurrencyStyle( mainStyles, format );
}

QString KSpreadStyle::saveOasisStyleNumericPercentage( KoGenStyles&mainStyles, FormatType _style, int _precision )
{
    //<number:percentage-style style:name="N106" style:family="data-style">
    //<number:number number:decimal-places="6" number:min-integer-digits="1"/>
    //<number:text>%</number:text>
    //</number:percentage-style>
    //TODO add decimal etc.
    QString format;
    if ( _precision == -1 )
        format="0.00";
    else
    {
        QString tmp;
        for ( int i = 0; i <_precision; i++ )
        {
            tmp+="0";
        }
        format = "0."+tmp;
    }
    return KoOasisStyles::saveOasisPercentageStyle( mainStyles, format );
}


QString KSpreadStyle::saveOasisStyleNumericScientific( KoGenStyles&mainStyles, FormatType _style, const QString &_prefix, const QString _suffix, int _precision )
{
    //<number:number-style style:name="N60" style:family="data-style">
    //  <number:scientific-number number:decimal-places="2" number:min-integer-digits="1" number:min-exponent-digits="3"/>
    //</number:number-style>
    QString format;
    if ( _precision == -1 )
        format="0.0E+00";
    else
    {
        QString tmp;
        for ( int i = 0; i <_precision; i++ )
        {
            tmp+="0";
        }
        format = "0."+tmp+"E+00";
    }
    return KoOasisStyles::saveOasisScientificStyle( mainStyles, format, _prefix,_suffix );
}

QString KSpreadStyle::saveOasisStyleNumericDate( KoGenStyles&mainStyles, FormatType _style )
{
    QString format;
    bool locale = false;
    switch( _style )
    {
        //TODO fixme use locale of kspread and not kglobal
    case ShortDate_format:
        format = KGlobal::locale()->dateFormatShort();
        locale = true;
        break;
    case TextDate_format:
        format = KGlobal::locale()->dateFormat();
        locale = true;
        break;
    case date_format1:
        format = "dd-MMM-yy";
        break;
    case date_format2:
        format = "dd-MMM-yyyy";
        break;
    case date_format3:
        format = "dd-M";
        break;
    case date_format4:
        format = "dd-MM";
        break;
    case date_format5:
        format = "dd/MM/yy";
        break;
    case date_format6:
        format = "dd/MM/yyyy";
        break;
    case date_format7:
        format = "MMM-yy";
        break;
    case date_format8:
        format = "MMMM-yy";
        break;
    case date_format9:
        format = "MMMM-yyyy";
        break;
    case date_format10:
        format = "MMMMM-yy";
        break;
    case date_format11:
        format = "dd/MMM";
        break;
    case date_format12:
        format = "dd/MM";
        break;
    case date_format13:
        format = "dd/MMM/yyyy";
        break;
    case date_format14:
        format = "yyyy/MMM/dd";
        break;
    case date_format15:
        format = "yyyy-MMM-dd";
        break;
    case date_format16:
        format = "yyyy/MM/dd";
        break;
    case date_format17:
        format = "d MMMM yyyy";
        break;
    case date_format18:
        format = "MM/dd/yyyy";
        break;
    case date_format19:
        format = "MM/dd/yy";
        break;
    case date_format20:
        format = "MMM/dd/yy";
        break;
    case date_format21:
        format = "MMM/dd/yyyy";
        break;
    case date_format22:
        format = "MMM-yyyy";
        break;
    case date_format23:
        format = "yyyy";
        break;
    case date_format24:
        format = "yy";
        break;
    case date_format25:
        format = "yyyy/MM/dd";
        break;
    case date_format26:
        format = "yyyy/MMM/dd";
        break;
    default:
        kdDebug()<<"this date format is not defined ! :"<<_style<<endl;
        break;
    }
    return KoOasisStyles::saveOasisDateStyle( mainStyles, format, locale );
}

QString KSpreadStyle::saveOasisStyleNumericCustom( KoGenStyles&mainStyles, FormatType _style )
{
    //TODO
    //<number:date-style style:name="N50" style:family="data-style" number:automatic-order="true" number:format-source="language">
    //<number:month/>
    //<number:text>/</number:text>
    //<number:day/>
    //<number:text>/</number:text>
    //<number:year/>
    //<number:text> </number:text>
    //<number:hours number:style="long"/>
    //<number:text>:</number:text>
    //<number:minutes number:style="long"/>
    // <number:text> </number:text>
    //<number:am-pm/>
    //</number:date-style>
    return "";
}

QString KSpreadStyle::saveOasisStyleNumericTime( KoGenStyles& mainStyles, FormatType _style )
{
    //<number:time-style style:name="N42" style:family="data-style">
    //<number:hours number:style="long"/>
    //<number:text>:</number:text>
    //<number:minutes number:style="long"/>
    //<number:text> </number:text>
    //<number:am-pm/>
    //</number:time-style>

    QString format;
    bool locale = false;
    switch( _style )
    {
    case Time_format: //TODO FIXME
        //format = ;
        break;
    case SecondeTime_format: //TODO FIXME
        //format = ;
        break;
    case Time_format1:
        format = "h:mm AP";
        break;
    case Time_format2:
        format = "h:mm:ss AP";
        break;
    case Time_format3: // 9 h 01 min 28 s
        format = "hh \\h mm \\m\\i\\n ss \\s";
        break;
    case Time_format4:
        format = "hh:mm";
        break;
    case Time_format5:
        format = "hh:mm:ss";
        break;
    case Time_format6:
        format = "m:ss";
        break;
    case Time_format7:
        format = "h:mm:ss";
        break;
    case Time_format8:
        format = "h:mm";
        break;
    default:
        kdDebug()<<"time format not defined :"<<_style<<endl;
        break;
    }
    return KoOasisStyles::saveOasisTimeStyle( mainStyles, format, locale );
}


QString KSpreadStyle::saveOasisStyleNumericFraction( KoGenStyles &mainStyles, FormatType _style, const QString &_prefix, const QString _suffix )
{
    //<number:number-style style:name="N71" style:family="data-style">
    //<number:fraction number:min-integer-digits="0" number:min-numerator-digits="2" number:min-denominator-digits="2"/>
    //</number:number-style>
    QString format;
    switch( _style )
    {
    case fraction_half:
        format = "# ?/2";
        break;
    case fraction_quarter:
        format = "# ?/4";
        break;
    case fraction_eighth:
        format = "# ?/8";
        break;
    case fraction_sixteenth:
        format = "# ?/16";
        break;
    case fraction_tenth:
        format = "# ?/10";
        break;
    case fraction_hundredth:
        format = "# ?/100";
        break;
    case fraction_one_digit:
        format = "# ?/?";
        break;
    case fraction_two_digits:
        format = "# \?\?/\?\?";
        break;
    case fraction_three_digits:
        format = "# \?\?\?/\?\?\?";
        break;
    default:
        kdDebug()<<" fraction format not defined :"<<_style<<endl;
        break;
    }

    return KoOasisStyles::saveOasisFractionStyle( mainStyles, format,_prefix, _suffix );
}


QString KSpreadStyle::saveOasisStyle( KoGenStyle &style, KoGenStyles &mainStyles )
{
    if ( featureSet( SAlignX ) && alignX() != KSpreadFormat::Undefined )
    {
        QString value ="start";
        switch( alignX() )
        {
        case KSpreadFormat::Center:
            value = "center";
            break;
        case KSpreadFormat::Right:
            value = "end";
            break;
        case KSpreadFormat::Left:
            value = "start";
            break;
        }
        style.addProperty( "fo:text-align", value );
    }

    if ( featureSet( SAlignY ) && alignY() != KSpreadFormat::Middle )
    {
        style.addProperty( "style:vertical-align", ( alignY() == KSpreadFormat::Bottom ? "bottom" : "top" ) );
    }
    if ( featureSet( SBackgroundColor ) && m_bgColor != QColor() && m_bgColor.isValid() )
        style.addProperty( "fo:background-color", m_bgColor.name() );

    if ( featureSet( SMultiRow ) && hasProperty( PMultiRow ) )
        style.addProperty( "fo:wrap-option", "wrap" );
    if ( featureSet( SVerticalText ) && hasProperty( PVerticalText ) )
    {
        style.addProperty( "fo:direction", "ttb" );
        style.addProperty( "style:rotation-angle", "0" );
    }
#if 0
    if ( featureSet( SFloatFormat ) )
        format.setAttribute( "float", (int) m_floatFormat );

    if ( featureSet( SFloatColor ) )
        format.setAttribute( "floatcolor", (int)m_floatColor );

    if ( featureSet( SFormatType ) )
        format.setAttribute( "format",(int) m_formatType );

    if ( featureSet( SCustomFormat ) && !strFormat().isEmpty() )
        format.setAttribute( "custom", m_strFormat );

    if ( featureSet( SFormatType ) && formatType() == KSpreadFormat::Money )
    {
        format.setAttribute( "type", (int) m_currency.type );
        format.setAttribute( "symbol", m_currency.symbol );
    }
#endif
    if ( featureSet( SAngle ) )
        style.addProperty( "style:rotation-angle", QString::number( -1.0 *m_rotateAngle  ) );

    if ( featureSet( SIndent ) )
    {
        style.addPropertyPt("fo:margin-left", m_indent );
        //FIXME
        //if ( a == KSpreadFormat::Undefined )
        //currentCellStyle.addProperty("fo:text-align", "start" );
    }
    if ( featureSet( SDontPrintText ) && hasProperty( PDontPrintText ) )
        style.addProperty( "style:print-content", "false");

    bool hideAll = false;
    bool hideFormula = false;
    bool isNotProtected = false;

    if ( featureSet( SNotProtected ) && hasProperty( PNotProtected ) )
        isNotProtected = true;

    if ( featureSet( SHideAll ) && hasProperty( PHideAll ) )
        hideAll=true;

    if ( featureSet( SHideFormula ) && hasProperty( PHideFormula ) )
        hideFormula = true;

    if ( hideAll )
        style.addProperty( "style:cell-protect", "hidden-and-protected" );
    else
    {
        if ( isNotProtected && !hideFormula )
            style.addProperty( "style:cell-protect", "none" );
        else
        {
            if ( isNotProtected && hideFormula )
                style.addProperty( "style:cell-protect", "formula-hidden" );
            else if ( hideFormula )
                style.addProperty( "style:cell-protect", "protected formula-hidden" );
            else if ( !isNotProtected )
                style.addProperty( "style:cell-protect", "protected" );
        }
    }

    if ( featureSet( SLeftBorder ) &&featureSet( SRightBorder ) &&
         featureSet( STopBorder ) && featureSet( SBottomBorder ) &&
        ( m_leftBorderPen == m_topBorderPen )&&
         ( m_leftBorderPen == m_rightBorderPen )&&
         ( m_leftBorderPen == m_bottomBorderPen ) )
    {
        if ( ( m_leftBorderPen.width() != 0 ) && ( m_leftBorderPen.style() != Qt::NoPen ) )
            style.addProperty("fo:border", convertOasisPenToString( m_leftBorderPen ) );
    }
    else
    {
        if ( featureSet( SLeftBorder ) &&
             ( ( m_leftBorderPen.width() != 0 ) && ( m_leftBorderPen.style() != Qt::NoPen ) ) )
            style.addProperty( "fo:border-left", convertOasisPenToString( m_leftBorderPen ) );

        if ( featureSet( SRightBorder ) &&
             ( ( m_rightBorderPen.width() != 0 ) && ( m_rightBorderPen.style() != Qt::NoPen ) ) )
            style.addProperty( "fo:border-right", convertOasisPenToString( m_rightBorderPen ) );

        if ( featureSet( STopBorder ) &&
             ( ( m_topBorderPen.width() != 0 ) && ( m_topBorderPen.style() != Qt::NoPen ) ) )
            style.addProperty( "fo:border-top", convertOasisPenToString( m_topBorderPen ) );

        if ( featureSet( SBottomBorder ) &&
             ( m_bottomBorderPen.width() != 0 ) && ( m_bottomBorderPen.style() != Qt::NoPen ) )
            style.addProperty( "fo:border-bottom", convertOasisPenToString( m_bottomBorderPen ) );
    }
    if ( featureSet( SFallDiagonal ) &&
         ( ( m_fallDiagonalPen.width() != 0 ) && ( m_fallDiagonalPen.style() != Qt::NoPen ) ) )
    {
        style.addProperty("style:diagonal-tl-br", convertOasisPenToString( m_fallDiagonalPen ) );
    }
    if ( featureSet( SGoUpDiagonal ) &&
         ( ( m_goUpDiagonalPen.width() != 0 ) && ( m_goUpDiagonalPen.style() != Qt::NoPen ) ))
    {
        style.addProperty("style:diagonal-bl-tr", convertOasisPenToString(m_goUpDiagonalPen ) );
    }
    if ( featureSet( SFontFamily ) )
    {
        style.addProperty("style:font-name", m_fontFamily );
    }
    if ( featureSet( SFontSize ) )
    {
        style.addProperty("fo:font-size",m_fontSize  );
    }
    if ( featureSet( STextPen ) && m_textPen.color().isValid() )
    {
        style.addProperty("fo:color", m_textPen.color().name() );
    }
#if 0
    if ( featureSet( SFontFlag ) )
        format.setAttribute( "font-flags", m_fontFlags );

    //  if ( featureSet( SFont ) )
    //    format.appendChild( util_createElement( "font", m_textFont, doc ) );
#endif

    if ( featureSet( SBackgroundBrush ) )
    {
        QString tmp = saveOasisBackgroundStyle( mainStyles, m_backGroundBrush );
        if ( !tmp.isEmpty() )
            style.addProperty("draw:style-name", tmp );
    }
    QString _prefix;
    QString _postfix;
    int _precision = -1;
    if ( featureSet( SPrefix ) && !prefix().isEmpty() )
        _prefix = m_prefix;

    if ( featureSet( SPostfix ) && !postfix().isEmpty() )
        _postfix = m_postfix;
    if ( featureSet( SPrecision ) )
        _precision =  m_precision;

    return saveOasisStyleNumeric( mainStyles, m_formatType, _prefix, _postfix, _precision );
}

QString KSpreadStyle::saveOasisBackgroundStyle( KoGenStyles &mainStyles, const QBrush &brush )
{
    KoGenStyle styleobjectauto = KoGenStyle( KoGenStyle::STYLE_GRAPHICAUTO, "graphic" );
    KoOasisStyles::saveOasisFillStyle( styleobjectauto, mainStyles, brush );
    return mainStyles.lookup( styleobjectauto, "gr" );
}

void KSpreadStyle::saveXML( QDomDocument & doc, QDomElement & format ) const
{
  if ( featureSet( SAlignX ) && alignX() != KSpreadFormat::Undefined )
    format.setAttribute( "alignX", (int) m_alignX );

  if ( featureSet( SAlignY ) && alignY() != KSpreadFormat::Middle )
    format.setAttribute( "alignY", (int) m_alignY );

  if ( featureSet( SBackgroundColor ) && m_bgColor != QColor() && m_bgColor.isValid() )
    format.setAttribute( "bgcolor", m_bgColor.name() );

  if ( featureSet( SMultiRow ) && hasProperty( PMultiRow ) )
    format.setAttribute( "multirow", "yes" );

  if ( featureSet( SVerticalText ) && hasProperty( PVerticalText ) )
    format.setAttribute( "verticaltext", "yes" );

  if ( featureSet( SPrecision ) )
    format.setAttribute( "precision", m_precision );

  if ( featureSet( SPrefix ) && !prefix().isEmpty() )
    format.setAttribute( "prefix", m_prefix );

  if ( featureSet( SPostfix ) && !postfix().isEmpty() )
    format.setAttribute( "postfix", m_postfix );

  if ( featureSet( SFloatFormat ) )
    format.setAttribute( "float", (int) m_floatFormat );

  if ( featureSet( SFloatColor ) )
  format.setAttribute( "floatcolor", (int)m_floatColor );

  if ( featureSet( SFormatType ) )
    format.setAttribute( "format",(int) m_formatType );

  if ( featureSet( SCustomFormat ) && !strFormat().isEmpty() )
    format.setAttribute( "custom", m_strFormat );

  if ( featureSet( SFormatType ) && formatType() == Money_format )
  {
    format.setAttribute( "type", (int) m_currency.type );
    format.setAttribute( "symbol", m_currency.symbol );
  }

  if ( featureSet( SAngle ) )
    format.setAttribute( "angle", m_rotateAngle );

  if ( featureSet( SIndent ) )
    format.setAttribute( "indent", m_indent );

  if ( featureSet( SDontPrintText ) && hasProperty( PDontPrintText ) )
    format.setAttribute( "dontprinttext", "yes" );

  if ( featureSet( SNotProtected ) && hasProperty( PNotProtected ) )
    format.setAttribute( "noprotection", "yes" );

  if ( featureSet( SHideAll ) && hasProperty( PHideAll ) )
    format.setAttribute( "hideall", "yes" );

  if ( featureSet( SHideFormula ) && hasProperty( PHideFormula ) )
    format.setAttribute( "hideformula", "yes" );

  if ( featureSet( SFontFamily ) )
    format.setAttribute( "font-family", m_fontFamily );
  if ( featureSet( SFontSize ) )
    format.setAttribute( "font-size", m_fontSize );
  if ( featureSet( SFontFlag ) )
    format.setAttribute( "font-flags", m_fontFlags );

  //  if ( featureSet( SFont ) )
  //    format.appendChild( util_createElement( "font", m_textFont, doc ) );

  if ( featureSet( STextPen ) && m_textPen.color().isValid() )
    format.appendChild( util_createElement( "pen", m_textPen, doc ) );

  if ( featureSet( SBackgroundBrush ) )
  {
    format.setAttribute( "brushcolor", m_backGroundBrush.color().name() );
    format.setAttribute( "brushstyle", (int) m_backGroundBrush.style() );
  }

  if ( featureSet( SLeftBorder ) )
  {
    QDomElement left = doc.createElement( "left-border" );
    left.appendChild( util_createElement( "pen", m_leftBorderPen, doc ) );
    format.appendChild( left );
  }

  if ( featureSet( STopBorder ) )
  {
    QDomElement top = doc.createElement( "top-border" );
    top.appendChild( util_createElement( "pen", m_topBorderPen, doc ) );
    format.appendChild( top );
  }

  if ( featureSet( SRightBorder ) )
  {
    QDomElement right = doc.createElement( "right-border" );
    right.appendChild( util_createElement( "pen", m_rightBorderPen, doc ) );
    format.appendChild( right );
  }

  if ( featureSet( SBottomBorder ) )
  {
    QDomElement bottom = doc.createElement( "bottom-border" );
    bottom.appendChild( util_createElement( "pen", m_bottomBorderPen, doc ) );
    format.appendChild( bottom );
  }

  if ( featureSet( SFallDiagonal ) )
  {
    QDomElement fallDiagonal  = doc.createElement( "fall-diagonal" );
    fallDiagonal.appendChild( util_createElement( "pen", m_fallDiagonalPen, doc ) );
    format.appendChild( fallDiagonal );
  }

  if ( featureSet( SGoUpDiagonal ) )
  {
    QDomElement goUpDiagonal = doc.createElement( "up-diagonal" );
    goUpDiagonal.appendChild( util_createElement( "pen", m_goUpDiagonalPen, doc ) );
    format.appendChild( goUpDiagonal );
  }
}

bool KSpreadStyle::loadXML( QDomElement & format )
{
  bool ok;
  if ( format.hasAttribute( "type" ) )
  {
    m_type = (StyleType) format.attribute( "type" ).toInt( &ok );
    if ( !ok )
      return false;
  }

  if ( format.hasAttribute( "alignX" ) )
  {
    KSpreadFormat::Align a = (KSpreadFormat::Align) format.attribute( "alignX" ).toInt( &ok );
    if ( !ok )
      return false;
    if ( (unsigned int) a >= 1 || (unsigned int) a <= 4 )
    {
      m_alignX = a;
      m_featuresSet |= SAlignX;
    }
  }
  if ( format.hasAttribute( "alignY" ) )
  {
    KSpreadFormat::AlignY a = (KSpreadFormat::AlignY) format.attribute( "alignY" ).toInt( &ok );
    if ( !ok )
      return false;
    if ( (unsigned int) a >= 1 || (unsigned int) a < 4 )
    {
      m_alignY = a;
      m_featuresSet |= SAlignY;
    }
  }

  if ( format.hasAttribute( "bgcolor" ) )
  {
    m_bgColor = QColor( format.attribute( "bgcolor" ) );
    m_featuresSet |= SBackgroundColor;
  }

  if ( format.hasAttribute( "multirow" ) )
  {
    setProperty( PMultiRow );
    m_featuresSet |= SMultiRow;
  }

  if ( format.hasAttribute( "verticaltext" ) )
  {
    setProperty( PVerticalText );
    m_featuresSet |= SVerticalText;
  }

  if ( format.hasAttribute( "precision" ) )
  {
    int i = format.attribute( "precision" ).toInt( &ok );
    if ( i < -1 )
    {
      kdDebug(36001) << "Value out of range Cell::precision=" << i << endl;
      return false;
    }
    m_precision = i;
    m_featuresSet |= SPrecision;
  }

  if ( format.hasAttribute( "float" ) )
  {
    KSpreadFormat::FloatFormat a = (KSpreadFormat::FloatFormat)format.attribute( "float" ).toInt( &ok );
    if ( !ok )
      return false;
    if ( (unsigned int) a >= 1 || (unsigned int) a <= 3 )
    {
      m_floatFormat = a;
      m_featuresSet |= SFloatFormat;
    }
  }

  if ( format.hasAttribute( "floatcolor" ) )
  {
    KSpreadFormat::FloatColor a = (KSpreadFormat::FloatColor) format.attribute( "floatcolor" ).toInt( &ok );
    if ( !ok ) return false;
    if ( (unsigned int) a >= 1 || (unsigned int) a <= 2 )
    {
      m_floatColor = a;
      m_featuresSet |= SFloatColor;
    }
  }

  if ( format.hasAttribute( "format" ) )
  {
    int fo = format.attribute( "format" ).toInt( &ok );
    if ( ! ok )
      return false;
    m_formatType = ( FormatType ) fo;
    m_featuresSet |= SFormatType;
  }
  if ( format.hasAttribute( "custom" ) )
  {
    m_strFormat = format.attribute( "custom" );
    m_featuresSet |= SCustomFormat;
  }
  if ( m_formatType == Money_format )
  {
    if ( format.hasAttribute( "type" ) )
    {
      m_currency.type   = format.attribute( "type" ).toInt( &ok );
      if (!ok)
        m_currency.type = 1;
    }
    if ( format.hasAttribute( "symbol" ) )
    {
      m_currency.symbol = format.attribute( "symbol" );
    }
    m_featuresSet |= SFormatType;
  }
  if ( format.hasAttribute( "angle" ) )
  {
    m_rotateAngle = format.attribute( "angle" ).toInt( &ok );
    if ( !ok )
      return false;
    m_featuresSet |= SAngle;
  }
  if ( format.hasAttribute( "indent" ) )
  {
    m_indent = format.attribute( "indent" ).toDouble( &ok );
    if ( !ok )
      return false;
    m_featuresSet |= SIndent;
  }
  if ( format.hasAttribute( "dontprinttext" ) )
  {
    setProperty( PDontPrintText );
    m_featuresSet |= SDontPrintText;
  }

  if ( format.hasAttribute( "noprotection" ) )
  {
    setProperty( PNotProtected );
    m_featuresSet |= SNotProtected;
  }

  if ( format.hasAttribute( "hideall" ) )
  {
    setProperty( PHideAll );
    m_featuresSet |= SHideAll;
  }

  if ( format.hasAttribute( "hideformula" ) )
  {
    setProperty( PHideFormula );
    m_featuresSet |= SHideFormula;
  }

  // TODO: remove that...
  QDomElement font = format.namedItem( "font" ).toElement();
  if ( !font.isNull() )
  {
    QFont f( util_toFont( font ) );
    m_fontFamily = f.family();
    m_fontSize = f.pointSize();
    if ( f.italic() )
      m_fontFlags |= FItalic;
    if ( f.bold() )
      m_fontFlags |= FBold;
    if ( f.underline() )
      m_fontFlags |= FUnderline;
    if ( f.strikeOut() )
      m_fontFlags |= FStrike;

    m_featuresSet |= SFont;
    m_featuresSet |= SFontFamily;
    m_featuresSet |= SFontFlag;
    m_featuresSet |= SFontSize;
  }

  if ( format.hasAttribute( "font-family" ) )
  {
    m_fontFamily = format.attribute( "font-family" );
    m_featuresSet |= SFont;
    m_featuresSet |= SFontFamily;
  }
  if ( format.hasAttribute( "font-size" ) )
  {
    m_fontSize = format.attribute( "font-size" ).toInt( &ok );
    if ( !ok )
      return false;
    m_featuresSet |= SFont;
    m_featuresSet |= SFontSize;
  }

  if ( format.hasAttribute( "font-flags" ) )
  {
    m_fontFlags = format.attribute( "font-flags" ).toInt( &ok );
    if ( !ok )
      return false;
    m_featuresSet |= SFont;
    m_featuresSet |= SFontFlag;
  }

  if ( format.hasAttribute( "brushcolor" ) )
  {
    m_backGroundBrush.setColor( QColor( format.attribute( "brushcolor" ) ) );
    m_featuresSet |= SBackgroundBrush;
  }

  if ( format.hasAttribute( "brushstyle" ) )
  {
    m_backGroundBrush.setStyle( (Qt::BrushStyle) format.attribute( "brushstyle" ).toInt( &ok )  );
    if ( !ok )
      return false;
    m_featuresSet |= SBackgroundBrush;
  }

  QDomElement pen = format.namedItem( "pen" ).toElement();
  if ( !pen.isNull() )
  {
    m_textPen = util_toPen( pen );
    m_featuresSet |= STextPen;
  }

  QDomElement left = format.namedItem( "left-border" ).toElement();
  if ( !left.isNull() )
  {
    QDomElement pen = left.namedItem( "pen" ).toElement();
    if ( !pen.isNull() )
    {
      m_leftBorderPen = util_toPen( pen );
      m_featuresSet |= SLeftBorder;
    }
  }

  QDomElement top = format.namedItem( "top-border" ).toElement();
  if ( !top.isNull() )
  {
    QDomElement pen = top.namedItem( "pen" ).toElement();
    if ( !pen.isNull() )
    {
      m_topBorderPen = util_toPen( pen );
      m_featuresSet |= STopBorder;
    }
  }

  QDomElement right = format.namedItem( "right-border" ).toElement();
  if ( !right.isNull() )
  {
    QDomElement pen = right.namedItem( "pen" ).toElement();
    if ( !pen.isNull() )
    {
      m_rightBorderPen = util_toPen( pen );
      m_featuresSet |= SRightBorder;
    }
  }

  QDomElement bottom = format.namedItem( "bottom-border" ).toElement();
  if ( !bottom.isNull() )
  {
    QDomElement pen = bottom.namedItem( "pen" ).toElement();
    if ( !pen.isNull() )
    {
      m_bottomBorderPen = util_toPen( pen );
      m_featuresSet |= SBottomBorder;
    }
  }

  QDomElement fallDiagonal = format.namedItem( "fall-diagonal" ).toElement();
  if ( !fallDiagonal.isNull() )
  {
    QDomElement pen = fallDiagonal.namedItem( "pen" ).toElement();
    if ( !pen.isNull() )
    {
      m_fallDiagonalPen = util_toPen( pen );
      m_featuresSet |= SFallDiagonal;
    }
  }

  QDomElement goUpDiagonal = format.namedItem( "up-diagonal" ).toElement();
  if ( !goUpDiagonal.isNull() )
  {
    QDomElement pen = goUpDiagonal.namedItem( "pen" ).toElement();
    if ( !pen.isNull() )
    {
      m_goUpDiagonalPen = util_toPen( pen );
      m_featuresSet |= SGoUpDiagonal;
    }
  }

  if ( format.hasAttribute( "prefix" ) )
  {
    m_prefix = format.attribute( "prefix" );
    m_featuresSet |= SPrefix;
  }
  if ( format.hasAttribute( "postfix" ) )
  {
    m_postfix = format.attribute( "postfix" );
    m_featuresSet |= SPostfix;
  }

  return true;
}

void KSpreadStyle::setParent( KSpreadCustomStyle * parent )
{
  m_parent = parent;
  if ( m_parent )
    m_parentName = m_parent->name();
}

KSpreadCustomStyle * KSpreadStyle::parent() const
{
  return m_parent;
}

bool KSpreadStyle::release()
{
  --m_usageCount;

  if ( m_type == CUSTOM || m_type == BUILTIN )
    return false; // never delete builtin styles...

  if ( m_usageCount < 1 )
    return true;

  return false;
}

void KSpreadStyle::addRef()
{
  ++m_usageCount;
}

bool KSpreadStyle::hasProperty( Properties p ) const
{
  FlagsSet f;
  switch( p )
  {
   case PDontPrintText:
    f = SDontPrintText;
    break;
   case PCustomFormat:
    f = SCustomFormat;
    break;
   case PNotProtected:
    f = SNotProtected;
    break;
   case PHideAll:
    f = SHideAll;
    break;
   case PHideFormula:
    f = SHideFormula;
    break;
   case PMultiRow:
    f = SMultiRow;
    break;
   case PVerticalText:
    f = SVerticalText;
    break;
   default:
    kdWarning() << "Unhandled property" << endl;
    return ( m_properties  & (uint) p );
  }

  return ( !m_parent || featureSet( f ) ? ( m_properties & (uint) p ) : m_parent->hasProperty( p ) );
}

bool KSpreadStyle::hasFeature( FlagsSet f, bool withoutParent ) const
{
  bool b = ( m_featuresSet & (uint) f );

  // check if feature is defined here or at parent level
  if ( m_parent && !withoutParent )
    b = ( m_parent->hasFeature( f, withoutParent ) ? true : b );

  return b;
}

QFont KSpreadStyle::font() const
{
  QString family = fontFamily();
  int  size      = fontSize();
  uint ff        = fontFlags();

  QFont f( family, size );
  if ( ff & (uint) FBold )
    f.setBold( true );
  if ( ff & (uint) FItalic )
    f.setItalic( true );
  if ( ff & (uint) FUnderline )
    f.setUnderline( true );
  if ( ff & (uint) FStrike )
    f.setStrikeOut( true );

  return f;
}

QString const & KSpreadStyle::fontFamily() const
{
  return ( !m_parent || featureSet( SFontFamily ) ? m_fontFamily : m_parent->fontFamily() );
}

uint KSpreadStyle::fontFlags() const
{
  return ( !m_parent || featureSet( SFontFlag ) ? m_fontFlags : m_parent->fontFlags() );
}

int KSpreadStyle::fontSize() const
{
  return ( !m_parent || featureSet( SFontSize ) ? m_fontSize : m_parent->fontSize() );
}

QPen const & KSpreadStyle::pen() const
{
  return ( !m_parent || featureSet( STextPen ) ? m_textPen : m_parent->pen() );
}

QColor const & KSpreadStyle::bgColor() const
{
  return ( !m_parent || featureSet( SBackgroundColor ) ? m_bgColor : m_parent->bgColor() );
}

QPen const & KSpreadStyle::rightBorderPen() const
{
  return ( !m_parent || featureSet( SRightBorder ) ? m_rightBorderPen : m_parent->rightBorderPen() );
}

QPen const & KSpreadStyle::bottomBorderPen() const
{
  return ( !m_parent || featureSet( SBottomBorder ) ? m_bottomBorderPen : m_parent->bottomBorderPen() );
}

QPen const & KSpreadStyle::leftBorderPen() const
{
  return ( !m_parent || featureSet( SLeftBorder ) ? m_leftBorderPen : m_parent->leftBorderPen() );
}

QPen const & KSpreadStyle::topBorderPen() const
{
  return ( !m_parent || featureSet( STopBorder ) ? m_topBorderPen : m_parent->topBorderPen() );
}

QPen const & KSpreadStyle::fallDiagonalPen() const
{
  return ( !m_parent || featureSet( SFallDiagonal ) ? m_fallDiagonalPen : m_parent->fallDiagonalPen() );
}

QPen const & KSpreadStyle::goUpDiagonalPen() const
{
  return ( !m_parent || featureSet( SGoUpDiagonal ) ? m_goUpDiagonalPen : m_parent->goUpDiagonalPen() );
}

int KSpreadStyle::precision() const
{
  return ( !m_parent || featureSet( SPrecision ) ? m_precision : m_parent->precision() );
}

int KSpreadStyle::rotateAngle() const
{
  return ( !m_parent || featureSet( SAngle ) ? m_rotateAngle : m_parent->rotateAngle() );
}

double KSpreadStyle::indent() const
{
  return ( !m_parent || featureSet( SIndent ) ? m_indent : m_parent->indent() );
}

QBrush const & KSpreadStyle::backGroundBrush() const
{
  return ( !m_parent || featureSet( SBackgroundBrush ) ? m_backGroundBrush : m_parent->backGroundBrush() );
}

KSpreadFormat::Align KSpreadStyle::alignX() const
{
  return ( !m_parent || featureSet( SAlignX ) ? m_alignX : m_parent->alignX() );
}

KSpreadFormat::AlignY KSpreadStyle::alignY() const
{
  return ( !m_parent || featureSet( SAlignY ) ? m_alignY : m_parent->alignY() );
}

KSpreadFormat::FloatFormat KSpreadStyle::floatFormat() const
{
  return ( !m_parent || featureSet( SFloatFormat ) ? m_floatFormat : m_parent->floatFormat() );
}

KSpreadFormat::FloatColor KSpreadStyle::floatColor() const
{
  return ( !m_parent || featureSet( SFloatColor ) ? m_floatColor : m_parent->floatColor() );
}

FormatType KSpreadStyle::formatType() const
{
  return ( !m_parent || featureSet( SFormatType ) ? m_formatType : m_parent->formatType() );
}

KSpreadFormat::Currency const & KSpreadStyle::currency() const
{
  return ( !m_parent || featureSet( SFormatType ) ? m_currency : m_parent->currency() );
}

QString const & KSpreadStyle::strFormat() const
{
  return ( !m_parent || featureSet( SCustomFormat ) ? m_strFormat : m_parent->strFormat() );
}

QString const & KSpreadStyle::prefix() const
{
  return ( !m_parent || featureSet( SPrefix ) ? m_prefix : m_parent->prefix() );
}

QString const & KSpreadStyle::postfix() const
{
  return ( !m_parent || featureSet( SPostfix ) ? m_postfix : m_parent->postfix() );
}



KSpreadStyle * KSpreadStyle::setAlignX( KSpreadFormat::Align alignX )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    KSpreadStyle * style = new KSpreadStyle( this );
    style->m_alignX = alignX;
    style->m_featuresSet |= SAlignX;
    return style;
  }

  m_alignX      = alignX;
  m_featuresSet |= SAlignX;
  return this;
}

KSpreadStyle * KSpreadStyle::setAlignY( KSpreadFormat::AlignY alignY )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    KSpreadStyle * style = new KSpreadStyle( this );
    style->m_alignY = alignY;
    style->m_featuresSet |= SAlignY;
    return style;
  }

  m_alignY = alignY;
  m_featuresSet |= SAlignY;
  return this;
}

KSpreadStyle * KSpreadStyle::setFont( QFont const & f )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    KSpreadStyle * style = new KSpreadStyle( this );
    if ( style->m_fontFamily != f.family() )
    {
      style->m_fontFamily = f.family();
      style->m_featuresSet |= SFont;
      style->m_featuresSet |= SFontFamily;
    }
    if ( style->m_fontSize != f.pointSize() )
    {
      style->m_fontSize = f.pointSize();
      style->m_featuresSet |= SFont;
      style->m_featuresSet |= SFontSize;
    }
    if ( f.italic() != (m_fontFlags & (uint) FItalic ) )
    {
      if ( f.italic() )
        style->m_fontFlags |= FItalic;
      else
        style->m_fontFlags &= ~(uint) FItalic;
      style->m_featuresSet |= SFont;
      style->m_featuresSet |= SFontFlag;
    }
    if ( f.bold() != (m_fontFlags & (uint) FBold ) )
    {
      if ( f.bold() )
        style->m_fontFlags |= FBold;
      else
        style->m_fontFlags &= ~(uint) FBold;
      style->m_featuresSet |= SFont;
      style->m_featuresSet |= SFontFlag;
    }
    if ( f.underline() != (m_fontFlags & (uint) FUnderline ) )
    {
      if ( f.underline() )
        style->m_fontFlags |= FUnderline;
      else
        style->m_fontFlags &= ~(uint) FUnderline;
      style->m_featuresSet |= SFont;
      style->m_featuresSet |= SFontFlag;
    }
    if ( f.strikeOut() != (m_fontFlags & (uint) FStrike ) )
    {
      if ( f.strikeOut() )
        style->m_fontFlags |= FStrike;
      else
        style->m_fontFlags &= ~(uint) FStrike;
      style->m_featuresSet |= SFont;
      style->m_featuresSet |= SFontFlag;
    }

    return style;
  }

  if ( m_fontFamily != f.family() )
  {
    m_fontFamily = f.family();
    m_featuresSet |= SFont;
    m_featuresSet |= SFontFamily;
  }
  if ( m_fontSize != f.pointSize() )
  {
    m_fontSize = f.pointSize();
    m_featuresSet |= SFont;
    m_featuresSet |= SFontSize;
  }
  if ( f.italic() != (m_fontFlags & (uint) FItalic ) )
  {
    if ( f.italic() )
      m_fontFlags |= FItalic;
    else
      m_fontFlags &= ~(uint) FItalic;
    m_featuresSet |= SFont;
    m_featuresSet |= SFontFlag;
  }
  if ( f.bold() != (m_fontFlags & (uint) FBold ) )
  {
    if ( f.bold() )
      m_fontFlags |= FBold;
    else
      m_fontFlags &= ~(uint) FBold;
    m_featuresSet |= SFont;
    m_featuresSet |= SFontFlag;
  }
  if ( f.underline() != (m_fontFlags & (uint) FUnderline ) )
  {
    if ( f.underline() )
      m_fontFlags |= FUnderline;
    else
      m_fontFlags &= ~(uint) FUnderline;
    m_featuresSet |= SFont;
    m_featuresSet |= SFontFlag;
  }
  if ( f.strikeOut() != (m_fontFlags & (uint) FStrike ) )
  {
    if ( f.strikeOut() )
      m_fontFlags |= FStrike;
    else
      m_fontFlags &= ~(uint) FStrike;
    m_featuresSet |= SFont;
    m_featuresSet |= SFontFlag;
  }

  return this;
}

KSpreadStyle * KSpreadStyle::setFontFamily( QString const & fam )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    if ( m_fontFamily != fam )
    {
      KSpreadStyle * style  = new KSpreadStyle( this );
      style->m_fontFamily   = fam;
      style->m_featuresSet |= SFontFamily;
      style->m_featuresSet |= SFont;
      return style;
    }
    return this;
  }

  m_fontFamily   = fam;
  m_featuresSet |= SFont;
  m_featuresSet |= SFontFamily;
  return this;
}

KSpreadStyle * KSpreadStyle::setFontFlags( uint flags )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    if ( m_fontFlags != flags )
    {
      KSpreadStyle * style = new KSpreadStyle( this );
      style->m_fontFlags = flags;
      style->m_featuresSet |= SFontFlag;
      style->m_featuresSet |= SFont;
      return style;
    }
    return this;
  }

  m_fontFlags    = flags;
  m_featuresSet |= SFont;
  m_featuresSet |= SFontFlag;
  return this;
}

KSpreadStyle * KSpreadStyle::setFontSize( int size )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    if ( m_fontSize != size )
    {
      KSpreadStyle * style  = new KSpreadStyle( this );
      style->m_fontSize     = size;
      style->m_featuresSet |= SFontSize;
      style->m_featuresSet |= SFont;
      return style;
    }
    return this;
  }

  m_fontSize     = size;
  m_featuresSet |= SFont;
  m_featuresSet |= SFontSize;
  return this;
}

KSpreadStyle * KSpreadStyle::setPen( QPen const & pen )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    KSpreadStyle * style = new KSpreadStyle( this );
    style->m_textPen = pen;
    style->m_featuresSet |= STextPen;
    return style;
  }

  m_textPen = pen;
  m_featuresSet |= STextPen;
  return this;
}

KSpreadStyle * KSpreadStyle::setBgColor( QColor const & color )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    KSpreadStyle * style = new KSpreadStyle( this );
    style->m_bgColor = color;
    style->m_featuresSet |= SBackgroundColor;
    return style;
  }

  m_bgColor = color;
  m_featuresSet |= SBackgroundColor;
  return this;
}

KSpreadStyle * KSpreadStyle::setRightBorderPen( QPen const & pen )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    KSpreadStyle * style = new KSpreadStyle( this );
    style->m_rightBorderPen = pen;
    style->m_rightPenValue = calculateValue( pen );
    style->m_featuresSet |= SRightBorder;
    return style;
  }

  m_rightBorderPen = pen;
  m_rightPenValue = calculateValue( pen );
  m_featuresSet |= SRightBorder;
  return this;
}

KSpreadStyle * KSpreadStyle::setBottomBorderPen( QPen const & pen )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    KSpreadStyle * style = new KSpreadStyle( this );
    style->m_bottomBorderPen = pen;
    style->m_bottomPenValue = calculateValue( pen );
    style->m_featuresSet |= SBottomBorder;
    return style;
  }

  m_bottomBorderPen = pen;
  m_bottomPenValue = calculateValue( pen );
  m_featuresSet |= SBottomBorder;
  return this;
}

KSpreadStyle * KSpreadStyle::setLeftBorderPen( QPen const & pen )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    KSpreadStyle * style = new KSpreadStyle( this );
    style->m_leftBorderPen = pen;
    style->m_leftPenValue = calculateValue( pen );
    style->m_featuresSet |= SLeftBorder;
    return style;
  }

  m_leftBorderPen = pen;
  m_leftPenValue = calculateValue( pen );
  m_featuresSet |= SLeftBorder;
  return this;
}

KSpreadStyle * KSpreadStyle::setTopBorderPen( QPen const & pen )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    KSpreadStyle * style = new KSpreadStyle( this );
    style->m_topBorderPen = pen;
    style->m_topPenValue = calculateValue( pen );
    style->m_featuresSet |= STopBorder;
    return style;
  }

  m_topBorderPen = pen;
  m_topPenValue = calculateValue( pen );
  m_featuresSet |= STopBorder;
  return this;
}

KSpreadStyle * KSpreadStyle::setFallDiagonalPen( QPen const & pen )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    KSpreadStyle * style = new KSpreadStyle( this );
    style->m_fallDiagonalPen = pen;
    style->m_featuresSet |= SFallDiagonal;
    return style;
  }

  m_fallDiagonalPen = pen;
  m_featuresSet |= SFallDiagonal;
  return this;
}

KSpreadStyle * KSpreadStyle::setGoUpDiagonalPen( QPen const & pen )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    KSpreadStyle * style = new KSpreadStyle( this );
    style->m_goUpDiagonalPen = pen;
    style->m_featuresSet |= SGoUpDiagonal;
    return style;
  }

  m_goUpDiagonalPen = pen;
  m_featuresSet |= SGoUpDiagonal;
  return this;
}

KSpreadStyle * KSpreadStyle::setRotateAngle( int angle )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    KSpreadStyle * style = new KSpreadStyle( this );
    style->m_rotateAngle = angle;
    style->m_featuresSet |= SAngle;
    return style;
  }

  m_rotateAngle = angle;
  m_featuresSet |= SAngle;
  return this;
}

KSpreadStyle * KSpreadStyle::setIndent( double indent )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    KSpreadStyle * style = new KSpreadStyle( this );
    style->m_indent = indent;
    style->m_featuresSet |= SIndent;
    return style;
  }

  m_indent = indent;
  m_featuresSet |= SIndent;
  return this;
}

KSpreadStyle * KSpreadStyle::setBackGroundBrush( QBrush const & brush )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    KSpreadStyle * style = new KSpreadStyle( this );
    style->m_backGroundBrush = brush;
    style->m_featuresSet |= SBackgroundBrush;
    return style;
  }

  m_backGroundBrush = brush;
  m_featuresSet |= SBackgroundBrush;
  return this;
}

KSpreadStyle * KSpreadStyle::setFloatFormat( KSpreadFormat::FloatFormat format )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    KSpreadStyle * style = new KSpreadStyle( this );
    style->m_floatFormat = format;
    style->m_featuresSet |= SFloatFormat;
    return style;
  }

  m_floatFormat = format;
  m_featuresSet |= SFloatFormat;
  return this;
}

KSpreadStyle * KSpreadStyle::setFloatColor( KSpreadFormat::FloatColor color )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    KSpreadStyle * style = new KSpreadStyle( this );
    style->m_floatColor = color;
    style->m_featuresSet |= SFloatColor;
    return style;
  }

  m_floatColor = color;
  m_featuresSet |= SFloatColor;
  return this;
}

KSpreadStyle * KSpreadStyle::setStrFormat( QString const & strFormat )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    KSpreadStyle * style = new KSpreadStyle( this );
    style->m_strFormat = strFormat;
    style->m_featuresSet |= SCustomFormat;
    return style;
  }

  m_strFormat = strFormat;
  m_featuresSet |= SCustomFormat;
  return this;
}

KSpreadStyle * KSpreadStyle::setPrecision( int precision )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    KSpreadStyle * style = new KSpreadStyle( this );
    style->m_precision = precision;
    style->m_featuresSet |= SPrecision;
    return style;
  }

  m_precision = precision;
  m_featuresSet |= SPrecision;
  return this;
}

KSpreadStyle * KSpreadStyle::setPrefix( QString const & prefix )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    KSpreadStyle * style = new KSpreadStyle( this );
    style->m_prefix = prefix;
    style->m_featuresSet |= SPrefix;
    return style;
  }

  m_prefix = prefix;
  m_featuresSet |= SPrefix;
  return this;
}

KSpreadStyle * KSpreadStyle::setPostfix( QString const & postfix )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    KSpreadStyle * style = new KSpreadStyle( this );
    style->m_postfix = postfix;
    style->m_featuresSet |= SPostfix;
    return style;
  }

  m_postfix = postfix;
  m_featuresSet |= SPostfix;
  return this;
}

KSpreadStyle * KSpreadStyle::setCurrency( KSpreadFormat::Currency const & currency )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    KSpreadStyle * style = new KSpreadStyle( this );
    style->m_currency = currency;
    style->m_featuresSet |= SFormatType;
    return style;
  }

  m_currency = currency;
  m_featuresSet |= SFormatType;
  return this;
}

KSpreadStyle * KSpreadStyle::setProperty( Properties p )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    KSpreadStyle * style = new KSpreadStyle( this );
    style->m_properties |= (uint) p;
    switch( p )
    {
     case PDontPrintText:
      style->m_featuresSet |= SDontPrintText;
      break;
     case PCustomFormat:
      style->m_featuresSet |= SCustomFormat;
      break;
     case PNotProtected:
      style->m_featuresSet |= SNotProtected;
      break;
     case PHideAll:
      style->m_featuresSet |= SHideAll;
      break;
     case PHideFormula:
      style->m_featuresSet |= SHideFormula;
      break;
     case PMultiRow:
      style->m_featuresSet |= SMultiRow;
      break;
     case PVerticalText:
      style->m_featuresSet |= SVerticalText;
      break;
     default:
      kdWarning() << "Unhandled property" << endl;
    }
    return style;
  }

  m_properties |= (uint) p;
  switch( p )
  {
   case PDontPrintText:
    m_featuresSet |= SDontPrintText;
    break;
   case PCustomFormat:
    m_featuresSet |= SCustomFormat;
    break;
   case PNotProtected:
    m_featuresSet |= SNotProtected;
    break;
   case PHideAll:
    m_featuresSet |= SHideAll;
    break;
   case PHideFormula:
    m_featuresSet |= SHideFormula;
    break;
   case PMultiRow:
    m_featuresSet |= SMultiRow;
    break;
   case PVerticalText:
    m_featuresSet |= SVerticalText;
    break;
   default:
    kdWarning() << "Unhandled property" << endl;
  }
  return this;
}

KSpreadStyle * KSpreadStyle::clearProperty( Properties p )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    KSpreadStyle * style = new KSpreadStyle( this );
    style->m_properties &= ~(uint) p;
    switch( p )
    {
     case PDontPrintText:
      style->m_featuresSet |= SDontPrintText;
      break;
     case PCustomFormat:
      style->m_featuresSet |= SCustomFormat;
      break;
     case PNotProtected:
      style->m_featuresSet |= SNotProtected;
      break;
     case PHideAll:
      style->m_featuresSet |= SHideAll;
      break;
     case PHideFormula:
      style->m_featuresSet |= SHideFormula;
      break;
     case PMultiRow:
      style->m_featuresSet |= SMultiRow;
      break;
     case PVerticalText:
      style->m_featuresSet |= SVerticalText;
      break;
     default:
      kdWarning() << "Unhandled property" << endl;
    }
    return style;
  }

  m_properties &= ~(uint) p;
  switch( p )
  {
   case PDontPrintText:
    m_featuresSet |= SDontPrintText;
    break;
   case PCustomFormat:
    m_featuresSet |= SCustomFormat;
    break;
   case PNotProtected:
    m_featuresSet |= SNotProtected;
    break;
   case PHideAll:
    m_featuresSet |= SHideAll;
    break;
   case PHideFormula:
    m_featuresSet |= SHideFormula;
    break;
   case PMultiRow:
    m_featuresSet |= SMultiRow;
    break;
   case PVerticalText:
    m_featuresSet |= SVerticalText;
    break;
   default:
    kdWarning() << "Unhandled property" << endl;
  }
  return this;
}


KSpreadStyle * KSpreadStyle::setFormatType( FormatType format )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    KSpreadStyle * style  = new KSpreadStyle( this );
    style->m_formatType  = format;
    style->m_featuresSet |= SFormatType;
    return style;
  }

  m_formatType  = format;
  m_featuresSet |= SFormatType;
  return this;
}


/**
 * ************************************************************
 * KSpreadCustomStyle
 * ************************************************************
 */

KSpreadCustomStyle::KSpreadCustomStyle()
  : KSpreadStyle(),
    m_name( i18n("Default") )
{
  m_parent = 0;
}

KSpreadCustomStyle::KSpreadCustomStyle( KSpreadStyle * parent, QString const & name )
  : KSpreadStyle(),
    m_name( name )
{
  m_type   = CUSTOM;
  m_parent = 0;

  // one to one copy
  if ( parent->hasProperty( PDontPrintText ) )
    addProperty( PDontPrintText );
  if ( parent->hasProperty( PCustomFormat ) )
    addProperty( PCustomFormat );
  if ( parent->hasProperty( PNotProtected ) )
    addProperty( PNotProtected );
  if ( parent->hasProperty( PHideAll ) )
    addProperty( PHideAll );
  if ( parent->hasProperty( PHideFormula ) )
    addProperty( PHideFormula );
  if ( parent->hasProperty( PMultiRow ) )
    addProperty( PMultiRow );
  if ( parent->hasProperty( PVerticalText ) )
    addProperty( PVerticalText );

  changeAlignX( parent->alignX() );
  changeAlignY( parent->alignY() );
  changeFloatFormat( parent->floatFormat() );
  changeFloatColor( parent->floatColor() );
  changeFormatType( parent->formatType() );
  changeFontFamily( parent->fontFamily() );
  changeFontSize( parent->fontSize() );
  changeFontFlags( parent->fontFlags() );
  changePen( parent->pen() );
  changeBgColor( parent->bgColor() );
  changeRightBorderPen( parent->rightBorderPen() );
  changeBottomBorderPen( parent->bottomBorderPen() );
  changeLeftBorderPen( parent->leftBorderPen() );
  changeTopBorderPen( parent->topBorderPen() );
  changeFallBorderPen( parent->fallDiagonalPen() );
  changeGoUpBorderPen( parent->goUpDiagonalPen() );
  changeBackGroundBrush( parent->backGroundBrush() );
  changeRotateAngle( parent->rotateAngle() );
  changeIndent( parent->indent() );
  changeStrFormat( parent->strFormat() );
  changePrecision( parent->precision() );
  changePrefix( parent->prefix() );
  changePostfix( parent->postfix() );
  changeCurrency( parent->currency() );
}

KSpreadCustomStyle::KSpreadCustomStyle( QString const & name, KSpreadCustomStyle * parent )
  : KSpreadStyle(),
    m_name( name )
{
  m_parent = parent;
  if ( m_parent )
    m_parentName = m_parent->name();
}

KSpreadCustomStyle::~KSpreadCustomStyle()
{
}

void KSpreadCustomStyle::saveOasis( KoGenStyles &mainStyles )
{
    if ( m_name.isEmpty() )
        return;
    KoGenStyle gs;
    if ( m_type == AUTO )
        gs = KoGenStyle(KSpreadDoc::STYLE_DEFAULTSTYLE );
    else
        gs = KoGenStyle( KSpreadDoc::STYLE_USERSTYLE ); //FIXME name of style
    if ( m_parent )
        gs.addAttribute( "style:parent-style-name", m_parent->name() );
    gs.addAttribute( "style:display-name", m_name );
    QString numericStyle = saveOasisStyle( gs, mainStyles );
    if ( !numericStyle.isEmpty() )
        gs.addAttribute( "style:data-style-name", numericStyle );
    mainStyles.lookup( gs, "custom-style" );
}

void KSpreadCustomStyle::loadOasis( KoOasisStyles& oasisStyles, const QDomElement & style, const QString & name )
{
    m_name = name;
    if ( style.hasAttributeNS( KoXmlNS::style, "parent-style-name" ) )
        m_parentName = style.attributeNS( KoXmlNS::style, "parent-style-name", QString::null );


    m_type = CUSTOM;

    KSpreadStyle::loadOasisStyle( oasisStyles, style );
}

void KSpreadCustomStyle::save( QDomDocument & doc, QDomElement & styles )
{
  if ( m_name.isEmpty() )
    return;

  QDomElement style( doc.createElement( "style" ) );
  style.setAttribute( "type", (int) m_type );
  if ( m_parent )
    style.setAttribute( "parent", m_parent->name() );
  style.setAttribute( "name", m_name );

  QDomElement format( doc.createElement( "format" ) );
  saveXML( doc, format );
  style.appendChild( format );

  styles.appendChild( style );
}

bool KSpreadCustomStyle::loadXML( QDomElement const & style, QString const & name )
{
  m_name = name;

  if ( style.hasAttribute( "parent" ) )
    m_parentName = style.attribute( "parent" );

  if ( !style.hasAttribute( "type" ) )
    return false;

  bool ok = true;
  m_type = (StyleType) style.attribute( "type" ).toInt( &ok );
  if ( !ok )
    return false;

  QDomElement f( style.namedItem( "format" ).toElement() );
  if ( !f.isNull() )
    if ( !KSpreadStyle::loadXML( f ) )
      return false;

  return true;
}

void KSpreadCustomStyle::setName( QString const & name )
{
  m_name = name;
}

void KSpreadCustomStyle::refreshParentName()
{
  if ( m_parent )
    m_parentName = m_parent->name();
}

bool KSpreadCustomStyle::definesAll() const
{
  if ( !( m_featuresSet & (uint) SAlignX ) )
    return false;
  if ( !( m_featuresSet & (uint) SAlignY ) )
    return false;
  if ( !( m_featuresSet & (uint) SPrefix ) )
    return false;
  if ( !( m_featuresSet & (uint) SPostfix ) )
    return false;
  if ( !( m_featuresSet & (uint) SLeftBorder ) )
    return false;
  if ( !( m_featuresSet & (uint) SRightBorder ) )
    return false;
  if ( !( m_featuresSet & (uint) STopBorder ) )
    return false;
  if ( !( m_featuresSet & (uint) SBottomBorder ) )
    return false;
  if ( !( m_featuresSet & (uint) SFallDiagonal ) )
    return false;
  if ( !( m_featuresSet & (uint) SGoUpDiagonal ) )
    return false;
  if ( !( m_featuresSet & (uint) SBackgroundBrush ) )
    return false;
  if ( !( m_featuresSet & (uint) SFontFamily ) )
    return false;
  if ( !( m_featuresSet & (uint) SFontSize ) )
    return false;
  if ( !( m_featuresSet & (uint) SFontFlag ) )
    return false;
  if ( !( m_featuresSet & (uint) STextPen ) )
    return false;
  if ( !( m_featuresSet & (uint) SBackgroundColor ) )
    return false;
  if ( !( m_featuresSet & (uint) SFloatFormat ) )
    return false;
  if ( !( m_featuresSet & (uint) SFloatColor ) )
    return false;
  if ( !( m_featuresSet & (uint) SMultiRow ) )
    return false;
  if ( !( m_featuresSet & (uint) SVerticalText ) )
    return false;
  if ( !( m_featuresSet & (uint) SPrecision ) )
    return false;
  if ( !( m_featuresSet & (uint) SFormatType ) )
    return false;
  if ( !( m_featuresSet & (uint) SAngle ) )
    return false;
  if ( !( m_featuresSet & (uint) SIndent ) )
    return false;
  if ( !( m_featuresSet & (uint) SDontPrintText ) )
    return false;
  if ( !( m_featuresSet & (uint) SCustomFormat ) )
    return false;
  if ( !( m_featuresSet & (uint) SNotProtected ) )
    return false;
  if ( !( m_featuresSet & (uint) SHideAll ) )
    return false;
  if ( !( m_featuresSet & (uint) SHideFormula ) )
    return false;

  return true;
}

void KSpreadCustomStyle::changeAlignX( KSpreadFormat::Align alignX )
{
  m_alignX = alignX;
  m_featuresSet |= SAlignX;
}

void KSpreadCustomStyle::changeAlignY( KSpreadFormat::AlignY alignY )
{
  m_alignY = alignY;
  m_featuresSet |= SAlignY;
}

void KSpreadCustomStyle::changeFont( QFont const & f )
{
  if ( m_fontFamily != f.family() )
  {
    m_fontFamily = f.family();
    m_featuresSet |= SFontFamily;
    m_featuresSet |= SFont;
  }
  if ( m_fontSize != f.pointSize() )
  {
    m_fontSize = f.pointSize();
    m_featuresSet |= SFont;
    m_featuresSet |= SFontSize;
  }

  if ( f.italic() != (m_fontFlags & (uint) FItalic ) )
  {
    if ( f.italic() )
      m_fontFlags |= FItalic;
    else
      m_fontFlags &= ~(uint) FItalic;
    m_featuresSet |= SFont;
    m_featuresSet |= SFontFlag;
  }
  if ( f.bold() != (m_fontFlags & (uint) FBold ) )
  {
    if ( f.bold() )
      m_fontFlags |= FBold;
    else
      m_fontFlags &= ~(uint) FBold;
    m_featuresSet |= SFont;
    m_featuresSet |= SFontFlag;
  }
  if ( f.underline() != (m_fontFlags & (uint) FUnderline ) )
  {
    if ( f.underline() )
      m_fontFlags |= FUnderline;
    else
      m_fontFlags &= ~(uint) FUnderline;
    m_featuresSet |= SFont;
    m_featuresSet |= SFontFlag;
  }
  if ( f.strikeOut() != (m_fontFlags & (uint) FStrike ) )
  {
    if ( f.strikeOut() )
      m_fontFlags |= FStrike;
    else
      m_fontFlags &= ~(uint) FStrike;
    m_featuresSet |= SFont;
    m_featuresSet |= SFontFlag;
  }
}

void KSpreadCustomStyle::changeFontFamily( QString const & fam )
{
  if ( m_fontFamily != fam )
  {
    m_fontFamily   = fam;
    m_featuresSet |= SFont;
    m_featuresSet |= SFontFamily;
  }
}

void KSpreadCustomStyle::changeFontSize( int size )
{
  if ( m_fontSize != size )
  {
    m_fontSize     = size;
    m_featuresSet |= SFont;
    m_featuresSet |= SFontSize;
  }
}

void KSpreadCustomStyle::changeFontFlags( uint flags )
{
  if ( m_fontFlags != flags )
  {
    m_fontFlags    = flags;
    m_featuresSet |= SFont;
    m_featuresSet |= SFontFlag;
  }
}

void KSpreadCustomStyle::changeTextColor( QColor const & color )
{
  m_textPen.setColor( color );
  m_featuresSet |= STextPen;
}

void KSpreadCustomStyle::changePen( QPen const & pen )
{
  m_textPen = pen;
  m_featuresSet |= STextPen;
}

void KSpreadCustomStyle::changeBgColor( QColor const & color )
{
  m_bgColor = color;
  m_featuresSet |= SBackgroundColor;
}

void KSpreadCustomStyle::changeRightBorderPen( QPen const & pen )
{
  m_rightBorderPen = pen;
  m_rightPenValue  = calculateValue( pen );
  m_featuresSet   |= SRightBorder;
}

void KSpreadCustomStyle::changeBottomBorderPen( QPen const & pen )
{
  m_bottomBorderPen = pen;
  m_bottomPenValue  = calculateValue( pen );
  m_featuresSet    |= SBottomBorder;
}

void KSpreadCustomStyle::changeLeftBorderPen( QPen const & pen )
{
  m_leftBorderPen = pen;
  m_leftPenValue  = calculateValue( pen );
  m_featuresSet  |= SLeftBorder;
}

void KSpreadCustomStyle::changeTopBorderPen( QPen const & pen )
{
  m_topBorderPen = pen;
  m_topPenValue  = calculateValue( pen );
  m_featuresSet |= STopBorder;
}

void KSpreadCustomStyle::changeFallBorderPen( QPen const & pen )
{
  m_fallDiagonalPen = pen;
  m_featuresSet |= SFallDiagonal;
}

void KSpreadCustomStyle::changeGoUpBorderPen( QPen const & pen )
{
  m_goUpDiagonalPen = pen;
  m_featuresSet |= SGoUpDiagonal;
}

void KSpreadCustomStyle::changeRotateAngle( int angle )
{
  m_rotateAngle = angle;
  m_featuresSet |= SAngle;
}

void KSpreadCustomStyle::changeIndent( double indent )
{
  m_indent = indent;
  m_featuresSet |= SIndent;
}

void KSpreadCustomStyle::changeBackGroundBrush( QBrush const & brush )
{
  m_backGroundBrush = brush;
  m_featuresSet |= SBackgroundBrush;
}

void KSpreadCustomStyle::changeFloatFormat( KSpreadFormat::FloatFormat format )
{
  m_floatFormat = format;
  m_featuresSet |= SFloatFormat;
}

void KSpreadCustomStyle::changeFloatColor( KSpreadFormat::FloatColor color )
{
  m_floatColor = color;
  m_featuresSet |= SFloatColor;
}

void KSpreadCustomStyle::changeFormatType( FormatType format )
{
  m_formatType = format;
  m_featuresSet |= SFormatType;
}

void KSpreadCustomStyle::changeStrFormat( QString const & strFormat )
{
  m_strFormat = strFormat;
  m_featuresSet |= SCustomFormat;
}

void KSpreadCustomStyle::changePrecision( int precision )
{
  m_precision = precision;
  m_featuresSet |= SPrecision;
}

void KSpreadCustomStyle::changePrefix( QString const & prefix )
{
  m_prefix = prefix;
  m_featuresSet |= SPrefix;
}

void KSpreadCustomStyle::changePostfix( QString const & postfix )
{
  m_postfix = postfix;
  m_featuresSet |= SPostfix;
}

void KSpreadCustomStyle::changeCurrency( KSpreadFormat::Currency const & currency )
{
  m_currency = currency;
}

void KSpreadCustomStyle::addProperty( Properties p )
{
  m_properties |= (uint) p;
  switch( p )
  {
   case PDontPrintText:
    m_featuresSet |= SDontPrintText;
    break;
   case PCustomFormat:
    m_featuresSet |= SCustomFormat;
    break;
   case PNotProtected:
    m_featuresSet |= SNotProtected;
    break;
   case PHideAll:
    m_featuresSet |= SHideAll;
    break;
   case PHideFormula:
    m_featuresSet |= SHideFormula;
    break;
   case PMultiRow:
    m_featuresSet |= SMultiRow;
    break;
   case PVerticalText:
    m_featuresSet |= SVerticalText;
    break;
   default:
    kdWarning() << "Unhandled property" << endl;
  }
}

void KSpreadCustomStyle::removeProperty( Properties p )
{
  m_properties &= ~(uint) p;
  switch( p )
  {
   case PDontPrintText:
    m_featuresSet &= SDontPrintText;
    break;
   case PCustomFormat:
    m_featuresSet &= SCustomFormat;
    break;
   case PNotProtected:
    m_featuresSet &= SNotProtected;
    break;
   case PHideAll:
    m_featuresSet &= SHideAll;
    break;
   case PHideFormula:
    m_featuresSet &= SHideFormula;
    break;
   case PMultiRow:
    m_featuresSet &= SMultiRow;
    break;
   case PVerticalText:
    m_featuresSet &= SVerticalText;
    break;
   default:
    kdWarning() << "Unhandled property" << endl;
  }
}



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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <qdom.h>
#include <qbuffer.h>

#include <kdebug.h>
#include <klocale.h>

#include <KoGlobal.h>
#include <KoGenStyles.h>
#include <KoOasisStyles.h>
#include <KoStyleStack.h>
#include <KoXmlWriter.h>
#include <KoXmlNS.h>

#include "kspread_util.h"
#include "kspread_doc.h"

#include "kspread_style.h"

using namespace KSpread;

static uint calculateValue( QPen const & pen )
{
  uint n = pen.color().red() + pen.color().green() + pen.color().blue();

  n += 1000 * pen.width();
  n += 10000 * (uint) pen.style();

  return n;
}

Style::Style()
  : m_parent( 0 ),
    m_type( AUTO ),
    m_usageCount( 1 ),
    m_featuresSet( 0 ),
    m_alignX( HAlignUndefined ),
    m_alignY( Middle ),
    m_floatFormat( OnlyNegSigned ),
    m_floatColor( AllBlack ),
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

Style::Style( Style * style )
  : m_parent( ( style->m_type == BUILTIN || style->m_type == CUSTOM ) ? (CustomStyle *) style : 0 ),
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

Style::~Style()
{
}

bool Style::operator == (const Style& style) const
{
	//This is prone to error because if someone adds additional
	//properties to the style class they will have to remember
	//to correct this function - can we do this a better way?

	if ( 	m_properties == style.m_properties &&
		m_type == style.m_type &&
		m_featuresSet == style.m_featuresSet &&
		m_alignX == style.m_alignX &&
		m_alignY == style.m_alignY &&
		m_floatFormat == style.m_floatFormat &&
		m_floatColor == style.m_floatColor &&
		m_formatType == style.m_formatType &&
		m_fontFamily == style.m_fontFamily &&
		m_fontFlags == style.m_fontFlags &&
		m_fontSize == style.m_fontSize &&
		m_textPen == style.m_textPen &&
		m_bgColor == style.m_bgColor &&
		m_rightBorderPen == style.m_rightBorderPen &&
		m_bottomBorderPen == style.m_bottomBorderPen &&
		m_leftBorderPen == style.m_leftBorderPen &&
		m_topBorderPen == style.m_topBorderPen &&
		m_fallDiagonalPen == style.m_fallDiagonalPen &&
	        m_goUpDiagonalPen == style.m_goUpDiagonalPen &&
		m_backGroundBrush == style.m_backGroundBrush &&
		m_rotateAngle == style.m_rotateAngle &&
		m_indent == style.m_indent &&
		m_strFormat == style.m_strFormat &&
		m_precision == style.m_precision &&
		m_prefix == style.m_prefix &&
		m_postfix == style.m_postfix &&
		m_currency.type == style.m_currency.type &&
		m_properties == style.m_properties )

		return true;
	else
		return false;
}

void Style::loadOasisStyle( KoOasisStyles& oasisStyles, const QDomElement & element )
{
    kDebug()<<"void Style::loadOasisStyle( const QDomElement & element )**************: name :"<<endl;
    KoStyleStack styleStack;
    styleStack.push( element );
    styleStack.setTypeProperties( "table-cell" );
    QString str;
    if ( element.hasAttributeNS( KoXmlNS::style, "data-style-name" ) )
    {
        //kDebug()<<"styleStack.attribute( style:data-style-name ) :"<<element.attributeNS( KoXmlNS::style, "data-style-name", QString::null )<<endl;
        //kDebug()<< " oasisStyles.dataFormats()[...] :"<< oasisStyles.dataFormats()[element.attributeNS( KoXmlNS::style, "data-style-name" , QString::null)].formatStr<<endl;
        //kDebug()<< " oasisStyles.dataFormats()[...] prefix :"<< oasisStyles.dataFormats()[element.attributeNS( KoXmlNS::style, "data-style-name" , QString::null)].prefix<<endl;
        //kDebug()<< " oasisStyles.dataFormats()[...] suffix :"<< oasisStyles.dataFormats()[element.attributeNS( KoXmlNS::style, "data-style-name" , QString::null)].suffix<<endl;

        str = element.attributeNS( KoXmlNS::style, "data-style-name" , QString::null);

        QString tmp = oasisStyles.dataFormats()[str].prefix;
        if ( !tmp.isEmpty() )
        {
            m_prefix = tmp;
            m_featuresSet |= SPrefix;
        }
        tmp = oasisStyles.dataFormats()[str].suffix;
        if ( !tmp.isEmpty() )
        {
            m_postfix = tmp;
            m_featuresSet |= SPostfix;
        }
        tmp = oasisStyles.dataFormats()[str].formatStr;
        if ( !tmp.isEmpty() )
        {
            m_formatType = Style::formatType( tmp );
            m_featuresSet |= SFormatType;
        }
    }

    styleStack.setTypeProperties( "text" );
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "font-family" ) )
    {
        m_fontFamily = styleStack.attributeNS( KoXmlNS::fo, "font-family" );
        kDebug()<<"styleStack.hasAttribute( fo:font-family ) :"<<styleStack.hasAttributeNS( KoXmlNS::fo, "font-family" )<<endl;
        m_featuresSet |= SFontFamily;
        m_featuresSet |= SFont;
        m_featuresSet |= SFontFlag;
    }
//fo:font-size="13pt" fo:font-style="italic" style:text-underline="double" style:text-underline-color="font-color" fo:font-weight="bold"
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "font-size" ) )
    {
        m_fontSize = (int) KoUnit::parseValue( styleStack.attributeNS( KoXmlNS::fo, "font-size" ), 10.0 );
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
        m_featuresSet |= SFontFlag;

    }
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "font-weight" ) )
    {
        m_fontFlags |= FBold;
    }

    //TODO "style:text-underline-width"/""style:text-underline-color"
    if ( ( styleStack.hasAttributeNS( KoXmlNS::fo, "text-underline-style" ) &&styleStack.attributeNS( KoXmlNS::fo, "text-underline-style" )!="none" )
         || ( styleStack.hasAttributeNS( KoXmlNS::style, "text-underline-style" ) && styleStack.attributeNS( KoXmlNS::style, "text-underline-style" )!="none") )
    {
        m_fontFlags |= FUnderline;
        m_featuresSet |= SFontFlag;
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

    if ( styleStack.hasAttributeNS( KoXmlNS::style, "text-line-through-style" ) && styleStack.attributeNS( KoXmlNS::style, "text-line-through-style" )!="none"
         /*&& styleStack.attributeNS("text-line-through-style")=="solid"*/ )
    {
        m_fontFlags |= FStrike;
        m_featuresSet |= SFontFlag;
    }


    styleStack.setTypeProperties( "paragraph" );
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "text-align" ) )
    {

        str = styleStack.attributeNS( KoXmlNS::fo, "text-align" );
        kDebug()<<"str :"<<str<<endl;
        if ( str == "center" )
            m_alignX = Center;
        else if ( str == "end" )
            m_alignX = Right;
        else if ( str == "start" )
            m_alignX = Left;
        else
            m_alignX = HAlignUndefined;
        m_featuresSet |= SHAlign;
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::office, "value-type" ) )
    {
      m_formatType = Generic_format;

      str = styleStack.attributeNS( KoXmlNS::office, "value-type" );
      kDebug()<<"str :"<<str<<endl<<endl;
      if ( str == "float" ) // TODO
        m_formatType = Number_format;
      else if ( str == "time" ) // TODO
        m_formatType = Time_format;
      else if ( str == "date" ) // TODO
        m_formatType = TextDate_format;
      else if ( str == "percentage" )
        m_formatType = Percentage_format;
      else if ( str == "currency" )
        m_formatType = Money_format;
      else if ( str == "boolean" ) // TODO
        ;
      else if ( str == "string" ) //TODO
        m_formatType = Text_format;

      if ( m_formatType != Generic_format )
        m_featuresSet |= SFormatType;
    }

    styleStack.setTypeProperties( "table-cell" );
    if ( styleStack.hasAttributeNS( KoXmlNS::style, "vertical-align" ) )
    {
        m_alignY = VAlignUndefined;

        str = styleStack.attributeNS( KoXmlNS::style, "vertical-align" );
        if ( str == "bottom" )
            m_alignY = Bottom;
        else if ( str =="top" )
            m_alignY = Top;
        else if ( str =="middle" )
            m_alignY = Middle;

        if (m_alignY != VAlignUndefined) // file's property is invalid
            m_featuresSet |= SVAlign;
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "background-color" ) )
    {
        m_bgColor = QColor(  styleStack.attributeNS( KoXmlNS::fo, "background-color" ) );
        if ( m_bgColor.isValid() && m_bgColor != Qt::white )
          m_featuresSet |= SBackgroundColor;
    }

    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "wrap-option" )&&( styleStack.attributeNS( KoXmlNS::fo, "wrap-option" )=="wrap" ) )
    {
        setProperty( SMultiRow );
        m_featuresSet |= SMultiRow;
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::style, "cell-protect" ) )
    {
        str = styleStack.attributeNS( KoXmlNS::style, "cell-protect" );
        if ( str=="hidden-and-protected" )
        {
            setProperty( SHideAll );
            m_featuresSet |= SHideAll;
        }
        else if ( str == "protected formula-hidden" )
        {
            setProperty( SHideFormula );
            m_featuresSet |= SHideFormula;
        }
        else if ( str == "protected" )
        {
            setProperty( SNotProtected );
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
        setProperty( SDontPrintText );
        m_featuresSet |= SDontPrintText;

    }
    if ( styleStack.hasAttributeNS( KoXmlNS::style, "direction" ) && ( styleStack.attributeNS( KoXmlNS::style, "direction" )=="ttb" ) )
    {
        setProperty( SVerticalText );
        m_featuresSet |= SVerticalText;

    }
    if ( styleStack.hasAttributeNS( KoXmlNS::style, "rotation-angle" ) )
    {
        bool ok;
        int a = styleStack.attributeNS( KoXmlNS::style, "rotation-angle" ).toInt( &ok );
        kDebug()<<" rotation-angle :"<<a<<endl;
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
        kDebug()<<" style name :"<<styleStack.attributeNS( KoXmlNS::draw, "style-name" )<<endl;

        const QDomElement * style = oasisStyles.findStyle( styleStack.attributeNS( KoXmlNS::draw, "style-name" ), "graphic" );
        kDebug()<<" style :"<<style<<endl;
      if ( style )
      {
        KoStyleStack drawStyleStack;
        drawStyleStack.push( *style );
        drawStyleStack.setTypeProperties( "graphic" );
        if ( drawStyleStack.hasAttributeNS( KoXmlNS::draw, "fill" ) )
        {
            const QString fill = drawStyleStack.attributeNS( KoXmlNS::draw, "fill" );
            kDebug()<<" load object gradient fill type :"<<fill<<endl;

            if ( fill == "solid" || fill == "hatch" )
            {
                kDebug()<<" Style ******************************************************\n";
                m_backGroundBrush=KoOasisStyles::loadOasisFillStyle( drawStyleStack, fill, oasisStyles );
                m_featuresSet |= SBackgroundBrush;
            }
            else
                kDebug()<<" fill style not supported into kspread : "<<fill<<endl;
        }
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
            kDebug(36001) << "Value out of range Cell::precision=" << i << endl;
            return false;
        }
        m_precision = i;
        m_featuresSet |= SPrecision;
    }

    if ( format.hasAttribute( "float" ) )
    {
        FloatFormat a = (FloatFormat)format.attribute( "float" ).toInt( &ok );
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
        FloatColor a = (FloatColor) format.attribute( "floatcolor" ).toInt( &ok );
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
    if ( m_formatType == Money )
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

FormatType Style::formatType( const QString &_format )
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

QString Style::saveOasisStyleNumeric( KoGenStyle &style, KoGenStyles &mainStyles,
                                      FormatType _style,
                                      const QString &_prefix, const QString &_postfix,
                                      int _precision, const QString& /*symbol*/ )
{
//  kDebug() << k_funcinfo << endl;
    QString styleName;
    QString valueType;
    switch( _style )
    {
    case Number_format:
        styleName = saveOasisStyleNumericNumber( mainStyles,_style, _precision );
        valueType = "float";
        break;
    case Text_format:
        styleName = saveOasisStyleNumericText( mainStyles,_style,_precision );
        valueType = "string";
        break;
    case Money_format:
        styleName = saveOasisStyleNumericMoney( mainStyles,_style,_precision);
        valueType = "currency";
        break;
    case Percentage_format:
        styleName = saveOasisStyleNumericPercentage( mainStyles,_style,_precision );
        valueType = "percentage";
        break;
    case Scientific_format:
        styleName = saveOasisStyleNumericScientific( mainStyles,_style, _prefix, _postfix,_precision );
        valueType = "float";
        break;
    case ShortDate_format:
    case TextDate_format:
        styleName = saveOasisStyleNumericDate( mainStyles,_style );
        valueType = "date";
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
        valueType = "time";
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
        valueType = "float";
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
        valueType = "date";
        break;
    case Custom_format:
        styleName = saveOasisStyleNumericCustom( mainStyles,_style );
        break;
    case Generic_format:
    case No_format:
      break;
    }
    if ( !valueType.isEmpty() )
    {
      kDebug() << "addProperty ParagraphType" << endl;
      KoGenStyle::PropertyType pt = KoGenStyle::ParagraphType;
      style.addProperty( "office:value-type", valueType, pt );
    }
    if ( !styleName.isEmpty() )
    {
      style.addAttribute( "style:data-style-name", styleName );
    }
    return styleName;
}

QString Style::saveOasisStyleNumericNumber( KoGenStyles& /*mainStyles*/, FormatType /*_style*/, int /*_precision*/ )
{
    return "";
}

QString Style::saveOasisStyleNumericText( KoGenStyles& /*mainStyles*/, FormatType /*_style*/, int /*_precision*/ )
{
    return "";
}

QString Style::saveOasisStyleNumericMoney( KoGenStyles& mainStyles, FormatType /*_style*/, int /*_precision*/ )
{
    QString format;
    return KoOasisStyles::saveOasisCurrencyStyle( mainStyles, format, format );
}

QString Style::saveOasisStyleNumericPercentage( KoGenStyles&mainStyles, FormatType /*_style*/, int _precision )
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


QString Style::saveOasisStyleNumericScientific( KoGenStyles&mainStyles, FormatType /*_style*/, const QString &_prefix, const QString _suffix, int _precision )
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

QString Style::saveOasisStyleNumericDate( KoGenStyles&mainStyles, FormatType _style )
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
        kDebug()<<"this date format is not defined ! :"<<_style<<endl;
        break;
    }
    return KoOasisStyles::saveOasisDateStyle( mainStyles, format, locale );
}

QString Style::saveOasisStyleNumericCustom( KoGenStyles& /*mainStyles*/, FormatType /*_style*/ )
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

QString Style::saveOasisStyleNumericTime( KoGenStyles& mainStyles, FormatType _style )
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
    //TODO use format
    switch( _style )
    {
    case Time_format: //TODO FIXME
        format = "hh:mm:ss";
        break;
    case SecondeTime_format: //TODO FIXME
        format = "hh:mm";
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
        kDebug()<<"time format not defined :"<<_style<<endl;
        break;
    }
    return KoOasisStyles::saveOasisTimeStyle( mainStyles, format, locale );
}


QString Style::saveOasisStyleNumericFraction( KoGenStyles &mainStyles, FormatType _style, const QString &_prefix, const QString _suffix )
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
        kDebug()<<" fraction format not defined :"<<_style<<endl;
        break;
    }

    return KoOasisStyles::saveOasisFractionStyle( mainStyles, format, _prefix, _suffix );
}

QString Style::saveOasis( KoGenStyle& style, KoGenStyles& mainStyles )
{
  // KSpread::Style is definitly an OASIS auto style,
  // but don't overwrite it, if it already exists
  if (style.type() == 0)
      style = KoGenStyle( Doc::STYLE_CELL_AUTO, "table-cell" );
  // doing the real work
  saveOasisStyle( style, mainStyles );
  return QString::null;
}

void Style::saveOasisStyle( KoGenStyle &style, KoGenStyles &mainStyles )
{
#ifndef NDEBUG
    //if (type() == BUILTIN )
    //  kDebug() << "BUILTIN" << endl;
    //else if (type() == CUSTOM )
    //  kDebug() << "CUSTOM" << endl;
    //else if (type() == AUTO )
    //  kDebug() << "AUTO" << endl;
#endif

    // don't store parent, if it's the default style
    if ( m_parent && (m_parent->type() != BUILTIN || m_parent->name() != "Default") )
        // FIXME this is not the OASIS parent style's name. it's its display name!
        style.addAttribute( "style:parent-style-name", m_parent->name() );


    if ( featureSet( SHAlign ) && halign() != HAlignUndefined )
    {
        QString value;
        switch( halign() )
        {
        case Center:
            value = "center";
            break;
        case Right:
            value = "end";
            break;
        case Left:
            value = "start";
            break;
        case HAlignUndefined:
            break;
        }
        if ( !value.isEmpty() )
            style.addProperty( "fo:text-align", value, KoGenStyle::ParagraphType );
    }

    if ( featureSet( SVAlign ) )
    {
        QString value;
        switch( valign() )
        {
        case Top:
            value = "top";
            break;
        case Middle:
            value = "middle";
            break;
        case Bottom:
            value = "bottom";
            break;
        case VAlignUndefined:
        default:
            break;
        }
        if (!value.isEmpty()) // sanity
            style.addProperty( "style:vertical-align", value );
    }

    if ( featureSet( SBackgroundColor ) && m_bgColor != QColor() && m_bgColor.isValid() )
        style.addProperty( "fo:background-color", colorName(m_bgColor) );

    if ( featureSet( SMultiRow ) && hasProperty( SMultiRow ) )
        style.addProperty( "fo:wrap-option", "wrap" );
    if ( featureSet( SVerticalText ) && hasProperty( SVerticalText ) )
    {
        style.addProperty( "style:direction", "ttb" );
        style.addProperty( "style:rotation-angle", "0" );
        style.addProperty( "style:rotation-align", "none" );
    }
#if 0
    if ( featureSet( SFloatFormat ) )
        format.setAttribute( "float", (int) m_floatFormat );

    if ( featureSet( SFloatColor ) )
        format.setAttribute( "floatcolor", (int)m_floatColor );

    if ( featureSet( SCustomFormat ) && !strFormat().isEmpty() )
        format.setAttribute( "custom", m_strFormat );

    if ( featureSet( SFormatType ) && formatType() == Money )
    {
        format.setAttribute( "type", (int) m_currency.type );
        format.setAttribute( "symbol", m_currency.symbol );
    }
#endif
    if ( featureSet( SAngle ) )
    {
        style.addProperty( "style:rotation-align", "none" );
        style.addProperty( "style:rotation-angle", QString::number( -1.0 *m_rotateAngle  ) );
    }
    if ( featureSet( SIndent ) )
    {
        style.addPropertyPt("fo:margin-left", m_indent, KoGenStyle::ParagraphType );
        //FIXME
        //if ( a == HAlignUndefined )
        //currentCellStyle.addProperty("fo:text-align", "start" );
    }
    if ( featureSet( SDontPrintText ) && hasProperty( SDontPrintText ) )
        style.addProperty( "style:print-content", "false");

    bool hideAll = false;
    bool hideFormula = false;
    bool isNotProtected = false;

    if ( featureSet( SNotProtected ) && hasProperty( SNotProtected ) )
        isNotProtected = true;

    if ( featureSet( SHideAll ) && hasProperty( SHideAll ) )
        hideAll=true;

    if ( featureSet( SHideFormula ) && hasProperty( SHideFormula ) )
        hideFormula = true;

    if ( hideAll )
        style.addProperty( "style:cell-protect", "hidden-and-protected" );
    else
    {
        if ( isNotProtected && !hideFormula )
            style.addProperty( "style:cell-protect", "none" );
        else if ( isNotProtected && hideFormula )
            style.addProperty( "style:cell-protect", "formula-hidden" );
        else if ( hideFormula )
            style.addProperty( "style:cell-protect", "protected formula-hidden" );
        else if ( featureSet( SNotProtected ) )// !isNotProtected
            // write out, only if it is explicity set
            style.addProperty( "style:cell-protect", "protected" );
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
        style.addProperty("fo:font-family", m_fontFamily, KoGenStyle::TextType );
    }
    if ( featureSet( SFontSize ) )
    {
        style.addPropertyPt("fo:font-size",m_fontSize, KoGenStyle::TextType  );
    }

    if (m_fontFlags & (uint) FBold )
        style.addProperty("fo:font-weight","bold", KoGenStyle::TextType );
    if ( m_fontFlags & (uint) FItalic )
        style.addProperty("fo:font-style", "italic", KoGenStyle::TextType );

    if ( m_fontFlags & (uint) FUnderline )
    {
        //style:text-underline-style="solid" style:text-underline-width="auto"
        style.addProperty( "style:text-underline-style", "solid", KoGenStyle::TextType );
        //copy from oo-129
        style.addProperty( "style:text-underline-width", "auto", KoGenStyle::TextType );
        style.addProperty( "style:text-underline-color", "font-color", KoGenStyle::TextType );
    }

    if ( m_fontFlags & (uint) FStrike )
        style.addProperty( "style:text-line-through-style", "solid", KoGenStyle::TextType );

    if ( featureSet( STextPen ) && m_textPen.color().isValid() )
    {
        style.addProperty("fo:color", colorName(m_textPen.color()), KoGenStyle::TextType );
    }
    //I don't think there is a reason why the background brush should be saved if it is null,
    //but remove the check if it causes problems.  -- Robert Knight <robertknight@gmail.com>
    if ( featureSet( SBackgroundBrush ) && (m_backGroundBrush.style() != Qt::NoBrush) )
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
    {
        if ( m_precision > -1 )
            style.addAttribute( "style:decimal-places", m_precision );
        _precision =  m_precision;
    }

    QString symbol;
    if ( featureSet( SFormatType ) && m_formatType == Money_format )
    {
      symbol = m_currency.symbol;
    }

    QString numericStyle = saveOasisStyleNumeric( style, mainStyles, m_formatType,
                                                  _prefix, _postfix, _precision,
                                                  symbol );
    if ( !numericStyle.isEmpty() )
        style.addAttribute( "style:data-style-name", numericStyle );
}

QString Style::saveOasisBackgroundStyle( KoGenStyles &mainStyles, const QBrush &brush )
{
    KoGenStyle styleobjectauto = KoGenStyle( KoGenStyle::STYLE_GRAPHICAUTO, "graphic" );
    KoOasisStyles::saveOasisFillStyle( styleobjectauto, mainStyles, brush );
    return mainStyles.lookup( styleobjectauto, "gr" );
}

void Style::saveXML( QDomDocument & doc, QDomElement & format ) const
{
  if ( featureSet( SHAlign ) && halign() != HAlignUndefined )
    format.setAttribute( "alignX", (int) m_alignX );

  if ( featureSet( SVAlign ) && valign() != Middle )
    format.setAttribute( "alignY", (int) m_alignY );

  if ( featureSet( SBackgroundColor ) && m_bgColor != QColor() && m_bgColor.isValid() )
    format.setAttribute( "bgcolor", m_bgColor.name() );

  if ( featureSet( SMultiRow ) && hasProperty( SMultiRow ) )
    format.setAttribute( "multirow", "yes" );

  if ( featureSet( SVerticalText ) && hasProperty( SVerticalText ) )
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

  if ( featureSet( SDontPrintText ) && hasProperty( SDontPrintText ) )
    format.setAttribute( "dontprinttext", "yes" );

  if ( featureSet( SNotProtected ) && hasProperty( SNotProtected ) )
    format.setAttribute( "noprotection", "yes" );

  if ( featureSet( SHideAll ) && hasProperty( SHideAll ) )
    format.setAttribute( "hideall", "yes" );

  if ( featureSet( SHideFormula ) && hasProperty( SHideFormula ) )
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

bool Style::loadXML( QDomElement & format )
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
    HAlign a = (HAlign) format.attribute( "alignX" ).toInt( &ok );
    if ( !ok )
      return false;
    if ( (unsigned int) a >= 1 || (unsigned int) a <= 4 )
    {
      m_alignX = a;
      m_featuresSet |= SHAlign;
    }
  }
  if ( format.hasAttribute( "alignY" ) )
  {
    VAlign a = (VAlign) format.attribute( "alignY" ).toInt( &ok );
    if ( !ok )
      return false;
    if ( (unsigned int) a >= 1 || (unsigned int) a < 4 )
    {
      m_alignY = a;
      m_featuresSet |= SVAlign;
    }
  }

  if ( format.hasAttribute( "bgcolor" ) )
  {
    m_bgColor = QColor( format.attribute( "bgcolor" ) );
    // FIXME: Is white always correct here?
    if ( m_bgColor != Qt::white )
	m_featuresSet |= SBackgroundColor;
  }

  if ( format.hasAttribute( "multirow" ) )
  {
    setProperty( SMultiRow );
    m_featuresSet |= SMultiRow;
  }

  if ( format.hasAttribute( "verticaltext" ) )
  {
    setProperty( SVerticalText );
    m_featuresSet |= SVerticalText;
  }

  if ( format.hasAttribute( "precision" ) )
  {
    int i = format.attribute( "precision" ).toInt( &ok );
    if ( i < -1 )
    {
      kDebug(36001) << "Value out of range Cell::precision=" << i << endl;
      return false;
    }
    m_precision = i;
    m_featuresSet |= SPrecision;
  }

  if ( format.hasAttribute( "float" ) )
  {
    FloatFormat a = (FloatFormat)format.attribute( "float" ).toInt( &ok );
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
    FloatColor a = (FloatColor) format.attribute( "floatcolor" ).toInt( &ok );
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
    setProperty( SDontPrintText );
    m_featuresSet |= SDontPrintText;
  }

  if ( format.hasAttribute( "noprotection" ) )
  {
    setProperty( SNotProtected );
    m_featuresSet |= SNotProtected;
  }

  if ( format.hasAttribute( "hideall" ) )
  {
    setProperty( SHideAll );
    m_featuresSet |= SHideAll;
  }

  if ( format.hasAttribute( "hideformula" ) )
  {
    setProperty( SHideFormula );
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
    // It is not necessary to set this feature just because the color changes.
    // FIXME: Or is it?
    //m_featuresSet |= SBackgroundBrush;
  }

  if ( format.hasAttribute( "brushstyle" ) )
  {
    m_backGroundBrush.setStyle( (Qt::BrushStyle) format.attribute( "brushstyle" ).toInt( &ok )  );
    if ( !ok )
      return false;

    if ( m_backGroundBrush.style() != Qt::NoBrush )
	m_featuresSet |= SBackgroundBrush;
  }

  QDomElement pen = format.namedItem( "pen" ).toElement();
  if ( !pen.isNull() )
  {
    m_textPen = util_toPen( pen );
    if ( m_textPen.style() != Qt::NoPen )
	m_featuresSet |= STextPen;
  }

  QDomElement left = format.namedItem( "left-border" ).toElement();
  if ( !left.isNull() )
  {
    QDomElement pen = left.namedItem( "pen" ).toElement();
    if ( !pen.isNull() )
    {
      m_leftBorderPen = util_toPen( pen );
      if ( m_leftBorderPen.style() != Qt::NoPen )
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
      if ( m_topBorderPen.style() != Qt::NoPen )
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
      if ( m_rightBorderPen.style() != Qt::NoPen )
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
      if ( m_bottomBorderPen.style() != Qt::NoPen )
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
      if ( m_fallDiagonalPen.style() != Qt::NoPen )
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
      if ( m_goUpDiagonalPen.style() != Qt::NoPen )
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

void Style::setParent( CustomStyle * parent )
{
  m_parent = parent;
  if ( m_parent )
    m_parentName = m_parent->name();
}

CustomStyle * Style::parent() const
{
  return m_parent;
}

bool Style::release()
{
  --m_usageCount;

  if ( m_type == CUSTOM || m_type == BUILTIN )
    return false; // never delete builtin styles...

  if ( m_usageCount < 1 )
    return true;

  return false;
}

void Style::addRef()
{
  ++m_usageCount;
}

bool Style::hasProperty( FlagsSet p ) const
{
  FlagsSet f;
  switch( p )
  {
   case SDontPrintText:
    f = SDontPrintText;
    break;
   case SCustomFormat:
    f = SCustomFormat;
    break;
   case SNotProtected:
    f = SNotProtected;
    break;
   case SHideAll:
    f = SHideAll;
    break;
   case SHideFormula:
    f = SHideFormula;
    break;
   case SMultiRow:
    f = SMultiRow;
    break;
   case SVerticalText:
    f = SVerticalText;
    break;
   default:
    kWarning() << "Unhandled property" << endl;
    return ( m_properties  & (uint) p );
  }

  return ( !m_parent || featureSet( f ) ? ( m_properties & (uint) p ) : m_parent->hasProperty( p ) );
}

bool Style::hasFeature( FlagsSet f, bool withoutParent ) const
{
  bool b = ( m_featuresSet & (uint) f );

  // check if feature is defined here or at parent level
  if ( m_parent && !withoutParent )
    b = ( m_parent->hasFeature( f, withoutParent ) ? true : b );

  return b;
}

QFont Style::font() const
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

QString const & Style::fontFamily() const
{
  return ( !m_parent || featureSet( SFontFamily ) ? m_fontFamily : m_parent->fontFamily() );
}

uint Style::fontFlags() const
{
  return ( !m_parent || featureSet( SFontFlag ) ? m_fontFlags : m_parent->fontFlags() );
}

int Style::fontSize() const
{
  return ( !m_parent || featureSet( SFontSize ) ? m_fontSize : m_parent->fontSize() );
}

QPen const & Style::pen() const
{
  return ( !m_parent || featureSet( STextPen ) ? m_textPen : m_parent->pen() );
}

QColor const & Style::bgColor() const
{
  return ( !m_parent || featureSet( SBackgroundColor ) ? m_bgColor : m_parent->bgColor() );
}

QPen const & Style::rightBorderPen() const
{
  return ( !m_parent || featureSet( SRightBorder ) ? m_rightBorderPen : m_parent->rightBorderPen() );
}

QPen const & Style::bottomBorderPen() const
{
  return ( !m_parent || featureSet( SBottomBorder ) ? m_bottomBorderPen : m_parent->bottomBorderPen() );
}

QPen const & Style::leftBorderPen() const
{
  return ( !m_parent || featureSet( SLeftBorder ) ? m_leftBorderPen : m_parent->leftBorderPen() );
}

QPen const & Style::topBorderPen() const
{
  return ( !m_parent || featureSet( STopBorder ) ? m_topBorderPen : m_parent->topBorderPen() );
}

QPen const & Style::fallDiagonalPen() const
{
  return ( !m_parent || featureSet( SFallDiagonal ) ? m_fallDiagonalPen : m_parent->fallDiagonalPen() );
}

QPen const & Style::goUpDiagonalPen() const
{
  return ( !m_parent || featureSet( SGoUpDiagonal ) ? m_goUpDiagonalPen : m_parent->goUpDiagonalPen() );
}

int Style::precision() const
{
  return ( !m_parent || featureSet( SPrecision ) ? m_precision : m_parent->precision() );
}

int Style::rotateAngle() const
{
  return ( !m_parent || featureSet( SAngle ) ? m_rotateAngle : m_parent->rotateAngle() );
}

double Style::indent() const
{
  return ( !m_parent || featureSet( SIndent ) ? m_indent : m_parent->indent() );
}

QBrush const & Style::backGroundBrush() const
{
  return ( !m_parent || featureSet( SBackgroundBrush ) ? m_backGroundBrush : m_parent->backGroundBrush() );
}

Style::HAlign Style::halign() const
{
  return ( !m_parent || featureSet( SHAlign ) ? m_alignX : m_parent->halign() );
}

Style::VAlign Style::valign() const
{
  return ( !m_parent || featureSet( SVAlign ) ? m_alignY : m_parent->valign() );
}

Style::FloatFormat Style::floatFormat() const
{
  return ( !m_parent || featureSet( SFloatFormat ) ? m_floatFormat : m_parent->floatFormat() );
}

Style::FloatColor Style::floatColor() const
{
  return ( !m_parent || featureSet( SFloatColor ) ? m_floatColor : m_parent->floatColor() );
}

FormatType Style::formatType() const
{
  return ( !m_parent || featureSet( SFormatType ) ? m_formatType : m_parent->formatType() );
}

Style::Currency const & Style::currency() const
{
  return ( !m_parent || featureSet( SFormatType ) ? m_currency : m_parent->currency() );
}

QString const & Style::strFormat() const
{
  return ( !m_parent || featureSet( SCustomFormat ) ? m_strFormat : m_parent->strFormat() );
}

QString const & Style::prefix() const
{
  return ( !m_parent || featureSet( SPrefix ) ? m_prefix : m_parent->prefix() );
}

QString const & Style::postfix() const
{
  return ( !m_parent || featureSet( SPostfix ) ? m_postfix : m_parent->postfix() );
}



Style * Style::setHAlign( HAlign alignX )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    Style * style = new Style( this );
    style->m_alignX = alignX;
    style->m_featuresSet |= SHAlign;
    return style;
  }

  m_alignX      = alignX;
  m_featuresSet |= SHAlign;
  return this;
}

Style * Style::setVAlign( VAlign alignY )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    Style * style = new Style( this );
    style->m_alignY = alignY;
    style->m_featuresSet |= SVAlign;
    return style;
  }

  m_alignY = alignY;
  m_featuresSet |= SVAlign;
  return this;
}

Style * Style::setFont( QFont const & f )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    Style * style = new Style( this );
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

Style * Style::setFontFamily( QString const & fam )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    if ( m_fontFamily != fam )
    {
      Style * style  = new Style( this );
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

Style * Style::setFontFlags( uint flags )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    if ( m_fontFlags != flags )
    {
      Style * style = new Style( this );
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

Style * Style::setFontSize( int size )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    if ( m_fontSize != size )
    {
      Style * style  = new Style( this );
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

Style * Style::setPen( QPen const & pen )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    Style * style = new Style( this );
    style->m_textPen = pen;
    if ( style->m_textPen.style() != Qt::NoPen )
	style->m_featuresSet |= STextPen;
    return style;
  }

  m_textPen = pen;
  if ( m_textPen.style() != Qt::NoPen )
      m_featuresSet |= STextPen;
  return this;
}

Style * Style::setBgColor( QColor const & color )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    Style * style = new Style( this );
    style->m_bgColor = color;
    if ( style->m_bgColor != Qt::white )
	style->m_featuresSet |= SBackgroundColor;
    return style;
  }

  m_bgColor = color;
  if ( m_bgColor != Qt::white )
      m_featuresSet |= SBackgroundColor;
  return this;
}

Style * Style::setRightBorderPen( QPen const & pen )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    Style * style = new Style( this );
    style->m_rightBorderPen = pen;
    style->m_rightPenValue = calculateValue( pen );
    if ( style->m_rightBorderPen.style() != Qt::NoPen )
	style->m_featuresSet |= SRightBorder;
    return style;
  }

  m_rightBorderPen = pen;
  m_rightPenValue = calculateValue( pen );
  if ( m_rightBorderPen.style() != Qt::NoPen )
      m_featuresSet |= SRightBorder;
  return this;
}

Style * Style::setBottomBorderPen( QPen const & pen )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    Style * style = new Style( this );
    style->m_bottomBorderPen = pen;
    style->m_bottomPenValue = calculateValue( pen );
    if ( style->m_bottomBorderPen.style() != Qt::NoPen )
        style->m_featuresSet |= SBottomBorder;
    return style;
  }

  m_bottomBorderPen = pen;
  m_bottomPenValue = calculateValue( pen );
  if ( m_bottomBorderPen.style() != Qt::NoPen )
      m_featuresSet |= SBottomBorder;
  return this;
}

Style * Style::setLeftBorderPen( QPen const & pen )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    Style * style = new Style( this );
    style->m_leftBorderPen = pen;
    style->m_leftPenValue = calculateValue( pen );
    if ( style->m_leftBorderPen.style() != Qt::NoPen )
	style->m_featuresSet |= SLeftBorder;
    return style;
  }

  m_leftBorderPen = pen;
  m_leftPenValue = calculateValue( pen );
  if ( m_leftBorderPen.style() != Qt::NoPen )
      m_featuresSet |= SLeftBorder;
  return this;
}

Style * Style::setTopBorderPen( QPen const & pen )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    Style * style = new Style( this );
    style->m_topBorderPen = pen;
    style->m_topPenValue = calculateValue( pen );
    if ( style->m_topBorderPen.style() != Qt::NoPen )
	style->m_featuresSet |= STopBorder;
    return style;
  }

  m_topBorderPen = pen;
  m_topPenValue = calculateValue( pen );
  if ( m_topBorderPen.style() != Qt::NoPen )
      m_featuresSet |= STopBorder;
  return this;
}

Style * Style::setFallDiagonalPen( QPen const & pen )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    Style * style = new Style( this );
    style->m_fallDiagonalPen = pen;
    if ( style->m_fallDiagonalPen.style() != Qt::NoPen )
	style->m_featuresSet |= SFallDiagonal;
    return style;
  }

  m_fallDiagonalPen = pen;
  if ( m_fallDiagonalPen.style() != Qt::NoPen )
      m_featuresSet |= SFallDiagonal;
  return this;
}

Style * Style::setGoUpDiagonalPen( QPen const & pen )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    Style * style = new Style( this );
    style->m_goUpDiagonalPen = pen;
    if ( style->m_goUpDiagonalPen.style() != Qt::NoPen )
	style->m_featuresSet |= SGoUpDiagonal;
    return style;
  }

  m_goUpDiagonalPen = pen;
  if ( m_goUpDiagonalPen.style() != Qt::NoPen )
      m_featuresSet |= SGoUpDiagonal;
  return this;
}

Style * Style::setRotateAngle( int angle )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    Style * style = new Style( this );
    style->m_rotateAngle = angle;
    style->m_featuresSet |= SAngle;
    return style;
  }

  m_rotateAngle = angle;
  m_featuresSet |= SAngle;
  return this;
}

Style * Style::setIndent( double indent )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    Style * style = new Style( this );
    style->m_indent = indent;
    style->m_featuresSet |= SIndent;
    return style;
  }

  m_indent = indent;
  m_featuresSet |= SIndent;
  return this;
}

Style * Style::setBackGroundBrush( QBrush const & brush )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    Style * style = new Style( this );
    style->m_backGroundBrush = brush;
    if ( style->m_backGroundBrush.style() != Qt::NoBrush )
	style->m_featuresSet |= SBackgroundBrush;
    return style;
  }

  m_backGroundBrush = brush;
  if ( m_backGroundBrush.style() != Qt::NoBrush )
      m_featuresSet |= SBackgroundBrush;
  return this;
}

Style * Style::setFloatFormat( FloatFormat format )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    Style * style = new Style( this );
    style->m_floatFormat = format;
    style->m_featuresSet |= SFloatFormat;
    return style;
  }

  m_floatFormat = format;
  m_featuresSet |= SFloatFormat;
  return this;
}

Style * Style::setFloatColor( FloatColor color )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    Style * style = new Style( this );
    style->m_floatColor = color;
    style->m_featuresSet |= SFloatColor;
    return style;
  }

  m_floatColor = color;
  m_featuresSet |= SFloatColor;
  return this;
}

Style * Style::setStrFormat( QString const & strFormat )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    Style * style = new Style( this );
    style->m_strFormat = strFormat;
    style->m_featuresSet |= SCustomFormat;
    return style;
  }

  m_strFormat = strFormat;
  m_featuresSet |= SCustomFormat;
  return this;
}

Style * Style::setPrecision( int precision )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    Style * style = new Style( this );
    style->m_precision = precision;
    style->m_featuresSet |= SPrecision;
    return style;
  }

  m_precision = precision;
  m_featuresSet |= SPrecision;
  return this;
}

Style * Style::setPrefix( QString const & prefix )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    Style * style = new Style( this );
    style->m_prefix = prefix;
    style->m_featuresSet |= SPrefix;
    return style;
  }

  m_prefix = prefix;
  m_featuresSet |= SPrefix;
  return this;
}

Style * Style::setPostfix( QString const & postfix )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    Style * style = new Style( this );
    style->m_postfix = postfix;
    style->m_featuresSet |= SPostfix;
    return style;
  }

  m_postfix = postfix;
  m_featuresSet |= SPostfix;
  return this;
}

Style * Style::setCurrency( Currency const & currency )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    Style * style = new Style( this );
    style->m_currency = currency;
    style->m_featuresSet |= SFormatType;
    return style;
  }

  m_currency = currency;
  m_featuresSet |= SFormatType;
  return this;
}

Style * Style::setProperty( FlagsSet p )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    kDebug() << k_funcinfo << endl;
    kDebug() << "m_type != AUTO || m_usageCount > 1" << endl;
    Style * style = new Style( this );
    style->m_properties |= (uint) p;
    switch( p )
    {
     case SDontPrintText:
      style->m_featuresSet |= SDontPrintText;
      break;
     case SCustomFormat:
      style->m_featuresSet |= SCustomFormat;
      break;
     case SNotProtected:
      style->m_featuresSet |= SNotProtected;
      break;
     case SHideAll:
      style->m_featuresSet |= SHideAll;
      break;
     case SHideFormula:
      style->m_featuresSet |= SHideFormula;
      break;
     case SMultiRow:
      style->m_featuresSet |= SMultiRow;
      break;
     case SVerticalText:
      style->m_featuresSet |= SVerticalText;
      break;
     default:
      kWarning() << "Unhandled property" << endl;
    }
    return style;
  }

  m_properties |= (uint) p;
  switch( p )
  {
   case SDontPrintText:
    m_featuresSet |= SDontPrintText;
    break;
   case SCustomFormat:
    m_featuresSet |= SCustomFormat;
    break;
   case SNotProtected:
     kDebug() << k_funcinfo << endl;
     kDebug() << "case SNotProtected" << endl;
     m_featuresSet |= SNotProtected;
    break;
   case SHideAll:
    m_featuresSet |= SHideAll;
    break;
   case SHideFormula:
    m_featuresSet |= SHideFormula;
    break;
   case SMultiRow:
    m_featuresSet |= SMultiRow;
    break;
   case SVerticalText:
    m_featuresSet |= SVerticalText;
    break;
   default:
    kWarning() << "Unhandled property" << endl;
  }
  return this;
}

Style * Style::clearProperty( FlagsSet p )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    Style * style = new Style( this );
    style->m_properties &= ~(uint) p;
    switch( p )
    {
     case SDontPrintText:
      style->m_featuresSet |= SDontPrintText;
      break;
     case SCustomFormat:
      style->m_featuresSet |= SCustomFormat;
      break;
     case SNotProtected:
      style->m_featuresSet |= SNotProtected;
      break;
     case SHideAll:
      style->m_featuresSet |= SHideAll;
      break;
     case SHideFormula:
      style->m_featuresSet |= SHideFormula;
      break;
     case SMultiRow:
      style->m_featuresSet |= SMultiRow;
      break;
     case SVerticalText:
      style->m_featuresSet |= SVerticalText;
      break;
     default:
      kWarning() << "Unhandled property" << endl;
    }
    return style;
  }

  m_properties &= ~(uint) p;
  switch( p )
  {
   case SDontPrintText:
    m_featuresSet |= SDontPrintText;
    break;
   case SCustomFormat:
    m_featuresSet |= SCustomFormat;
    break;
   case SNotProtected:
    m_featuresSet |= SNotProtected;
    break;
   case SHideAll:
    m_featuresSet |= SHideAll;
    break;
   case SHideFormula:
    m_featuresSet |= SHideFormula;
    break;
   case SMultiRow:
    m_featuresSet |= SMultiRow;
    break;
   case SVerticalText:
    m_featuresSet |= SVerticalText;
    break;
   default:
    kWarning() << "Unhandled property" << endl;
  }
  return this;
}


Style * Style::setFormatType( FormatType format )
{
  if ( m_type != AUTO || m_usageCount > 1 )
  {
    Style * style  = new Style( this );
    style->m_formatType  = format;
    style->m_featuresSet |= SFormatType;
    return style;
  }

  m_formatType  = format;
  m_featuresSet |= SFormatType;
  return this;
}

QString Style::colorName( const QColor& color )
{
	static QMap<QRgb , QString> map;

	QRgb rgb = color.rgb();

	if (!map.contains( rgb ))
	{
		map[rgb] = color.name();
		return map[rgb];
	}
	else
	{
		return map[rgb];
	}
}

/**
 * ************************************************************
 * CustomStyle
 * ************************************************************
 */

CustomStyle::CustomStyle()
  : Style(),
    m_name( "Default" )
{
  m_type   = BUILTIN;
  m_parent = 0;
}

CustomStyle::CustomStyle( Style * parent, QString const & name )
  : Style(),
    m_name( name )
{
  m_type   = CUSTOM;
  m_parent = 0;

  // one to one copy
  if ( parent->hasProperty( SDontPrintText ) )
    addProperty( SDontPrintText );
  if ( parent->hasProperty( SCustomFormat ) )
    addProperty( SCustomFormat );
  if ( parent->hasProperty( SNotProtected ) )
    addProperty( SNotProtected );
  if ( parent->hasProperty( SHideAll ) )
    addProperty( SHideAll );
  if ( parent->hasProperty( SHideFormula ) )
    addProperty( SHideFormula );
  if ( parent->hasProperty( SMultiRow ) )
    addProperty( SMultiRow );
  if ( parent->hasProperty( SVerticalText ) )
    addProperty( SVerticalText );

  changeHAlign( parent->halign() );
  changeVAlign( parent->valign() );
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

CustomStyle::CustomStyle( QString const & name, CustomStyle * parent )
  : Style(),
    m_name( name )
{
  m_parent = parent;
  if ( m_parent )
    m_parentName = m_parent->name();
}

CustomStyle::~CustomStyle()
{
}

QString CustomStyle::saveOasis( KoGenStyle& style, KoGenStyles &mainStyles )
{
    // TODO:
    // If the type is undefined, we're called from Format
    // and the OASIS style is not an automatic style. As
    // the user styles are already created, we return here.
    // TODO: CUSTOM styles
//     if ( style.type() == 0 && ( m_type == BUILTIN ) && ( m_name == "Default" ) )
//       return "Default";

    if ( m_name.isEmpty() )
        return QString::null; // TODO fallback to Style::saveOasis() ???

    // default style does not need display name
    if( type() != BUILTIN || m_name != "Default" )
        style.addAttribute( "style:display-name", m_name );

    // doing the real work
    saveOasisStyle( style, mainStyles );

    // The lookup is done in the calling object (Format).
    if ( style.type() == Doc::STYLE_CELL_AUTO )
        return QString::null;

    if( ( m_type == BUILTIN ) && ( m_name == "Default" ) )
    {
        style.setDefaultStyle(true);
        // don't i18n'ize "Default" in this case
        return mainStyles.lookup( style, "Default", KoGenStyles::DontForceNumbering );
    }
    else
        // this is a custom style
        return mainStyles.lookup( style, "custom-style" );
}

void CustomStyle::loadOasis( KoOasisStyles& oasisStyles, const QDomElement& style, const QString & name )
{
    m_name = name;
    if ( style.hasAttributeNS( KoXmlNS::style, "parent-style-name" ) )
        m_parentName = style.attributeNS( KoXmlNS::style, "parent-style-name", QString::null );
    else if ( m_name != "Default" )
        m_parentName = "Default";

    m_type = CUSTOM;

    Style::loadOasisStyle( oasisStyles, style );
}

void CustomStyle::save( QDomDocument & doc, QDomElement & styles )
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

bool CustomStyle::loadXML( QDomElement const & style, QString const & name )
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
    if ( !Style::loadXML( f ) )
      return false;

  return true;
}

void CustomStyle::setName( QString const & name )
{
  m_name = name;
}

void CustomStyle::refreshParentName()
{
  if ( m_parent )
    m_parentName = m_parent->name();
}

bool CustomStyle::definesAll() const
{
  if ( !( m_featuresSet & (uint) SHAlign ) )
    return false;
  if ( !( m_featuresSet & (uint) SVAlign ) )
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

void CustomStyle::changeHAlign( HAlign alignX )
{
  m_alignX = alignX;
  m_featuresSet |= SHAlign;
}

void CustomStyle::changeVAlign( VAlign alignY )
{
  m_alignY = alignY;
  m_featuresSet |= SVAlign;
}

void CustomStyle::changeFont( QFont const & f )
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

void CustomStyle::changeFontFamily( QString const & fam )
{
  if ( m_fontFamily != fam )
  {
    m_fontFamily   = fam;
    m_featuresSet |= SFont;
    m_featuresSet |= SFontFamily;
  }
}

void CustomStyle::changeFontSize( int size )
{
  if ( m_fontSize != size )
  {
    m_fontSize     = size;
    m_featuresSet |= SFont;
    m_featuresSet |= SFontSize;
  }
}

void CustomStyle::changeFontFlags( uint flags )
{
  if ( m_fontFlags != flags )
  {
    m_fontFlags    = flags;
    m_featuresSet |= SFont;
    m_featuresSet |= SFontFlag;
  }
}

void CustomStyle::changeTextColor( QColor const & color )
{
  m_textPen.setColor( color );
  m_featuresSet |= STextPen;
}

void CustomStyle::changePen( QPen const & pen )
{
  m_textPen = pen;
  m_featuresSet |= STextPen;
}

void CustomStyle::changeBgColor( QColor const & color )
{
  m_bgColor = color;
  m_featuresSet |= SBackgroundColor;
}

void CustomStyle::changeRightBorderPen( QPen const & pen )
{
  m_rightBorderPen = pen;
  m_rightPenValue  = calculateValue( pen );
  m_featuresSet   |= SRightBorder;
}

void CustomStyle::changeBottomBorderPen( QPen const & pen )
{
  m_bottomBorderPen = pen;
  m_bottomPenValue  = calculateValue( pen );
  m_featuresSet    |= SBottomBorder;
}

void CustomStyle::changeLeftBorderPen( QPen const & pen )
{
  m_leftBorderPen = pen;
  m_leftPenValue  = calculateValue( pen );
  m_featuresSet  |= SLeftBorder;
}

void CustomStyle::changeTopBorderPen( QPen const & pen )
{
  m_topBorderPen = pen;
  m_topPenValue  = calculateValue( pen );
  m_featuresSet |= STopBorder;
}

void CustomStyle::changeFallBorderPen( QPen const & pen )
{
  m_fallDiagonalPen = pen;
  m_featuresSet |= SFallDiagonal;
}

void CustomStyle::changeGoUpBorderPen( QPen const & pen )
{
  m_goUpDiagonalPen = pen;
  m_featuresSet |= SGoUpDiagonal;
}

void CustomStyle::changeRotateAngle( int angle )
{
  m_rotateAngle = angle;
  m_featuresSet |= SAngle;
}

void CustomStyle::changeIndent( double indent )
{
  m_indent = indent;
  m_featuresSet |= SIndent;
}

void CustomStyle::changeBackGroundBrush( QBrush const & brush )
{
  m_backGroundBrush = brush;
  m_featuresSet |= SBackgroundBrush;
}

void CustomStyle::changeFloatFormat( FloatFormat format )
{
  m_floatFormat = format;
  m_featuresSet |= SFloatFormat;
}

void CustomStyle::changeFloatColor( FloatColor color )
{
  m_floatColor = color;
  m_featuresSet |= SFloatColor;
}

void CustomStyle::changeFormatType( FormatType format )
{
  m_formatType = format;
  m_featuresSet |= SFormatType;
}

void CustomStyle::changeStrFormat( QString const & strFormat )
{
  m_strFormat = strFormat;
  m_featuresSet |= SCustomFormat;
}

void CustomStyle::changePrecision( int precision )
{
  m_precision = precision;
  m_featuresSet |= SPrecision;
}

void CustomStyle::changePrefix( QString const & prefix )
{
  m_prefix = prefix;
  m_featuresSet |= SPrefix;
}

void CustomStyle::changePostfix( QString const & postfix )
{
  m_postfix = postfix;
  m_featuresSet |= SPostfix;
}

void CustomStyle::changeCurrency( Currency const & currency )
{
  m_currency = currency;
}

void CustomStyle::addProperty( FlagsSet p )
{
  m_properties |= (uint) p;
  switch( p )
  {
   case SDontPrintText:
    m_featuresSet |= SDontPrintText;
    break;
   case SCustomFormat:
    m_featuresSet |= SCustomFormat;
    break;
   case SNotProtected:
    m_featuresSet |= SNotProtected;
    break;
   case SHideAll:
    m_featuresSet |= SHideAll;
    break;
   case SHideFormula:
    m_featuresSet |= SHideFormula;
    break;
   case SMultiRow:
    m_featuresSet |= SMultiRow;
    break;
   case SVerticalText:
    m_featuresSet |= SVerticalText;
    break;
   default:
    kWarning() << "Unhandled property" << endl;
  }
}

void CustomStyle::removeProperty( FlagsSet p )
{
  m_properties &= ~(uint) p;
  switch( p )
  {
   case SDontPrintText:
    m_featuresSet &= SDontPrintText;
    break;
   case SCustomFormat:
    m_featuresSet &= SCustomFormat;
    break;
   case SNotProtected:
    m_featuresSet &= SNotProtected;
    break;
   case SHideAll:
    m_featuresSet &= SHideAll;
    break;
   case SHideFormula:
    m_featuresSet &= SHideFormula;
    break;
   case SMultiRow:
    m_featuresSet &= SMultiRow;
    break;
   case SVerticalText:
    m_featuresSet &= SVerticalText;
    break;
   default:
    kWarning() << "Unhandled property" << endl;
  }
}



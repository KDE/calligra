/* This file is part of the KDE project
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2003 Norbert Andres <nandres@web.de>

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
   Boston, MA 02110-1301, USA.
*/

#include <QBrush>
#include <QHash>
#include <QPen>

#include <kdebug.h>
#include <klocale.h>

#include <KoGenStyles.h>
#include <KoGlobal.h>
#include <KoOasisStyles.h>
#include <KoStyleStack.h>
#include <KoUnit.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>

#include "Doc.h"
#include "Global.h"
#include "Util.h"

#include "Style.h"

using namespace KSpread;

/////////////////////////////////////////////////////////////////////////////
//
// SubStyles
//
/////////////////////////////////////////////////////////////////////////////

static uint calculateValue( QPen const & pen )
{
  uint n = pen.color().red() + pen.color().green() + pen.color().blue();
  n += 1000 * pen.width();
  n += 10000 * (uint) pen.style();
  return n;
}

template<Style::Key key, class Value1>
class SubStyleOne : public SubStyle
{
public:
    SubStyleOne( const Value1& v = Value1() ) : SubStyle(), value1( v ) {}
    virtual Style::Key type() const { return key; }
    virtual void dump() const { kDebug(36006) << debugData() << endl; }
    virtual QString debugData( bool withName = true ) const
    { QString out; if (withName) out = name(key) + ' '; QDebug qdbg(&out); qdbg << value1; return out; }
    Value1 value1;
};

// specialized debug method
template<>
QString SubStyleOne<Style::CurrencyFormat, Currency>::debugData( bool withName ) const
{ QString out; if (withName) out = name(Style::CurrencyFormat) + ' '; QDebug qdbg(&out); qdbg << value1.symbol(); return out; }

template<Style::Key key>
class PenStyle : public SubStyleOne<key, QPen>
{
public:
    PenStyle( const QPen& p = Qt::NoPen ) : SubStyleOne<key, QPen>( p ) {}
};

template<Style::Key key>
class BorderPenStyle : public PenStyle<key>
{
public:
    BorderPenStyle( const QPen& p = Qt::NoPen ) : PenStyle<key>(p), value( calculateValue(p) ) {}
    int value;
};

QString SubStyle::name( Style::Key key )
{
    QString name;
    switch ( key )
    {
        case Style::DefaultStyleKey:        name = "Default style"; break;
        case Style::NamedStyleKey:          name = "Named style"; break;
        case Style::LeftPen:                name = "Left pen"; break;
        case Style::RightPen:               name = "Right pen"; break;
        case Style::TopPen:                 name = "Top pen"; break;
        case Style::BottomPen:              name = "Bottom pen"; break;
        case Style::FallDiagonalPen:        name = "Fall diagonal pen"; break;
        case Style::GoUpDiagonalPen:        name = "Go up diagonal pen"; break;
        case Style::HorizontalAlignment:    name = "Horz. alignment"; break;
        case Style::VerticalAlignment:      name = "Vert. alignment"; break;
        case Style::MultiRow:               name = "Wrap text"; break;
        case Style::VerticalText:           name = "Vertical text"; break;
        case Style::Angle:                  name = "Angle"; break;
        case Style::Indentation:            name = "Indentation"; break;
        case Style::Prefix:                 name = "Prefix"; break;
        case Style::Postfix:                name = "Postfix"; break;
        case Style::Precision:              name = "Precision"; break;
        case Style::FormatTypeKey:          name = "Format type"; break;
        case Style::FloatFormatKey:         name = "Float format"; break;
        case Style::FloatColorKey:          name = "Float color"; break;
        case Style::CurrencyFormat:         name = "Currency"; break;
        case Style::CustomFormat:           name = "Custom format"; break;
        case Style::BackgroundBrush:        name = "Background brush"; break;
        case Style::BackgroundColor:        name = "Background color"; break;
        case Style::FontColor:              name = "Font color"; break;
        case Style::FontFamily:             name = "Font family"; break;
        case Style::FontSize:               name = "Font size"; break;
        case Style::FontBold:               name = "Font bold"; break;
        case Style::FontItalic:             name = "Font italic"; break;
        case Style::FontStrike:             name = "Font strikeout"; break;
        case Style::FontUnderline:          name = "Font underline"; break;
        case Style::DontPrintText:          name = "Dont print text"; break;
        case Style::NotProtected:           name = "Not protected"; break;
        case Style::HideAll:                name = "Hide all"; break;
        case Style::HideFormula:            name = "Hide formula"; break;
    }
    return name;
}

/////////////////////////////////////////////////////////////////////////////
//
// Style::Private
//
/////////////////////////////////////////////////////////////////////////////

class Style::Private : public QSharedData
{
public:
    QHash<Key, SharedSubStyle> subStyles;
    StyleType type;
};


/////////////////////////////////////////////////////////////////////////////
//
// Style
//
/////////////////////////////////////////////////////////////////////////////

Style::Style()
    : d( new Private )
{
}

Style::Style( const Style& style )
    : d( style.d )
{
}

Style::~Style()
{
}

Style::StyleType Style::type() const
{
    return AUTO;
}

QString Style::parentName() const
{
    if ( !d->subStyles.contains( NamedStyleKey ) )
        return QString();
    return static_cast<const NamedStyle*>( d->subStyles[NamedStyleKey].data() )->name;
}

void Style::setParentName( const QString& name )
{
    d->subStyles.remove( NamedStyleKey );
    d->subStyles.insert( NamedStyleKey, SharedSubStyle( new NamedStyle( name ) ) );
}

void Style::clearAttribute( Key key )
{
    d->subStyles.remove( key );
}

bool Style::hasAttribute( Key key ) const
{
    return d->subStyles.contains( key );
}

void Style::loadAttributes( const QList<SharedSubStyle>& subStyles )
{
    d->subStyles.clear();
    for ( int i = 0; i < subStyles.count(); ++i )
    {
        // already existing items are replaced
        d->subStyles.insert( subStyles[i]->type(), subStyles[i] );
    }
}

void Style::loadOasisStyle( KoOasisStyles& oasisStyles, const KoXmlElement & element )
{
  // NOTE Stefan: Don't fill the style stack with the parent styles!
    KoStyleStack styleStack;
    styleStack.push( element );
    styleStack.setTypeProperties( "table-cell" );
    loadOasisTableCellProperties( oasisStyles, styleStack );
    styleStack.setTypeProperties( "text" );
    loadOasisTextProperties( oasisStyles, styleStack );
    styleStack.setTypeProperties( "paragraph" );
    loadOasisParagraphProperties( oasisStyles, styleStack );

    loadOasisDataStyle( oasisStyles, element );
}

void Style::loadOasisDataStyle( KoOasisStyles& oasisStyles, const KoXmlElement& element )
{
    QString str;
    if ( element.hasAttributeNS( KoXmlNS::style, "data-style-name" ) )
    {
        const QString styleName = element.attributeNS( KoXmlNS::style, "data-style-name", QString() );
        if ( oasisStyles.dataFormats().contains(styleName) )
        {
            const KoOasisStyles::NumericStyleFormat dataStyle = oasisStyles.dataFormats()[styleName];

            QString tmp = dataStyle.prefix;
            if ( !tmp.isEmpty() )
            {
                setPrefix( tmp );
            }
            tmp = dataStyle.suffix;
            if ( !tmp.isEmpty() )
            {
                setPostfix( tmp );
            }
            // determine data formatting
            switch (dataStyle.type)
            {
                case KoOasisStyles::NumericStyleFormat::Number:
                    setFormatType( Format::Number );
                    break;
                case KoOasisStyles::NumericStyleFormat::Scientific:
                    setFormatType( Format::Scientific );
                    break;
                case KoOasisStyles::NumericStyleFormat::Currency:
                    kDebug(36003) << " currency-symbol: " << dataStyle.currencySymbol << endl;
                    if (!dataStyle.currencySymbol.isEmpty())
                    {
                        setCurrency( Currency(dataStyle.currencySymbol) );
                    }
                    break;
                case KoOasisStyles::NumericStyleFormat::Percentage:
                    setFormatType( Format::Percentage );
                    break;
                case KoOasisStyles::NumericStyleFormat::Fraction:
                    // determine format of fractions, dates and times by using the
                    // formatting string
                    tmp = dataStyle.formatStr;
                    if ( !tmp.isEmpty() )
                    {
                        setFormatType( Style::fractionType( tmp ) );
                    }
                    break;
                case KoOasisStyles::NumericStyleFormat::Date:
                    // determine format of fractions, dates and times by using the
                    // formatting string
                    tmp = dataStyle.formatStr;
                    if ( !tmp.isEmpty() )
                    {
                        setFormatType( Style::dateType( tmp ) );
                    }
                    break;
                case KoOasisStyles::NumericStyleFormat::Time:
                    // determine format of fractions, dates and times by using the
                    // formatting string
                    tmp = dataStyle.formatStr;
                    if ( !tmp.isEmpty() )
                    {
                        setFormatType( Style::timeType( tmp ) );
                    }
                    break;
                case KoOasisStyles::NumericStyleFormat::Boolean:
                    setFormatType( Format::Number );
                    break;
                case KoOasisStyles::NumericStyleFormat::Text:
                    setFormatType( Format::Text );
                    break;
            }

            if (dataStyle.precision > -1)
            {
                setPrecision( dataStyle.precision );
            }
        }
    }
}

void Style::loadOasisParagraphProperties( KoOasisStyles& oasisStyles, const KoStyleStack& styleStack )
{
    Q_UNUSED( oasisStyles );
    kDebug(36003) << "\t paragraph-properties" << endl;
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "text-align" ) )
    {
        QString str = styleStack.attributeNS( KoXmlNS::fo, "text-align" );
        if ( str == "center" )
            setHAlign( Style::Center );
        else if ( str == "end" )
            setHAlign( Style::Right );
        else if ( str == "start" )
            setHAlign( Style::Left );
        else
            setHAlign( Style::HAlignUndefined );
        kDebug(36003) << "\t\t text-align: " << str << endl;
    }
}

void Style::loadOasisTableCellProperties( KoOasisStyles& oasisStyles, const KoStyleStack& styleStack )
{
    // precision for the default cell style
    if ( styleStack.hasAttributeNS( KoXmlNS::style, "decimal-places" ) )
    {
        bool ok;
        const int precision = styleStack.attributeNS( KoXmlNS::style, "decimal-places" ).toInt( &ok );
        if ( ok && precision > -1 )
        {
            kDebug(36003) << "\tprecision: " << precision << endl;
            setPrecision( precision );
        }
    }

    QString str;
    if ( styleStack.hasAttributeNS( KoXmlNS::style, "vertical-align" ) )
    {
        str = styleStack.attributeNS( KoXmlNS::style, "vertical-align" );
        if ( str == "bottom" )
            setVAlign( Style::Bottom );
        else if ( str =="top" )
            setVAlign( Style::Top );
        else if ( str =="middle" )
            setVAlign( Style::Middle );
        else
        setVAlign( Style::VAlignUndefined );
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "background-color" ) )
    {
        setBackgroundColor( QColor(  styleStack.attributeNS( KoXmlNS::fo, "background-color" ) ) );
    }

    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "wrap-option" )&&( styleStack.attributeNS( KoXmlNS::fo, "wrap-option" )=="wrap" ) )
    {
        setWrapText( true );
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::style, "cell-protect" ) )
    {
        str = styleStack.attributeNS( KoXmlNS::style, "cell-protect" );
        if ( str=="hidden-and-protected" )
        {
            setHideAll( true );
        }
        else if ( str == "protected formula-hidden" )
        {
            setHideFormula( true );
        }
        else if ( str == "protected" )
        {
            setNotProtected( false );
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
    if ( styleStack.hasAttributeNS( KoXmlNS::style, "print-content" ) &&
         ( styleStack.attributeNS( KoXmlNS::style, "print-content" )=="false" ) )
    {
        setDontPrintText( true );
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::style, "direction" ) &&
         ( styleStack.attributeNS( KoXmlNS::style, "direction" )=="ttb" ) )
    {
        setVerticalText( true );
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::style, "rotation-angle" ) )
    {
        bool ok;
        int a = styleStack.attributeNS( KoXmlNS::style, "rotation-angle" ).toInt( &ok );
        kDebug(36003)<<" rotation-angle :"<<a<<endl;
        if ( a != 0 )
        {
            setAngle( -a  );
        }
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "margin-left" ) )
    {
        //todo fix me
        setIndentation( KoUnit::parseValue( styleStack.attributeNS( KoXmlNS::fo, "margin-left" ),0.0 ) );
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "border" ) )
    {
        str=styleStack.attributeNS( KoXmlNS::fo, "border" );
        QPen pen = Oasis::decodePen( str );
        setLeftBorderPen( pen );
        setTopBorderPen( pen );
        setBottomBorderPen( pen );
        setRightBorderPen( pen );
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "border-left" ) )
    {
        str=styleStack.attributeNS( KoXmlNS::fo, "border-left" );
        setLeftBorderPen( Oasis::decodePen( str ) );
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "border-right" ) )
    {
        str=styleStack.attributeNS( KoXmlNS::fo, "border-right" );
        setRightBorderPen( Oasis::decodePen( str ) );
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "border-top" ) )
    {
        str=styleStack.attributeNS( KoXmlNS::fo, "border-top" );
        setTopBorderPen( Oasis::decodePen( str ) );
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "border-bottom" ) )
    {
        str=styleStack.attributeNS( KoXmlNS::fo, "border-bottom" );
        setBottomBorderPen( Oasis::decodePen( str ) );
    }
    if (styleStack.hasAttributeNS( KoXmlNS::style, "diagonal-tl-br" ) )
    {
        str=styleStack.attributeNS( KoXmlNS::style, "diagonal-tl-br" );
        setFallDiagonalPen( Oasis::decodePen( str ) );
    }
    if (styleStack.hasAttributeNS( KoXmlNS::style, "diagonal-bl-tr" ) )
    {
        str=styleStack.attributeNS( KoXmlNS::style, "diagonal-bl-tr" );
        setGoUpDiagonalPen( Oasis::decodePen( str ) );
    }

    if ( styleStack.hasAttributeNS( KoXmlNS::draw, "style-name" ) )
    {
        kDebug(36003)<<" style name :"<<styleStack.attributeNS( KoXmlNS::draw, "style-name" )<<endl;

        const KoXmlElement * style = oasisStyles.findStyle( styleStack.attributeNS( KoXmlNS::draw, "style-name" ), "graphic" );
        kDebug(36003)<<" style :"<<style<<endl;
        if ( style )
        {
            KoStyleStack drawStyleStack;
            drawStyleStack.push( *style );
            drawStyleStack.setTypeProperties( "graphic" );
            if ( drawStyleStack.hasAttributeNS( KoXmlNS::draw, "fill" ) )
            {
                const QString fill = drawStyleStack.attributeNS( KoXmlNS::draw, "fill" );
                kDebug(36003)<<" load object gradient fill type :"<<fill<<endl;

                if ( fill == "solid" || fill == "hatch" )
                {
                    kDebug(36003)<<" Style ******************************************************\n";
                    setBackgroundBrush( KoOasisStyles::loadOasisFillStyle( drawStyleStack, fill, oasisStyles ) );

                }
                else
                    kDebug(36003)<<" fill style not supported into kspread : "<<fill<<endl;
            }
        }
    }
}

void Style::loadOasisTextProperties( KoOasisStyles& oasisStyles, const KoStyleStack& styleStack )
{
    Q_UNUSED( oasisStyles );
  // fo:font-size="13pt"
  // fo:font-style="italic"
  // style:text-underline="double"
  // style:text-underline-color="font-color"
  // fo:font-weight="bold"
    kDebug(36003) << "\t text-properties" << endl;
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "font-family" ) )
    {
        setFontFamily( styleStack.attributeNS( KoXmlNS::fo, "font-family" ) ); // FIXME Stefan: sanity check
        kDebug(36003) << "\t\t fo:font-family: " << fontFamily() << endl;
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "font-size" ) )
    {
        setFontSize( (int) KoUnit::parseValue( styleStack.attributeNS( KoXmlNS::fo, "font-size" ), 10.0 ) ); // FIXME Stefan: fallback to default
        kDebug(36003) << "\t\t fo:font-size: " << fontSize() << endl;
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "font-style" ) )
    {
        if ( styleStack.attributeNS( KoXmlNS::fo, "font-style" ) == "italic" ) // "normal", "oblique"
        {
            setFontItalic( true );
            kDebug(36003) << "\t\t fo:font-style: " << "italic" << endl;
        }
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "font-weight" ) )
    {
        if ( styleStack.attributeNS( KoXmlNS::fo, "font-weight" ) == "bold" ) // "normal", "100", "200", ...
        {
            setFontBold( true );
            kDebug(36003) << "\t\t fo:font-weight: " << "bold" << endl;
        }
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::style, "text-underline-style" ) )
    {
        if ( styleStack.attributeNS( KoXmlNS::style, "text-underline-style" ) != "none" )
        {
            setFontUnderline( true );
            kDebug(36003) << "\t\t style:text-underline-style: " << "solid (actually: !none)" << endl;
        }
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::style, "text-underline-width" ) )
    {
        //TODO
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::style, "text-underline-color" ) )
    {
        //TODO
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "color" ) )
    {
        QColor color = QColor( styleStack.attributeNS( KoXmlNS::fo, "color" ) );
        if ( color.isValid() )
        {
            setFontColor( color );
            kDebug(36003) << "\t\t fo:color: " << color.name() << endl;
        }
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::style, "text-line-through-style" ) )
    {
        if ( styleStack.attributeNS( KoXmlNS::style, "text-line-through-style" ) != "none"
         /*&& styleStack.attributeNS("text-line-through-style")=="solid"*/ )
        {
            setFontStrikeOut( true );
            kDebug(36003) << "\t\t text-line-through-style: " << "solid (actually: !none)" << endl;
        }
    }
}

static QString convertDateFormat( const QString& date )
{
    QString result = date;
    result.replace( "%Y", "yyyy" );
    result.replace( "%y", "yy" );
    result.replace( "%n", "M" );
    result.replace( "%m", "MM" );
    result.replace( "%e", "d" );
    result.replace( "%d", "dd" );
    result.replace( "%b", "MMM" );
    result.replace( "%B", "MMMM" );
    result.replace( "%a", "ddd" );
    result.replace( "%A", "dddd" );
    return result;
}

Format::Type Style::dateType( const QString &_format )
{
    const QString dateFormatShort = convertDateFormat( KGlobal::locale()->dateFormatShort() );
    const QString dateFormat = convertDateFormat( KGlobal::locale()->dateFormat() );

    if ( _format == "dd-MMM-yy" )
        return Format::Date1;
    else if ( _format == "dd-MMM-yyyy" )
        return Format::Date2;
    else if ( _format == "d-MM" )
        return Format::Date3;
    else if ( _format == "dd-MM" ) //TODO ???????
        return Format::Date4;
    else if ( _format == "dd/MM/yy" )
        return Format::Date5;
    else if ( _format == "dd/MM/yyyy" )
        return Format::Date6;
    else if ( _format == "MMM-yy" )
        return Format::Date7;
    else if ( _format == "MMMM-yyyy" )
        return Format::Date9;
    else if ( _format == "MMMMM-yy" )
        return Format::Date10;
    else if ( _format == "dd/MMM" )
        return Format::Date11;
    else if ( _format == "dd/MM" )
        return Format::Date12;
    else if ( _format == "dd/MMM/yyyy" )
        return Format::Date13;
    else if ( _format == "yyyy/MMM/dd" )
        return Format::Date14;
    else if ( _format == "yyyy-MMM-dd" )
        return Format::Date15;
    else if ( _format == "yyyy/MM/dd" )
        return Format::Date16;
    else if ( _format == "d MMMM yyyy" )
        return Format::Date17;
    else if ( _format == "MM/dd/yyyy" )
        return Format::Date18;
    else if ( _format == "MM/dd/yy" )
        return Format::Date19;
    else if ( _format == "MMM/dd/yy" )
        return Format::Date20;
    else if ( _format == "MMM/dd/yyyy" )
        return Format::Date21;
    else if ( _format == "MMM-yyyy" )
        return Format::Date22;
    else if ( _format == "yyyy" )
        return Format::Date23;
    else if ( _format == "yy" )
        return Format::Date24;
    else if ( _format == "yyyy/MM/dd" )
        return Format::Date25;
    else if ( _format == "yyyy/MMM/dd" )
        return Format::Date26;
    else if ( _format == dateFormatShort )
        return Format::ShortDate;
    else if ( _format == dateFormat )
        return Format::TextDate;
    else
        return Format::ShortDate;
}

Format::Type Style::timeType( const QString &_format )
{
    if ( _format == "h:mm AP" )
        return Format::Time1;
    else if ( _format == "h:mm:ss AP" )
        return Format::Time2;
    else if ( _format == "hh \\h mm \\m\\i\\n ss \\s" )
        return Format::Time3;
    else if ( _format == "hh:mm" )
        return Format::Time4;
    else if ( _format == "hh:mm:ss" )
        return Format::Time5;
    else if ( _format == "m:ss" )
        return Format::Time6;
    else if ( _format == "h:mm:ss" )
        return Format::Time7;
    else if ( _format == "h:mm" )
        return Format::Time8;
    else
        return Format::Time;
}

Format::Type Style::fractionType( const QString &_format )
{
    if ( _format == "# ?/2" )
        return Format::fraction_half;
    else if ( _format =="# ?/4" )
        return Format::fraction_quarter;
    else if ( _format == "# ?/8" )
        return Format::fraction_eighth;
    else if ( _format == "# ?/16" )
        return Format::fraction_sixteenth;
    else if ( _format == "# ?/10" )
        return Format::fraction_tenth;
    else if ( _format == "# ?/100" )
        return Format::fraction_hundredth;
    else if ( _format == "# ?/?" )
        return Format::fraction_one_digit;
    else if ( _format == "# \?\?/\?\?" )
        return Format::fraction_two_digits;
    else if ( _format == "# \?\?\?/\?\?\?" )
        return Format::fraction_three_digits;
    else
        return Format::fraction_half;
}

QString Style::saveOasisStyleNumeric( KoGenStyle &style, KoGenStyles &mainStyles,
                                         Format::Type _style,
                                         const QString &_prefix, const QString &_postfix,
                                         int _precision, const QString& symbol )
{
//  kDebug(36003) << k_funcinfo << endl;
    QString styleName;
    QString valueType;
    switch( _style )
    {
        case Format::Number:
            styleName = saveOasisStyleNumericNumber( mainStyles, _style, _precision, _prefix, _postfix );
            valueType = "float";
            break;
        case Format::Text:
            styleName = saveOasisStyleNumericText( mainStyles, _style, _precision, _prefix, _postfix );
            valueType = "string";
            break;
        case Format::Money:
            styleName = saveOasisStyleNumericMoney( mainStyles, _style, symbol, _precision, _prefix, _postfix );
            valueType = "currency";
            break;
        case Format::Percentage:
            styleName = saveOasisStyleNumericPercentage( mainStyles, _style, _precision, _prefix, _postfix );
            valueType = "percentage";
            break;
        case Format::Scientific:
            styleName = saveOasisStyleNumericScientific( mainStyles, _style, _prefix, _postfix, _precision );
            valueType = "float";
            break;
        case Format::ShortDate:
        case Format::TextDate:
            styleName = saveOasisStyleNumericDate( mainStyles, _style, _prefix, _postfix );
            valueType = "date";
            break;
        case Format::Time:
        case Format::SecondeTime:
        case Format::Time1:
        case Format::Time2:
        case Format::Time3:
        case Format::Time4:
        case Format::Time5:
        case Format::Time6:
        case Format::Time7:
        case Format::Time8:
            styleName = saveOasisStyleNumericTime( mainStyles, _style, _prefix, _postfix );
            valueType = "time";
            break;
        case Format::fraction_half:
        case Format::fraction_quarter:
        case Format::fraction_eighth:
        case Format::fraction_sixteenth:
        case Format::fraction_tenth:
        case Format::fraction_hundredth:
        case Format::fraction_one_digit:
        case Format::fraction_two_digits:
        case Format::fraction_three_digits:
            styleName = saveOasisStyleNumericFraction( mainStyles, _style, _prefix, _postfix );
            valueType = "float";
            break;
        case Format::Date1:
        case Format::Date2:
        case Format::Date3:
        case Format::Date4:
        case Format::Date5:
        case Format::Date6:
        case Format::Date7:
        case Format::Date8:
        case Format::Date9:
        case Format::Date10:
        case Format::Date11:
        case Format::Date12:
        case Format::Date13:
        case Format::Date14:
        case Format::Date15:
        case Format::Date16:
        case Format::Date17:
        case Format::Date18:
        case Format::Date19:
        case Format::Date20:
        case Format::Date21:
        case Format::Date22:
        case Format::Date23:
        case Format::Date24:
        case Format::Date25:
        case Format::Date26:
            styleName = saveOasisStyleNumericDate( mainStyles, _style, _prefix, _postfix );
            valueType = "date";
            break;
        case Format::Custom:
            styleName = saveOasisStyleNumericCustom( mainStyles, _style, _prefix, _postfix );
            break;
        case Format::Generic:
        case Format::None:
            if (_precision > -1 || !_prefix.isEmpty() || !_postfix.isEmpty())
            {
                styleName = saveOasisStyleNumericNumber( mainStyles, _style, _precision, _prefix, _postfix );
                valueType = "float";
            }
            break;
    }
    if ( !styleName.isEmpty() )
    {
        style.addAttribute( "style:data-style-name", styleName );
    }
    return styleName;
}

QString Style::saveOasisStyleNumericNumber( KoGenStyles& mainStyles, Format::Type /*_style*/, int _precision,
                                               const QString& _prefix, const QString& _postfix )
{
    QString format;
    if ( _precision == -1 )
        format='0';
    else
    {
        QString tmp;
        for ( int i = 0; i <_precision; i++ )
        {
            tmp+='0';
        }
        format = "0."+tmp;
    }
    return KoOasisStyles::saveOasisNumberStyle( mainStyles, format, _prefix, _postfix );
}

QString Style::saveOasisStyleNumericText( KoGenStyles& /*mainStyles*/, Format::Type /*_style*/, int /*_precision*/,
                                             const QString& /*_prefix*/, const QString& /*_postfix*/ )
{
    return "";
}

QString Style::saveOasisStyleNumericMoney( KoGenStyles& mainStyles, Format::Type /*_style*/,
                                              const QString& symbol, int _precision,
                                              const QString& _prefix, const QString& _postfix )
{
    QString format;
    if ( _precision == -1 )
        format='0';
    else
    {
        QString tmp;
        for ( int i = 0; i <_precision; i++ )
        {
            tmp+='0';
        }
        format = "0."+tmp;
    }
    return KoOasisStyles::saveOasisCurrencyStyle( mainStyles, format, symbol, _prefix, _postfix );
}

QString Style::saveOasisStyleNumericPercentage( KoGenStyles&mainStyles, Format::Type /*_style*/, int _precision,
        const QString& _prefix, const QString& _postfix )
{
    //<number:percentage-style style:name="N106" style:family="data-style">
    //<number:number number:decimal-places="6" number:min-integer-digits="1"/>
    //<number:text>%</number:text>
    //</number:percentage-style>
    //TODO add decimal etc.
    QString format;
    if ( _precision == -1 )
        format='0';
    else
    {
        QString tmp;
        for ( int i = 0; i <_precision; i++ )
        {
            tmp+='0';
        }
        format = "0."+tmp;
    }
    return KoOasisStyles::saveOasisPercentageStyle( mainStyles, format, _prefix, _postfix );
}


QString Style::saveOasisStyleNumericScientific( KoGenStyles&mainStyles, Format::Type /*_style*/,
        const QString &_prefix, const QString _suffix, int _precision )
{
    //<number:number-style style:name="N60" style:family="data-style">
    //  <number:scientific-number number:decimal-places="2" number:min-integer-digits="1" number:min-exponent-digits="3"/>
    //</number:number-style>
    QString format;
    if ( _precision == -1 )
        format="0E+00";
    else
    {
        QString tmp;
        for ( int i = 0; i <_precision; i++ )
        {
            tmp+='0';
        }
        format = "0."+tmp+"E+00";
    }
    return KoOasisStyles::saveOasisScientificStyle( mainStyles, format, _prefix, _suffix );
}

QString Style::saveOasisStyleNumericDate( KoGenStyles&mainStyles, Format::Type _style,
                                             const QString& _prefix, const QString& _postfix )
{
    QString format;
    bool locale = false;
    switch( _style )
    {
        //TODO fixme use locale of kspread and not kglobal
        case Format::ShortDate:
            format = KGlobal::locale()->dateFormatShort();
            locale = true;
            break;
        case Format::TextDate:
            format = KGlobal::locale()->dateFormat();
            locale = true;
            break;
        case Format::Date1:
            format = "dd-MMM-yy";
            break;
        case Format::Date2:
            format = "dd-MMM-yyyy";
            break;
        case Format::Date3:
            format = "dd-M";
            break;
        case Format::Date4:
            format = "dd-MM";
            break;
        case Format::Date5:
            format = "dd/MM/yy";
            break;
        case Format::Date6:
            format = "dd/MM/yyyy";
            break;
        case Format::Date7:
            format = "MMM-yy";
            break;
        case Format::Date8:
            format = "MMMM-yy";
            break;
        case Format::Date9:
            format = "MMMM-yyyy";
            break;
        case Format::Date10:
            format = "MMMMM-yy";
            break;
        case Format::Date11:
            format = "dd/MMM";
            break;
        case Format::Date12:
            format = "dd/MM";
            break;
        case Format::Date13:
            format = "dd/MMM/yyyy";
            break;
        case Format::Date14:
            format = "yyyy/MMM/dd";
            break;
        case Format::Date15:
            format = "yyyy-MMM-dd";
            break;
        case Format::Date16:
            format = "yyyy/MM/dd";
            break;
        case Format::Date17:
            format = "d MMMM yyyy";
            break;
        case Format::Date18:
            format = "MM/dd/yyyy";
            break;
        case Format::Date19:
            format = "MM/dd/yy";
            break;
        case Format::Date20:
            format = "MMM/dd/yy";
            break;
        case Format::Date21:
            format = "MMM/dd/yyyy";
            break;
        case Format::Date22:
            format = "MMM-yyyy";
            break;
        case Format::Date23:
            format = "yyyy";
            break;
        case Format::Date24:
            format = "yy";
            break;
        case Format::Date25:
            format = "yyyy/MM/dd";
            break;
        case Format::Date26:
            format = "yyyy/MMM/dd";
            break;
        default:
            kDebug(36003)<<"this date format is not defined ! :"<<_style<<endl;
            break;
    }
    return KoOasisStyles::saveOasisDateStyle( mainStyles, format, locale, _prefix, _postfix );
}

QString Style::saveOasisStyleNumericCustom( KoGenStyles& /*mainStyles*/, Format::Type /*_style*/,
                                               const QString& /*_prefix*/, const QString& /*_postfix*/ )
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

QString Style::saveOasisStyleNumericTime( KoGenStyles& mainStyles, Format::Type _style,
                                             const QString& _prefix, const QString& _postfix )
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
        case Format::Time: //TODO FIXME
            format = "hh:mm:ss";
            break;
            case Format::SecondeTime: //TODO FIXME
                format = "hh:mm";
                break;
        case Format::Time1:
            format = "h:mm AP";
            break;
        case Format::Time2:
            format = "h:mm:ss AP";
            break;
            case Format::Time3: // 9 h 01 min 28 s
                format = "hh \\h mm \\m\\i\\n ss \\s";
                break;
        case Format::Time4:
            format = "hh:mm";
            break;
        case Format::Time5:
            format = "hh:mm:ss";
            break;
        case Format::Time6:
            format = "m:ss";
            break;
        case Format::Time7:
            format = "h:mm:ss";
            break;
        case Format::Time8:
            format = "h:mm";
            break;
        default:
            kDebug(36003)<<"time format not defined :"<<_style<<endl;
            break;
    }
    return KoOasisStyles::saveOasisTimeStyle( mainStyles, format, locale, _prefix, _postfix );
}


QString Style::saveOasisStyleNumericFraction( KoGenStyles &mainStyles, Format::Type formatType,
        const QString &_prefix, const QString _suffix )
{
    //<number:number-style style:name="N71" style:family="data-style">
    //<number:fraction number:min-integer-digits="0" number:min-numerator-digits="2" number:min-denominator-digits="2"/>
    //</number:number-style>
    QString format;
    switch( formatType )
    {
        case Format::fraction_half:
            format = "# ?/2";
            break;
        case Format::fraction_quarter:
            format = "# ?/4";
            break;
        case Format::fraction_eighth:
            format = "# ?/8";
            break;
        case Format::fraction_sixteenth:
            format = "# ?/16";
            break;
        case Format::fraction_tenth:
            format = "# ?/10";
            break;
        case Format::fraction_hundredth:
            format = "# ?/100";
            break;
        case Format::fraction_one_digit:
            format = "# ?/?";
            break;
        case Format::fraction_two_digits:
            format = "# \?\?/\?\?";
            break;
        case Format::fraction_three_digits:
            format = "# \?\?\?/\?\?\?";
            break;
        default:
            kDebug(36003)<<" fraction format not defined :"<<formatType<<endl;
            break;
    }

    return KoOasisStyles::saveOasisFractionStyle( mainStyles, format, _prefix, _suffix );
}

QString Style::saveOasis( KoGenStyle& style, KoGenStyles& mainStyles ) const
{
    // KSpread::Style is definitly an OASIS auto style,
    // but don't overwrite it, if it already exists
    if (style.type() == 0)
        style = KoGenStyle( Doc::STYLE_CELL_AUTO, "table-cell" );
    // doing the real work
    saveOasisStyle( style, mainStyles );
    return mainStyles.lookup( style, "ce" );
}

void Style::saveOasisStyle( KoGenStyle &style, KoGenStyles &mainStyles ) const
{
#ifndef NDEBUG
    //if (type() == BUILTIN )
    //  kDebug(36006) << "BUILTIN" << endl;
    //else if (type() == CUSTOM )
    //  kDebug(36006) << "CUSTOM" << endl;
    //else if (type() == AUTO )
    //  kDebug(36006) << "AUTO" << endl;
#endif

    // don't store parent, if it's the default style
#ifdef __GNUC__
#warning FIXME Stefan: check for default style
#endif
//     if ( m_parent && (m_parent->type() != BUILTIN || m_parent->name() != "Default") )
//         // FIXME this is not the OASIS parent style's name. it's its display name!
//         style.addAttribute( "style:parent-style-name", m_parent->name() );

    if ( d->subStyles.contains( HorizontalAlignment ) )
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
        {
            style.addProperty( "style:text-align-source", "fix" ); // table-cell-properties
            style.addProperty( "fo:text-align", value, KoGenStyle::ParagraphType );
        }
    }

    if ( d->subStyles.contains( VerticalAlignment ) )
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

    if ( d->subStyles.contains( BackgroundColor ) && backgroundColor() != QColor() && backgroundColor().isValid() )
        style.addProperty( "fo:background-color", colorName(backgroundColor()) );

    if ( d->subStyles.contains( MultiRow ) && d->subStyles.contains( MultiRow ) )
        style.addProperty( "fo:wrap-option", "wrap" );

    if ( d->subStyles.contains( VerticalText ) && d->subStyles.contains( VerticalText ) )
    {
        style.addProperty( "style:direction", "ttb" );
        style.addProperty( "style:rotation-angle", "0" );
        style.addProperty( "style:rotation-align", "none" );
    }
#if 0
    if ( d->subStyles.contains( FloatFormat ) )
    format.setAttribute( "float", (int) floatFormat() );

                                 if ( d->subStyles.contains( FloatColor ) )
                                 format.setAttribute( "floatcolor", (int)floatColor() );

                                 if ( d->subStyles.contains( CustomFormat ) && !customFormat().isEmpty() )
                                 format.setAttribute( "custom", customFormat() );

                                 if ( d->subStyles.contains( Format::Type ) && formatType() == Money )
                         {
                                 format.setAttribute( "type", (int) currency().type );
                                 format.setAttribute( "symbol", currency().symbol );
}
#endif
                                 if ( d->subStyles.contains( Angle ) && angle() != 0 )
                         {
                             style.addProperty( "style:rotation-align", "none" );
                             style.addProperty( "style:rotation-angle", QString::number( -1.0 * angle()  ) );
                         }

                         if ( d->subStyles.contains( Indentation ) && indentation() != 0.0 )
                         {
                             style.addPropertyPt("fo:margin-left", indentation(), KoGenStyle::ParagraphType );
        //FIXME
        //if ( a == HAlignUndefined )
        //currentCellStyle.addProperty("fo:text-align", "start" );
                         }

                         if ( d->subStyles.contains( DontPrintText ) && d->subStyles.contains( DontPrintText ) )
                             style.addProperty( "style:print-content", "false");

    // protection
                         bool hideAll = false;
                         bool hideFormula = false;
                         bool isNotProtected = false;

                         if ( d->subStyles.contains( NotProtected ) && d->subStyles.contains( NotProtected ) )
                             isNotProtected = true;

                         if ( d->subStyles.contains( HideAll ) && d->subStyles.contains( HideAll ) )
                             hideAll=true;

                         if ( d->subStyles.contains( HideFormula ) && d->subStyles.contains( HideFormula ) )
                             hideFormula = true;

                         if ( hideAll )
                             style.addProperty( "style:cell-protect", "hidden-and-protected" );
                         else
                         {
                             if ( isNotProtected && !hideFormula )
                                 style.addProperty( "style:cell-protect", "none" );
                             else if ( isNotProtected && hideFormula )
                                 style.addProperty( "style:cell-protect", "Formula.hidden" );
                             else if ( hideFormula )
                                 style.addProperty( "style:cell-protect", "protected Formula.hidden" );
                             else if ( d->subStyles.contains( NotProtected ) && !d->subStyles.contains( NotProtected ) )
            // write out, only if it is explicitly set
                                 style.addProperty( "style:cell-protect", "protected" );
                         }

    // borders
    // NOTE Stefan: QPen api docs:
    //              A line width of zero indicates a cosmetic pen. This means
    //              that the pen width is always drawn one pixel wide,
    //              independent of the transformation set on the painter.
                         if ( d->subStyles.contains( LeftPen ) && d->subStyles.contains( RightPen ) &&
                              d->subStyles.contains( TopPen ) && d->subStyles.contains( BottomPen ) &&
                              ( leftBorderPen() == topBorderPen() ) &&
                              ( leftBorderPen() == rightBorderPen() ) &&
                              ( leftBorderPen() == bottomBorderPen() ) )
                         {
                             if ( leftBorderPen().style() != Qt::NoPen )
                                 style.addProperty("fo:border", Oasis::encodePen( leftBorderPen() ) );
                         }
                         else
                         {
                             if ( d->subStyles.contains( LeftPen ) && ( leftBorderPen().style() != Qt::NoPen ) )
                                 style.addProperty( "fo:border-left", Oasis::encodePen( leftBorderPen() ) );

                             if ( d->subStyles.contains( RightPen ) && ( rightBorderPen().style() != Qt::NoPen ) )
                                 style.addProperty( "fo:border-right", Oasis::encodePen( rightBorderPen() ) );

                             if ( d->subStyles.contains( TopPen ) && ( topBorderPen().style() != Qt::NoPen ) )
                                 style.addProperty( "fo:border-top", Oasis::encodePen( topBorderPen() ) );

                             if ( d->subStyles.contains( BottomPen ) && ( bottomBorderPen().style() != Qt::NoPen ) )
                                 style.addProperty( "fo:border-bottom", Oasis::encodePen( bottomBorderPen() ) );
                         }
                         if ( d->subStyles.contains( FallDiagonalPen ) && ( fallDiagonalPen().style() != Qt::NoPen ) )
                         {
                             style.addProperty("style:diagonal-tl-br", Oasis::encodePen( fallDiagonalPen() ) );
                         }
                         if ( d->subStyles.contains( GoUpDiagonalPen ) && ( goUpDiagonalPen().style() != Qt::NoPen ) )
                         {
                             style.addProperty("style:diagonal-bl-tr", Oasis::encodePen( goUpDiagonalPen() ) );
                         }

    // font
                         if ( d->subStyles.contains( FontFamily ) ) // !fontFamily().isEmpty() == true
                         {
                             style.addProperty("fo:font-family", fontFamily(), KoGenStyle::TextType );
                         }
                         if ( d->subStyles.contains( FontSize ) ) // fontSize() != 0
                         {
                             style.addPropertyPt("fo:font-size",fontSize(), KoGenStyle::TextType  );
                         }

                         if ( d->subStyles.contains( FontBold ) && bold() )
                             style.addProperty("fo:font-weight","bold", KoGenStyle::TextType );

                         if ( d->subStyles.contains( FontItalic ) && italic() )
                             style.addProperty("fo:font-style", "italic", KoGenStyle::TextType );

                         if ( d->subStyles.contains( FontUnderline ) && underline() )
                         {
        //style:text-underline-style="solid" style:text-underline-width="auto"
                             style.addProperty( "style:text-underline-style", "solid", KoGenStyle::TextType );
        //copy from oo-129
                             style.addProperty( "style:text-underline-width", "auto", KoGenStyle::TextType );
                             style.addProperty( "style:text-underline-color", "font-color", KoGenStyle::TextType );
                         }

                         if ( d->subStyles.contains( FontStrike ) && strikeOut() )
                             style.addProperty( "style:text-line-through-style", "solid", KoGenStyle::TextType );

                         if ( d->subStyles.contains( FontColor ) && fontColor().isValid() ) // always save
                         {
                             style.addProperty("fo:color", colorName( fontColor() ), KoGenStyle::TextType );
                         }

    //I don't think there is a reason why the background brush should be saved if it is null,
    //but remove the check if it causes problems.  -- Robert Knight <robertknight@gmail.com>
                         if ( d->subStyles.contains( BackgroundBrush ) && (backgroundBrush().style() != Qt::NoBrush) )
                         {
                             QString tmp = saveOasisBackgroundStyle( mainStyles, backgroundBrush() );
                             if ( !tmp.isEmpty() )
                                 style.addProperty("draw:style-name", tmp );
                         }

                         QString _prefix;
                         QString _postfix;
                         int _precision = -1;
                         if ( d->subStyles.contains( Prefix ) && !prefix().isEmpty() )
                             _prefix = prefix();
                         if ( d->subStyles.contains( Postfix ) && !postfix().isEmpty() )
                             _postfix = postfix();
                         if ( d->subStyles.contains( Precision ) && precision() != -1 )
                             _precision = precision();

                         QString currencyCode;
                         if ( d->subStyles.contains( FormatTypeKey ) && formatType() == Format::Money )
                         {
                             currencyCode = currency().code();
                         }

                         QString numericStyle = saveOasisStyleNumeric( style, mainStyles, formatType(),
                                 _prefix, _postfix, _precision,
                                 currencyCode );
                         if ( !numericStyle.isEmpty() )
                             style.addAttribute( "style:data-style-name", numericStyle );
}

QString Style::saveOasisBackgroundStyle( KoGenStyles &mainStyles, const QBrush &brush )
{
    KoGenStyle styleobjectauto = KoGenStyle( KoGenStyle::STYLE_GRAPHICAUTO, "graphic" );
    KoOasisStyles::saveOasisFillStyle( styleobjectauto, mainStyles, brush );
    return mainStyles.lookup( styleobjectauto, "gr" );
}

void Style::saveXML( QDomDocument& doc, QDomElement& format, bool force, bool copy ) const
{
    if ( d->subStyles.contains( HorizontalAlignment ) && halign() != HAlignUndefined )
        format.setAttribute( "alignX", (int) halign() );

    if ( d->subStyles.contains( VerticalAlignment ) && valign() != Middle )
        format.setAttribute( "alignY", (int) valign() );

    if ( d->subStyles.contains( BackgroundColor ) && backgroundColor() != QColor() && backgroundColor().isValid() )
        format.setAttribute( "bgcolor", backgroundColor().name() );

    if ( d->subStyles.contains( MultiRow ) && d->subStyles.contains( MultiRow ) )
        format.setAttribute( "multirow", "yes" );

    if ( d->subStyles.contains( VerticalText ) && d->subStyles.contains( VerticalText ) )
        format.setAttribute( "verticaltext", "yes" );

    if ( d->subStyles.contains( Precision ) )
        format.setAttribute( "precision", precision() );

    if ( d->subStyles.contains( Prefix ) && !prefix().isEmpty() )
        format.setAttribute( "prefix", prefix() );

    if ( d->subStyles.contains( Postfix ) && !postfix().isEmpty() )
        format.setAttribute( "postfix", postfix() );

    if ( d->subStyles.contains( FloatFormatKey ) )
        format.setAttribute( "float", (int) floatFormat() );

    if ( d->subStyles.contains( FloatColorKey ) )
        format.setAttribute( "floatcolor", (int)floatColor() );

    if ( d->subStyles.contains( FormatTypeKey ) )
        format.setAttribute( "format",(int) formatType() );

    if ( d->subStyles.contains( CustomFormat ) && !customFormat().isEmpty() )
        format.setAttribute( "custom", customFormat() );

    if ( d->subStyles.contains( FormatTypeKey ) && formatType() == Format::Money )
    {
        format.setAttribute( "type", (int) currency().index() );
        format.setAttribute( "symbol", currency().symbol() );
    }

    if ( d->subStyles.contains( Angle ) )
        format.setAttribute( "angle", angle() );

    if ( d->subStyles.contains( Indentation ) )
        format.setAttribute( "indent", indentation() );

    if ( d->subStyles.contains( DontPrintText ) && d->subStyles.contains( DontPrintText ) )
        format.setAttribute( "dontprinttext", "yes" );

    if ( d->subStyles.contains( NotProtected ) && d->subStyles.contains( NotProtected ) )
        format.setAttribute( "noprotection", "yes" );

    if ( d->subStyles.contains( HideAll ) && d->subStyles.contains( HideAll ) )
        format.setAttribute( "hideall", "yes" );

    if ( d->subStyles.contains( HideFormula ) && d->subStyles.contains( HideFormula ) )
        format.setAttribute( "hideformula", "yes" );

    if ( d->subStyles.contains( FontFamily ) )
        format.setAttribute( "font-family", fontFamily() );
    if ( d->subStyles.contains( FontSize ) )
        format.setAttribute( "font-size", fontSize() );
    if ( d->subStyles.contains( FontBold ) || d->subStyles.contains( FontItalic ) ||
         d->subStyles.contains( FontUnderline ) || d->subStyles.contains( FontStrike ) )
    {
        enum FontFlags
        {
            FBold      = 0x01,
            FUnderline = 0x02,
            FItalic    = 0x04,
            FStrike    = 0x08
        };
        int fontFlags = 0;
        fontFlags |= bold()      ? FBold      : 0;
        fontFlags |= italic()    ? FItalic    : 0;
        fontFlags |= underline() ? FUnderline : 0;
        fontFlags |= strikeOut() ? FStrike    : 0;
        format.setAttribute( "font-flags", fontFlags );
    }

  //  if ( d->subStyles.contains( Font ) )
  //    format.appendChild( NativeFormat::createElement( "font", m_textFont, doc ) );

    if ( d->subStyles.contains( FontColor ) && fontColor().isValid() )
        format.appendChild( NativeFormat::createElement( "pen", fontColor(), doc ) );

    if ( d->subStyles.contains( BackgroundBrush ) )
    {
        format.setAttribute( "brushcolor", backgroundBrush().color().name() );
        format.setAttribute( "brushstyle", (int) backgroundBrush().style() );
    }

    if ( d->subStyles.contains( LeftPen ) )
    {
        QDomElement left = doc.createElement( "left-border" );
        left.appendChild( NativeFormat::createElement( "pen", leftBorderPen(), doc ) );
        format.appendChild( left );
    }

    if ( d->subStyles.contains( TopPen ) )
    {
        QDomElement top = doc.createElement( "top-border" );
        top.appendChild( NativeFormat::createElement( "pen", topBorderPen(), doc ) );
        format.appendChild( top );
    }

    if ( d->subStyles.contains( RightPen ) )
    {
        QDomElement right = doc.createElement( "right-border" );
        right.appendChild( NativeFormat::createElement( "pen", rightBorderPen(), doc ) );
        format.appendChild( right );
    }

    if ( d->subStyles.contains( BottomPen ) )
    {
        QDomElement bottom = doc.createElement( "bottom-border" );
        bottom.appendChild( NativeFormat::createElement( "pen", bottomBorderPen(), doc ) );
        format.appendChild( bottom );
    }

    if ( d->subStyles.contains( FallDiagonalPen ) )
    {
        QDomElement fallDiagonal  = doc.createElement( "fall-diagonal" );
        fallDiagonal.appendChild( NativeFormat::createElement( "pen", fallDiagonalPen(), doc ) );
        format.appendChild( fallDiagonal );
    }

    if ( d->subStyles.contains( GoUpDiagonalPen ) )
    {
        QDomElement goUpDiagonal = doc.createElement( "up-diagonal" );
        goUpDiagonal.appendChild( NativeFormat::createElement( "pen", goUpDiagonalPen(), doc ) );
        format.appendChild( goUpDiagonal );
    }
}

bool Style::loadXML( KoXmlElement& format, Paste::Mode mode, bool paste )
{
    bool ok;
    if ( format.hasAttribute( "type" ) )
    {
        d->type = (StyleType) format.attribute( "type" ).toInt( &ok );
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
            setHAlign( a );
        }
    }
    if ( format.hasAttribute( "alignY" ) )
    {
        VAlign a = (VAlign) format.attribute( "alignY" ).toInt( &ok );
        if ( !ok )
            return false;
        if ( (unsigned int) a >= 1 || (unsigned int) a < 4 )
        {
            setVAlign( a );
        }
    }

    if ( format.hasAttribute( "bgcolor" ) )
    {
        setBackgroundColor( QColor( format.attribute( "bgcolor" ) ) );
    }

    if ( format.hasAttribute( "multirow" ) )
    {
        setWrapText( true );
    }

    if ( format.hasAttribute( "verticaltext" ) )
    {
        setVerticalText( true );
    }

    if ( format.hasAttribute( "precision" ) )
    {
        int i = format.attribute( "precision" ).toInt( &ok );
        if ( i < -1 )
        {
            kDebug(36003) << "Value out of range Cell::precision=" << i << endl;
            return false;
        }
        setPrecision( i );
    }

    if ( format.hasAttribute( "float" ) )
    {
        FloatFormat a = (FloatFormat)format.attribute( "float" ).toInt( &ok );
        if ( !ok )
            return false;
        if ( (unsigned int) a >= 1 || (unsigned int) a <= 3 )
        {
            setFloatFormat( a );
        }
    }

    if ( format.hasAttribute( "floatcolor" ) )
    {
        FloatColor a = (FloatColor) format.attribute( "floatcolor" ).toInt( &ok );
        if ( !ok ) return false;
        if ( (unsigned int) a >= 1 || (unsigned int) a <= 2 )
        {
            setFloatColor( a );
        }
    }

    if ( format.hasAttribute( "format" ) )
    {
        int fo = format.attribute( "format" ).toInt( &ok );
        if ( ! ok )
            return false;
        setFormatType( static_cast<Format::Type>(fo) );
    }
    if ( format.hasAttribute( "custom" ) )
    {
        setCustomFormat( format.attribute( "custom" ) );
    }
    if ( formatType() == Format::Money )
    {
        ok = true;
        Currency currency;
        if ( format.hasAttribute( "type" ) )
        {
            currency = Currency( format.attribute( "type" ).toInt( &ok ) );
            if ( !ok )
            {
                if ( format.hasAttribute( "symbol" ) )
                    currency = Currency( format.attribute( "symbol" ) );
            }
        }
        else if ( format.hasAttribute( "symbol" ) )
            currency = Currency( format.attribute( "symbol" ) );
        setCurrency( currency );
    }
    if ( format.hasAttribute( "angle" ) )
    {
        setAngle( format.attribute( "angle" ).toInt( &ok ) );
        if ( !ok )
            return false;
    }
    if ( format.hasAttribute( "indent" ) )
    {
        setIndentation( format.attribute( "indent" ).toDouble( &ok ) );
        if ( !ok )
            return false;
    }
    if ( format.hasAttribute( "dontprinttext" ) )
    {
        setDontPrintText( true );
    }

    if ( format.hasAttribute( "noprotection" ) )
    {
        setNotProtected( true );
    }

    if ( format.hasAttribute( "hideall" ) )
    {
        setHideAll( true );
    }

    if ( format.hasAttribute( "hideformula" ) )
    {
        setHideFormula( true );
    }

  // TODO: remove that...
    KoXmlElement font = format.namedItem( "font" ).toElement();
    if ( !font.isNull() )
    {
        QFont f( NativeFormat::toFont( font ) );
        setFontFamily( f.family() );
        setFontSize( f.pointSize() );
        if ( f.italic() )
            setFontItalic( true );
        if ( f.bold() )
            setFontBold( true );
        if ( f.underline() )
            setFontUnderline( true );
        if ( f.strikeOut() )
            setFontStrikeOut( true );

    }

    if ( format.hasAttribute( "font-family" ) )
    {
        setFontFamily( format.attribute( "font-family" ) );
    }
    if ( format.hasAttribute( "font-size" ) )
    {
        setFontSize( format.attribute( "font-size" ).toInt( &ok ) );
        if ( !ok )
            return false;
    }

    if ( format.hasAttribute( "font-flags" ) )
    {
        int fontFlags = format.attribute( "font-flags" ).toInt( &ok );
        if ( !ok )
            return false;

        enum FontFlags
        {
            FBold      = 0x01,
            FUnderline = 0x02,
            FItalic    = 0x04,
            FStrike    = 0x08
        };
        setFontBold     ( fontFlags & FBold );
        setFontItalic   ( fontFlags & FItalic );
        setFontUnderline( fontFlags & FUnderline );
        setFontStrikeOut( fontFlags & FStrike );
    }

    if ( format.hasAttribute( "brushcolor" ) )
    {
        QBrush brush = backgroundBrush();
        brush.setColor( QColor( format.attribute( "brushcolor" ) ) );
        setBackgroundBrush( brush );
    }

    if ( format.hasAttribute( "brushstyle" ) )
    {
        QBrush brush = backgroundBrush();
        brush.setStyle( (Qt::BrushStyle) format.attribute( "brushstyle" ).toInt( &ok )  );
        if ( !ok )
            return false;
        setBackgroundBrush( brush );
    }

    KoXmlElement pen = format.namedItem( "pen" ).toElement();
    if ( !pen.isNull() )
    {
        setFontColor( NativeFormat::toPen( pen ).color() );
    }

    KoXmlElement left = format.namedItem( "left-border" ).toElement();
    if ( !left.isNull() )
    {
        KoXmlElement pen = left.namedItem( "pen" ).toElement();
        if ( !pen.isNull() )
        {
            setLeftBorderPen( NativeFormat::toPen( pen ) );
        }
    }

    KoXmlElement top = format.namedItem( "top-border" ).toElement();
    if ( !top.isNull() )
    {
        KoXmlElement pen = top.namedItem( "pen" ).toElement();
        if ( !pen.isNull() )
        {
            setTopBorderPen( NativeFormat::toPen( pen ) );
        }
    }

    KoXmlElement right = format.namedItem( "right-border" ).toElement();
    if ( !right.isNull() )
    {
        KoXmlElement pen = right.namedItem( "pen" ).toElement();
        if ( !pen.isNull() )
        {
            setRightBorderPen( NativeFormat::toPen( pen ) );
        }
    }

    KoXmlElement bottom = format.namedItem( "bottom-border" ).toElement();
    if ( !bottom.isNull() )
    {
        KoXmlElement pen = bottom.namedItem( "pen" ).toElement();
        if ( !pen.isNull() )
        {
            setBottomBorderPen( NativeFormat::toPen( pen ) );
        }
    }

    KoXmlElement fallDiagonal = format.namedItem( "fall-diagonal" ).toElement();
    if ( !fallDiagonal.isNull() )
    {
        KoXmlElement pen = fallDiagonal.namedItem( "pen" ).toElement();
        if ( !pen.isNull() )
        {
            setFallDiagonalPen( NativeFormat::toPen( pen ) );
        }
    }

    KoXmlElement goUpDiagonal = format.namedItem( "up-diagonal" ).toElement();
    if ( !goUpDiagonal.isNull() )
    {
        KoXmlElement pen = goUpDiagonal.namedItem( "pen" ).toElement();
        if ( !pen.isNull() )
        {
            setGoUpDiagonalPen( NativeFormat::toPen( pen ) );
        }
    }

    if ( format.hasAttribute( "prefix" ) )
    {
        setPrefix( format.attribute( "prefix" ) );
    }
    if ( format.hasAttribute( "postfix" ) )
    {
        setPostfix( format.attribute( "postfix" ) );
    }

    return true;
}

uint Style::bottomPenValue() const
{
    if ( !d->subStyles.contains( BottomPen ) )
        return BorderPenStyle<BottomPen>().value;
    return static_cast<const BorderPenStyle<BottomPen>*>( d->subStyles[BottomPen].data() )->value;
}

uint Style::rightPenValue() const
{
    if ( !d->subStyles.contains( RightPen ) )
        return BorderPenStyle<RightPen>().value;
    return static_cast<const BorderPenStyle<RightPen>*>( d->subStyles[RightPen].data() )->value;
}

uint Style::leftPenValue() const
{
    if ( !d->subStyles.contains( LeftPen ) )
        return BorderPenStyle<LeftPen>().value;
    return static_cast<const BorderPenStyle<LeftPen>*>( d->subStyles[LeftPen].data() )->value;
}

uint Style::topPenValue() const
{
    if ( !d->subStyles.contains( TopPen ) )
        return BorderPenStyle<TopPen>().value;
    return static_cast<const BorderPenStyle<TopPen>*>( d->subStyles[TopPen].data() )->value;
}

QColor Style::fontColor() const
{
    if ( !d->subStyles.contains( FontColor ) )
        return Qt::black;//SubStyleOne<FontColor, QColor>().value1;
    return static_cast<const SubStyleOne<FontColor, QColor>*>( d->subStyles[FontColor].data() )->value1;;
}

QColor Style::backgroundColor() const
{
    if ( !d->subStyles.contains( BackgroundColor ) )
        return SubStyleOne<BackgroundColor, QColor>().value1;
    return static_cast<const SubStyleOne<BackgroundColor, QColor>*>( d->subStyles[BackgroundColor].data() )->value1;;
}

QPen Style::rightBorderPen() const
{
    if ( !d->subStyles.contains( RightPen ) )
        return BorderPenStyle<RightPen>().value1;
    return static_cast<const BorderPenStyle<RightPen>*>( d->subStyles[RightPen].data() )->value1;;
}

QPen Style::bottomBorderPen() const
{
    if ( !d->subStyles.contains( BottomPen ) )
        return BorderPenStyle<BottomPen>().value1;
    return static_cast<const BorderPenStyle<BottomPen>*>( d->subStyles[BottomPen].data() )->value1;;
}

QPen Style::leftBorderPen() const
{
    if ( !d->subStyles.contains( LeftPen ) )
        return BorderPenStyle<LeftPen>().value1;
    return static_cast<const BorderPenStyle<LeftPen>*>( d->subStyles[LeftPen].data() )->value1;;
}

QPen Style::topBorderPen() const
{
    if ( !d->subStyles.contains( TopPen ) )
        return BorderPenStyle<TopPen>().value1;
    return static_cast<const BorderPenStyle<TopPen>*>( d->subStyles[TopPen].data() )->value1;;
}

QPen Style::fallDiagonalPen() const
{
    if ( !d->subStyles.contains( FallDiagonalPen ) )
        return PenStyle<FallDiagonalPen>().value1;
    return static_cast<const PenStyle<FallDiagonalPen>*>( d->subStyles[FallDiagonalPen].data() )->value1;;
}

QPen Style::goUpDiagonalPen() const
{
    if ( !d->subStyles.contains( GoUpDiagonalPen ) )
        return PenStyle<GoUpDiagonalPen>().value1;
    return static_cast<const PenStyle<GoUpDiagonalPen>*>( d->subStyles[GoUpDiagonalPen].data() )->value1;;
}

QBrush Style::backgroundBrush() const
{
    if ( !d->subStyles.contains( BackgroundBrush ) )
        return SubStyleOne<BackgroundBrush, QBrush>().value1;
    return static_cast<const SubStyleOne<BackgroundBrush, QBrush>*>( d->subStyles[BackgroundBrush].data() )->value1;;
}

QString Style::customFormat() const
{
    if ( !d->subStyles.contains( CustomFormat ) )
        return SubStyleOne<CustomFormat,QString>().value1;
    return static_cast<const SubStyleOne<CustomFormat, QString>*>( d->subStyles[CustomFormat].data() )->value1;;
}

QString Style::prefix() const
{
    if ( !d->subStyles.contains( Prefix ) )
        return SubStyleOne<Prefix, QString>().value1;
    return static_cast<const SubStyleOne<Prefix, QString>*>( d->subStyles[Prefix].data() )->value1;;
}

QString Style::postfix() const
{
    if ( !d->subStyles.contains( Postfix ) )
        return SubStyleOne<Postfix, QString>().value1;
    return static_cast<const SubStyleOne<Postfix, QString>*>( d->subStyles[Postfix].data() )->value1;;
}

QString Style::fontFamily() const
{
    if ( !d->subStyles.contains( FontFamily ) )
        return KoGlobal::defaultFont().family(); // SubStyleOne<FontFamily, QString>().value1;
    return static_cast<const SubStyleOne<FontFamily, QString>*>( d->subStyles[FontFamily].data() )->value1;;
}

Style::HAlign Style::halign() const
{
    if ( !d->subStyles.contains( HorizontalAlignment ) )
        return SubStyleOne<HorizontalAlignment, Style::HAlign>().value1;
    return static_cast<const SubStyleOne<HorizontalAlignment, Style::HAlign>*>( d->subStyles[HorizontalAlignment].data() )->value1;;
}

Style::VAlign Style::valign() const
{
    if ( !d->subStyles.contains( VerticalAlignment ) )
        return SubStyleOne<VerticalAlignment, Style::VAlign>().value1;
    return static_cast<const SubStyleOne<VerticalAlignment, Style::VAlign>*>( d->subStyles[VerticalAlignment].data() )->value1;;
}

Style::FloatFormat Style::floatFormat() const
{
    if ( !d->subStyles.contains( FloatFormatKey ) )
        return SubStyleOne<FloatFormatKey, FloatFormat>().value1;
    return static_cast<const SubStyleOne<FloatFormatKey, FloatFormat>*>( d->subStyles[FloatFormatKey].data() )->value1;;
}

Style::FloatColor Style::floatColor() const
{
    if ( !d->subStyles.contains( FloatColorKey ) )
        return SubStyleOne<FloatColorKey, FloatColor>().value1;
    return static_cast<const SubStyleOne<FloatColorKey, FloatColor>*>( d->subStyles[FloatColorKey].data() )->value1;;
}

Format::Type Style::formatType() const
{
    if ( !d->subStyles.contains( FormatTypeKey ) )
        return SubStyleOne<FormatTypeKey, Format::Type>().value1;
    return static_cast<const SubStyleOne<FormatTypeKey, Format::Type>*>( d->subStyles[FormatTypeKey].data() )->value1;;
}

Currency Style::currency() const
{
    if ( !d->subStyles.contains( CurrencyFormat ) )
        return Currency();
    return static_cast<const SubStyleOne<CurrencyFormat, Currency>*>( d->subStyles[CurrencyFormat].data() )->value1;;
}

QFont Style::font() const
{
    QFont font;
    font.setFamily( fontFamily() );
    font.setPointSize( fontSize() );
    font.setBold( bold() );
    font.setItalic( italic() );
    font.setUnderline( underline() );
    font.setStrikeOut( strikeOut() );
    return font;
}

bool Style::bold() const
{
    if ( !d->subStyles.contains( FontBold ) )
        return SubStyleOne<FontBold, bool>().value1;
    return static_cast<const SubStyleOne<FontBold, bool>*>( d->subStyles[FontBold].data() )->value1;;
}

bool Style::italic() const
{
    if ( !d->subStyles.contains( FontItalic ) )
        return SubStyleOne<FontItalic, bool>().value1;
    return static_cast<const SubStyleOne<FontItalic, bool>*>( d->subStyles[FontItalic].data() )->value1;;
}

bool Style::underline() const
{
    if ( !d->subStyles.contains( FontUnderline ) )
        return SubStyleOne<FontUnderline, bool>().value1;
    return static_cast<const SubStyleOne<FontUnderline, bool>*>( d->subStyles[FontUnderline].data() )->value1;;
}

bool Style::strikeOut() const
{
    if ( !d->subStyles.contains( FontStrike ) )
        return SubStyleOne<FontStrike, bool>().value1;
    return static_cast<const SubStyleOne<FontStrike, bool>*>( d->subStyles[FontStrike].data() )->value1;;
}

int Style::fontSize() const
{
    if ( !d->subStyles.contains( FontSize ) )
        return KoGlobal::defaultFont().pointSize(); //SubStyleOne<FontSize, int>().value1;
    return static_cast<const SubStyleOne<FontSize, int>*>( d->subStyles[FontSize].data() )->value1;;
}

int Style::precision() const
{
    if ( !d->subStyles.contains( Precision ) )
        return -1; //SubStyleOne<Precision, int>().value1;
    return static_cast<const SubStyleOne<Precision, int>*>( d->subStyles[Precision].data() )->value1;;
}

int Style::angle() const
{
    if ( !d->subStyles.contains( Angle ) )
        return SubStyleOne<Angle, int>().value1;
    return static_cast<const SubStyleOne<Angle, int>*>( d->subStyles[Angle].data() )->value1;;
}

double Style::indentation() const
{
    if ( !d->subStyles.contains( Indentation ) )
        return SubStyleOne<Indentation, int>().value1;
    return static_cast<const SubStyleOne<Indentation, int>*>( d->subStyles[Indentation].data() )->value1;;
}

bool Style::verticalText() const
{
    if ( !d->subStyles.contains( VerticalText ) )
        return SubStyleOne<VerticalText, bool>().value1;
    return static_cast<const SubStyleOne<VerticalText, bool>*>( d->subStyles[VerticalText].data() )->value1;;
}

bool Style::wrapText() const
{
    if ( !d->subStyles.contains( MultiRow ) )
        return SubStyleOne<MultiRow, bool>().value1;
    return static_cast<const SubStyleOne<MultiRow, bool>*>( d->subStyles[MultiRow].data() )->value1;;
}

bool Style::printText() const
{
    if ( !d->subStyles.contains( DontPrintText ) )
        return !SubStyleOne<DontPrintText, bool>().value1;
    return !static_cast<const SubStyleOne<DontPrintText, bool>*>( d->subStyles[DontPrintText].data() )->value1;;
}

bool Style::hideAll() const
{
    if ( !d->subStyles.contains( HideAll ) )
        return SubStyleOne<HideAll, bool>().value1;
    return static_cast<const SubStyleOne<HideAll, bool>*>( d->subStyles[HideAll].data() )->value1;;
}

bool Style::hideFormula() const
{
    if ( !d->subStyles.contains( HideFormula ) )
        return SubStyleOne<HideFormula, bool>().value1;
    return static_cast<const SubStyleOne<HideFormula, bool>*>( d->subStyles[HideFormula].data() )->value1;;
}

bool Style::notProtected() const
{
    if ( !d->subStyles.contains( NotProtected ) )
        return SubStyleOne<NotProtected, bool>().value1;
    return static_cast<const SubStyleOne<NotProtected, bool>*>( d->subStyles[NotProtected].data() )->value1;;
}

bool Style::isDefault() const
{
    return isEmpty() || d->subStyles.contains( DefaultStyleKey );
}

bool Style::isEmpty() const
{
    return d->subStyles.isEmpty();
}

void Style::setHAlign( HAlign align )
{
    insertSubStyle( HorizontalAlignment, align );
}

void Style::setVAlign( VAlign align )
{
    insertSubStyle( VerticalAlignment, align );
}

void Style::setFont( QFont const & font )
{
    insertSubStyle( FontFamily,     font.family() );
    insertSubStyle( FontSize,       font.pointSize() );
    insertSubStyle( FontBold,       font.bold() );
    insertSubStyle( FontItalic,     font.italic() );
    insertSubStyle( FontStrike,     font.strikeOut() );
    insertSubStyle( FontUnderline,  font.underline() );
}

void Style::setFontFamily( QString const & family )
{
    insertSubStyle( FontFamily, family );
}

void Style::setFontBold( bool enabled )
{
    insertSubStyle( FontBold, enabled );
}

void Style::setFontItalic( bool enabled )
{
    insertSubStyle( FontItalic, enabled );
}

void Style::setFontUnderline( bool enabled )
{
    insertSubStyle( FontUnderline, enabled );
}

void Style::setFontStrikeOut( bool enabled )
{
    insertSubStyle( FontStrike, enabled );
}

void Style::setFontSize( int size )
{
    insertSubStyle( FontSize, size );
}

void Style::setFontColor( QColor const & color )
{
    insertSubStyle( FontColor, color );
}

void Style::setBackgroundColor( QColor const & color )
{
    insertSubStyle( BackgroundColor, color );
}

void Style::setRightBorderPen( QPen const & pen )
{
    insertSubStyle( RightPen, pen );
}

void Style::setBottomBorderPen( QPen const & pen )
{
    insertSubStyle( BottomPen, pen );
}

void Style::setLeftBorderPen( QPen const & pen )
{
    insertSubStyle( LeftPen, pen );
}

void Style::setTopBorderPen( QPen const & pen )
{
    insertSubStyle( TopPen, pen );
}

void Style::setFallDiagonalPen( QPen const & pen )
{
    insertSubStyle( FallDiagonalPen, pen );
}

void Style::setGoUpDiagonalPen( QPen const & pen )
{
    insertSubStyle( GoUpDiagonalPen, pen );
}

void Style::setAngle( int angle )
{
    insertSubStyle( Angle, angle );
}

void Style::setIndentation( double indent )
{
    insertSubStyle( Indentation, indent );
}

void Style::setBackgroundBrush( QBrush const & brush )
{
    insertSubStyle( BackgroundBrush, brush );
}

void Style::setFloatFormat( FloatFormat format )
{
    insertSubStyle( FloatFormatKey, format );
}

void Style::setFloatColor( FloatColor color )
{
    insertSubStyle( FloatColorKey, color );
}

void Style::setFormatType( Format::Type format )
{
    insertSubStyle( FormatTypeKey, format );
}

void Style::setCustomFormat( QString const & strFormat )
{
    insertSubStyle( CustomFormat, strFormat );
}

void Style::setPrecision( int precision )
{
    insertSubStyle( Precision, precision );
}

void Style::setPrefix( QString const & prefix )
{
    insertSubStyle( Prefix, prefix );
}

void Style::setPostfix( QString const & postfix )
{
    insertSubStyle( Postfix, postfix );
}

void Style::setCurrency( Currency const & currency )
{
    QVariant variant;
    variant.setValue( currency );
    insertSubStyle( CurrencyFormat, variant );
}

void Style::setWrapText( bool enable )
{
    insertSubStyle( MultiRow, enable );
}

void Style::setHideAll( bool enable )
{
    insertSubStyle( HideAll, enable );
}

void Style::setHideFormula( bool enable )
{
    insertSubStyle( HideFormula, enable );
}

void Style::setNotProtected( bool enable )
{
    insertSubStyle( NotProtected, enable );
}

void Style::setDontPrintText( bool enable )
{
    insertSubStyle( DontPrintText, enable );
}

void Style::setVerticalText( bool enable )
{
    insertSubStyle( VerticalText, enable );
}

void Style::setDefault()
{
    insertSubStyle( DefaultStyleKey, true );
}

void Style::clear()
{
    d->subStyles.clear();
}

QString Style::colorName( const QColor& color )
{
    static QMap<QRgb, QString> map;
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

bool Style::compare( const SubStyle* one, const SubStyle* two )
{
    if ( !one || !two )
        return one == two;
    if ( one->type() != two->type() )
        return false;
    switch ( one->type() )
    {
        case DefaultStyleKey:
            return true;
        case NamedStyleKey:
            return static_cast<const NamedStyle*>(one)->name != static_cast<const NamedStyle*>(two)->name;
        // borders
        case LeftPen:
            return static_cast<const SubStyleOne<LeftPen,QPen>*>(one)->value1 == static_cast<const SubStyleOne<LeftPen,QPen>*>(two)->value1;
        case RightPen:
            return static_cast<const SubStyleOne<RightPen,QPen>*>(one)->value1 == static_cast<const SubStyleOne<RightPen,QPen>*>(two)->value1;
        case TopPen:
            return static_cast<const SubStyleOne<TopPen,QPen>*>(one)->value1 == static_cast<const SubStyleOne<TopPen,QPen>*>(two)->value1;
        case BottomPen:
            return static_cast<const SubStyleOne<BottomPen,QPen>*>(one)->value1 == static_cast<const SubStyleOne<BottomPen,QPen>*>(two)->value1;
        case FallDiagonalPen:
            return static_cast<const SubStyleOne<FallDiagonalPen,QPen>*>(one)->value1 == static_cast<const SubStyleOne<FallDiagonalPen,QPen>*>(two)->value1;
        case GoUpDiagonalPen:
            return static_cast<const SubStyleOne<GoUpDiagonalPen,QPen>*>(one)->value1 == static_cast<const SubStyleOne<GoUpDiagonalPen,QPen>*>(two)->value1;
        // layout
        case HorizontalAlignment:
            return static_cast<const SubStyleOne<HorizontalAlignment,HAlign>*>(one)->value1 == static_cast<const SubStyleOne<HorizontalAlignment,HAlign>*>(two)->value1;
        case VerticalAlignment:
            return static_cast<const SubStyleOne<VerticalAlignment,VAlign>*>(one)->value1 == static_cast<const SubStyleOne<VerticalAlignment,VAlign>*>(two)->value1;
        case MultiRow:
            return static_cast<const SubStyleOne<MultiRow,bool>*>(one)->value1 == static_cast<const SubStyleOne<MultiRow,bool>*>(two)->value1;
        case VerticalText:
            return static_cast<const SubStyleOne<VerticalText,bool>*>(one)->value1 == static_cast<const SubStyleOne<VerticalText,bool>*>(two)->value1;
        case Angle:
            return static_cast<const SubStyleOne<Angle,int>*>(one)->value1 == static_cast<const SubStyleOne<Angle,int>*>(two)->value1;
        case Indentation:
            return static_cast<const SubStyleOne<Indentation,int>*>(one)->value1 == static_cast<const SubStyleOne<Indentation,int>*>(two)->value1;
        // content format
        case Prefix:
            return static_cast<const SubStyleOne<Prefix,QString>*>(one)->value1 == static_cast<const SubStyleOne<Prefix,QString>*>(two)->value1;
        case Postfix:
            return static_cast<const SubStyleOne<Postfix,QString>*>(one)->value1 == static_cast<const SubStyleOne<Postfix,QString>*>(two)->value1;
        case Precision:
            return static_cast<const SubStyleOne<Precision,int>*>(one)->value1 == static_cast<const SubStyleOne<Precision,int>*>(two)->value1;
        case FormatTypeKey:
            return static_cast<const SubStyleOne<FormatTypeKey,Format::Type>*>(one)->value1 == static_cast<const SubStyleOne<FormatTypeKey,Format::Type>*>(two)->value1;
        case FloatFormatKey:
            return static_cast<const SubStyleOne<FloatFormatKey,FloatFormat>*>(one)->value1 == static_cast<const SubStyleOne<FloatFormatKey,FloatFormat>*>(two)->value1;
        case FloatColorKey:
            return static_cast<const SubStyleOne<FloatColorKey,FloatColor>*>(one)->value1 == static_cast<const SubStyleOne<FloatColorKey,FloatColor>*>(two)->value1;
        case CurrencyFormat:
        {
            Currency currencyOne = static_cast<const SubStyleOne<CurrencyFormat,Currency>*>(one)->value1;
            Currency currencyTwo = static_cast<const SubStyleOne<CurrencyFormat,Currency>*>(two)->value1;
            if ( currencyOne != currencyTwo )
                return false;
            return true;
        }
        case CustomFormat:
            return static_cast<const SubStyleOne<CustomFormat,QString>*>(one)->value1 == static_cast<const SubStyleOne<CustomFormat,QString>*>(two)->value1;
        // background
        case BackgroundBrush:
            return static_cast<const SubStyleOne<BackgroundBrush,QBrush>*>(one)->value1 == static_cast<const SubStyleOne<BackgroundBrush,QBrush>*>(two)->value1;
        case BackgroundColor:
            return static_cast<const SubStyleOne<BackgroundColor,QColor>*>(one)->value1 == static_cast<const SubStyleOne<BackgroundColor,QColor>*>(two)->value1;
        // font
        case FontColor:
           return static_cast<const SubStyleOne<FontColor,QColor>*>(one)->value1 == static_cast<const SubStyleOne<FontColor,QColor>*>(two)->value1;
        case FontFamily:
           return static_cast<const SubStyleOne<FontFamily,QString>*>(one)->value1 == static_cast<const SubStyleOne<FontFamily,QString>*>(two)->value1;
        case FontSize:
           return static_cast<const SubStyleOne<FontSize,int>*>(one)->value1 == static_cast<const SubStyleOne<FontSize,int>*>(two)->value1;
        case FontBold:
            return static_cast<const SubStyleOne<FontBold,bool>*>(one)->value1 == static_cast<const SubStyleOne<FontBold,bool>*>(two)->value1;
        case FontItalic:
            return static_cast<const SubStyleOne<FontItalic,bool>*>(one)->value1 == static_cast<const SubStyleOne<FontItalic,bool>*>(two)->value1;
        case FontStrike:
            return static_cast<const SubStyleOne<FontStrike,bool>*>(one)->value1 == static_cast<const SubStyleOne<FontStrike,bool>*>(two)->value1;
        case FontUnderline:
            return static_cast<const SubStyleOne<FontUnderline,bool>*>(one)->value1 == static_cast<const SubStyleOne<FontUnderline,bool>*>(two)->value1;
        //misc
        case DontPrintText:
            return static_cast<const SubStyleOne<DontPrintText,bool>*>(one)->value1 == static_cast<const SubStyleOne<DontPrintText,bool>*>(two)->value1;
        case NotProtected:
            return static_cast<const SubStyleOne<NotProtected,bool>*>(one)->value1 == static_cast<const SubStyleOne<NotProtected,bool>*>(two)->value1;
        case HideAll:
            return static_cast<const SubStyleOne<HideAll,bool>*>(one)->value1 == static_cast<const SubStyleOne<HideAll,bool>*>(two)->value1;
        case HideFormula:
            return static_cast<const SubStyleOne<HideFormula,bool>*>(one)->value1 == static_cast<const SubStyleOne<HideFormula,bool>*>(two)->value1;
        default:
            return false;
    }
}

bool Style::operator==( const Style& other ) const
{
    if ( other.isEmpty() )
        return isEmpty() ? true : false;
    QList<Key> attributes = d->subStyles.keys() + other.d->subStyles.keys();
    for ( int i = 0; i < attributes.count(); ++i )
    {
        if ( !compare( d->subStyles.value(attributes[i]).data(), other.d->subStyles.value(attributes[i]).data() ) )
            return false;
    }
    return true;
}

void Style::operator=( const Style& other )
{
    d = other.d;
}

void Style::merge( const Style& style )
{
    const QList<SharedSubStyle> subStyles( style.subStyles() );
//     kDebug(36006) << "merging " << subStyles.count() << " attributes." << endl;
    for ( int i = 0; i < subStyles.count(); ++i )
    {
//         kDebug(36006) << subStyles[i]->debugData() << endl;
        insertSubStyle( subStyles[i] );
    }
}

void Style::dump() const
{
    for ( int i = 0; i < subStyles().count(); ++i )
        subStyles()[i]->dump();
}

void Style::setType( StyleType type )
{
    d->type = type;
}

QList<SharedSubStyle> Style::subStyles() const
{
    return d->subStyles.values();
}

SharedSubStyle Style::createSubStyle( Key key, const QVariant& value )
{
    SharedSubStyle newSubStyle;
    switch ( key )
    {
        // special cases
        case DefaultStyleKey:
            newSubStyle = new SubStyle();
            break;
        case NamedStyleKey:
            newSubStyle = new NamedStyle( value.value<QString>() );
            break;
        case LeftPen:
            newSubStyle = new BorderPenStyle<LeftPen>( value.value<QPen>() );
            break;
        case RightPen:
            newSubStyle = new BorderPenStyle<RightPen>( value.value<QPen>() );
            break;
        case TopPen:
            newSubStyle = new BorderPenStyle<TopPen>( value.value<QPen>() );
            break;
        case BottomPen:
            newSubStyle = new BorderPenStyle<BottomPen>( value.value<QPen>() );
            break;
        case FallDiagonalPen:
            newSubStyle = new BorderPenStyle<FallDiagonalPen>( value.value<QPen>() );
            break;
        case GoUpDiagonalPen:
            newSubStyle = new BorderPenStyle<GoUpDiagonalPen>( value.value<QPen>() );
            break;
        // layout
        case HorizontalAlignment:
            newSubStyle = new SubStyleOne<HorizontalAlignment, HAlign>( (HAlign)value.value<int>() );
            break;
        case VerticalAlignment:
            newSubStyle = new SubStyleOne<VerticalAlignment, VAlign>( (VAlign)value.value<int>() );
            break;
        case MultiRow:
            newSubStyle = new SubStyleOne<MultiRow, bool>( value.value<bool>() );
            break;
        case VerticalText:
            newSubStyle = new SubStyleOne<VerticalText, bool>( value.value<bool>() );
            break;
        case Angle:
            newSubStyle = new SubStyleOne<Angle, int>( value.value<int>() );
            break;
        case Indentation:
            newSubStyle = new SubStyleOne<Indentation, int>( value.value<int>() );
            break;
        // content format
        case Prefix:
            newSubStyle = new SubStyleOne<Prefix, QString>( value.value<QString>() );
            break;
        case Postfix:
            newSubStyle = new SubStyleOne<Postfix, QString>( value.value<QString>() );
            break;
        case Precision:
            newSubStyle = new SubStyleOne<Precision, int>( value.value<int>() );
            break;
        case FormatTypeKey:
            newSubStyle = new SubStyleOne<FormatTypeKey, Format::Type>( (Format::Type)value.value<int>() );
            break;
        case FloatFormatKey:
            newSubStyle = new SubStyleOne<FloatFormatKey, FloatFormat>( (FloatFormat)value.value<int>() );
            break;
        case FloatColorKey:
            newSubStyle = new SubStyleOne<FloatColorKey, FloatColor>( (FloatColor)value.value<int>() );
            break;
        case CurrencyFormat:
            newSubStyle = new SubStyleOne<CurrencyFormat, Currency>( value.value<Currency>() );
            break;
        case CustomFormat:
            newSubStyle = new SubStyleOne<CustomFormat, QString>( value.value<QString>() );
            break;
        // background
        case BackgroundBrush:
            newSubStyle = new SubStyleOne<BackgroundBrush, QBrush>( value.value<QBrush>() );
            break;
        case BackgroundColor:
            newSubStyle = new SubStyleOne<BackgroundColor, QColor>( value.value<QColor>() );
            break;
        // font
        case FontColor:
            newSubStyle = new SubStyleOne<FontColor, QColor>( value.value<QColor>() );
            break;
        case FontFamily:
            newSubStyle = new SubStyleOne<FontFamily, QString>( value.value<QString>() );
            break;
        case FontSize:
            newSubStyle = new SubStyleOne<FontSize, int>( value.value<int>() );
            break;
        case FontBold:
            newSubStyle = new SubStyleOne<FontBold, bool>( value.value<bool>() );
            break;
        case FontItalic:
            newSubStyle = new SubStyleOne<FontItalic, bool>( value.value<bool>() );
            break;
        case FontStrike:
            newSubStyle = new SubStyleOne<FontStrike, bool>( value.value<bool>() );
            break;
        case FontUnderline:
            newSubStyle = new SubStyleOne<FontUnderline, bool>( value.value<bool>() );
            break;
        //misc
        case DontPrintText:
            newSubStyle = new SubStyleOne<DontPrintText, bool>( value.value<bool>() );
            break;
        case NotProtected:
            newSubStyle = new SubStyleOne<NotProtected, bool>( value.value<bool>() );
            break;
        case HideAll:
            newSubStyle = new SubStyleOne<HideAll, bool>( value.value<bool>() );
            break;
        case HideFormula:
            newSubStyle = new SubStyleOne<HideFormula, bool>( value.value<bool>() );
            break;
    }
    return newSubStyle;
}

void Style::insertSubStyle( Key key, const QVariant& value )
{
    const SharedSubStyle subStyle = createSubStyle( key, value );
    Q_ASSERT( !!subStyle );
    insertSubStyle( subStyle );
}

void Style::insertSubStyle( const SharedSubStyle& subStyle )
{
    if ( !subStyle )
        return;
    releaseSubStyle( subStyle->type() );
    d->subStyles.insert( subStyle->type(), subStyle );
}

bool Style::releaseSubStyle( Key key )
{
    if ( !d->subStyles.contains( key ) )
        return false;

    d->subStyles.remove( key );
    return true;
}

/////////////////////////////////////////////////////////////////////////////
//
// CustomStyle::Private
//
/////////////////////////////////////////////////////////////////////////////

class CustomStyle::Private : public QSharedData
{
public:
    QString name;
};


/////////////////////////////////////////////////////////////////////////////
//
// CustomStyle
//
/////////////////////////////////////////////////////////////////////////////

CustomStyle::CustomStyle()
    : Style()
    , d( new Private )
{
}

CustomStyle::CustomStyle( QString const & name, CustomStyle * parent )
    : Style()
    , d( new Private )
{
    d->name = name;
    if ( parent )
        setParentName( parent->name() );
}

CustomStyle::~CustomStyle()
{
}

Style::StyleType CustomStyle::type() const
{
    return AUTO;
}

void CustomStyle::setType( StyleType type )
{
    Style::setType( type );
}

const QString& CustomStyle::name() const
{
    return d->name;
}

void CustomStyle::setName( QString const & name )
{
    d->name = name;
}

bool CustomStyle::definesAll() const
{
    // TODO
    return false;
}

QString CustomStyle::saveOasis( KoGenStyle& style, KoGenStyles &mainStyles ) const
{
    // If the type is undefined, we're called from Format
    // and the OASIS style is not an automatic style.
    // TODO: As the user styles are already created, look them up
    //       in what way ever and return here.
//     if ( style.type() == 0 && ( m_type == BUILTIN ) && ( m_name == "Default" ) )
//       return "Default";
    if ( style.type() == 0 )
        style = KoGenStyle( Doc::STYLE_CELL_USER, "table-cell" );

    if ( name().isEmpty() )
        return QString(); // TODO fallback to Style::saveOasis() ???

    // default style does not need display name
    if( type() != BUILTIN || name() != "Default" )
        style.addAttribute( "style:display-name", name() );

    // doing the real work
    saveOasisStyle( style, mainStyles );

    // The lookup is done in the calling object (Format).
    if ( style.type() == Doc::STYLE_CELL_AUTO )
        return QString();

    if( ( type() == BUILTIN ) && ( name() == "Default" ) )
    {
        style.setDefaultStyle(true);
        // don't i18n'ize "Default" in this case
        return mainStyles.lookup( style, "Default", KoGenStyles::DontForceNumbering );
    }
    else
        // this is a custom style
        return mainStyles.lookup( style, "custom-style" );
}

void CustomStyle::loadOasis( KoOasisStyles& oasisStyles, const KoXmlElement& style, const QString & name )
{
    setName (name);
    if ( style.hasAttributeNS( KoXmlNS::style, "parent-style-name" ) )
        setParentName (style.attributeNS( KoXmlNS::style, "parent-style-name", QString::null ));

    setType (CUSTOM);

    Style::loadOasisStyle( oasisStyles, style );
}

void CustomStyle::save( QDomDocument & doc, QDomElement & styles )
{
    if ( name().isEmpty() )
        return;

    QDomElement style( doc.createElement( "style" ) );
    style.setAttribute( "type", (int) type() );
    if ( !parentName().isNull() )
        style.setAttribute( "parent", parentName() );
    style.setAttribute( "name", name() );

    QDomElement format( doc.createElement( "format" ) );
    saveXML( doc, format );
    style.appendChild( format );

    styles.appendChild( style );
}

bool CustomStyle::loadXML( KoXmlElement const & style, QString const & name )
{
    setName (name);

    if ( style.hasAttribute( "parent" ) )
        setParentName (style.attribute( "parent" ));

    if ( !style.hasAttribute( "type" ) )
        return false;

    bool ok = true;
    setType ((StyleType) style.attribute( "type" ).toInt( &ok ));
    if ( !ok )
        return false;

    KoXmlElement f( style.namedItem( "format" ).toElement() );
    if ( !f.isNull() )
        if ( !Style::loadXML( f ) )
            return false;

    return true;
}

#if 0
void CustomStyle::insertSubStyle( Key key, const QVariant& value )
{
    SharedSubStyle subStyle = createSubStyle( key, value );
    Q_ASSERT( subStyle );
    Style::insertSubStyle( subStyle );
}
#endif

int CustomStyle::usage() const
{
    return d->ref;
}

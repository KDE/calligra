#include "KSpreadCellIface.h"

#include "kspread_table.h"
#include "kspread_cell.h"

KSpreadCellIface::KSpreadCellIface()
{
}

void KSpreadCellIface::setCell( KSpreadTable* table, const QPoint& point )
{
    m_table = table;
    m_point = point;
}

QString KSpreadCellIface::text() const
{
    KSpreadCell* cell = m_table->cellAt( m_point );
    return cell->valueString();
}

void KSpreadCellIface::setText( const QString& text )
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    cell->setCellText( text );
}

void KSpreadCellIface::setComment( const QString& comment )
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    cell->setComment( comment);
    cell->update();
}

void KSpreadCellIface::setValue( int value )
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    cell->setValue( (double)value );
}

void KSpreadCellIface::setValue( double value )
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    cell->setValue( value );
}

double KSpreadCellIface::value() const
{
    KSpreadCell* cell = m_table->cellAt( m_point );
    return cell->valueDouble();
}

void KSpreadCellIface::setBgColor(const QString& _c)
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    QColor c(_c);
    cell->setBgColor(c);
    cell->update();
}

void KSpreadCellIface::setBgColor(int r,int g,int b)
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    QColor c(r,g,b);
    cell->setBgColor(c);
    cell->update();
}

QString KSpreadCellIface::bgColor() const
{
    KSpreadCell* cell = m_table->cellAt( m_point );
    return cell->bgColor( m_point.x(), m_point.y() ).name();
}

QString KSpreadCellIface::textColor() const
{
    KSpreadCell* cell = m_table->cellAt( m_point );
    return cell->textColor( m_point.x(), m_point.y() ).name();
}

void KSpreadCellIface::setTextColor(int r,int g,int b)
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    QColor c(r,g,b);
    cell->setTextColor(c);
    cell->update();
}

void KSpreadCellIface::setTextColor(const QString& _c)
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    QColor c(_c);
    cell->setTextColor(c);
    cell->update();
}

void KSpreadCellIface::setAngle(int angle)
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    cell->setAngle(angle);
    cell->update();
}

int  KSpreadCellIface::angle() const
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    return cell->getAngle(m_point.x(), m_point.y());
}

void KSpreadCellIface::setVerticalText(bool _vertical)
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    cell->setVerticalText(_vertical);
    cell->update();
}

bool KSpreadCellIface::verticalText() const
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    return cell->verticalText( m_point.x(), m_point.y() );
}


void KSpreadCellIface::setMultiRow(bool _multi)
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    cell->setMultiRow( _multi );
    cell->update();
}

bool KSpreadCellIface::multiRow() const
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    return cell->multiRow( m_point.x(), m_point.y() );
}

void KSpreadCellIface::setAlign( const QString& _Align )
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    KSpreadCell::Align Align;
    if(_Align=="Left")
        Align=KSpreadCell::Left;
    else if(_Align=="Right")
        Align=KSpreadCell::Right;
    else if(_Align=="Center")
        Align=KSpreadCell::Center;
    else
        Align=KSpreadCell::Undefined;
    cell->setAlign( Align);
    cell->update();
}

QString KSpreadCellIface::align() const
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    QString alignString;
    switch( cell->align( m_point.x(), m_point.y() ) )
        {
        case KSpreadCell::Left :
                alignString="Left";
                break;
        case KSpreadCell::Right :
                alignString="Right";
                break;
        case KSpreadCell::Center :
                alignString="Center";
                break;
        case KSpreadCell::Undefined :
                alignString="Undefined";
                break;
        }
    return alignString;
}

void KSpreadCellIface::setAlignY( const QString& _AlignY )
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    KSpreadCell::AlignY AlignY;
    if(_AlignY=="Top")
        AlignY=KSpreadCell::Top;
    else if(_AlignY=="Middle")
        AlignY=KSpreadCell::Middle;
    else if(_AlignY=="Bottom")
        AlignY=KSpreadCell::Bottom;
    else
        AlignY=KSpreadCell::Middle;
    cell->setAlignY( AlignY);
    cell->update();
}

QString KSpreadCellIface::alignY() const
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    QString alignString;
    switch( cell->alignY( m_point.x(), m_point.y() ) )
        {
        case KSpreadCell::Top :
                alignString="Top";
                break;
        case KSpreadCell::Middle :
                alignString="Middle";
                break;
        case KSpreadCell::Bottom :
                alignString="Bottom";
                break;
        }
    return alignString;
}

void KSpreadCellIface::setPostfix(const QString &_postfix)
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    cell->setPostfix( _postfix );
    cell->update();
}

QString KSpreadCellIface::prefix() const
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    return cell->prefix( m_point.x(), m_point.y() );
}

void KSpreadCellIface::setPrefix(const QString &_prefix)
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    cell->setPrefix( _prefix );
    cell->update();
}

QString KSpreadCellIface::postfix() const
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    return cell->postfix( m_point.x(), m_point.y() );
}

void KSpreadCellIface::setFormatNumber(const QString &_formatNumber)
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    KSpreadCell::formatNumber format;
    cell->setFaktor( 1.0);
    cell->setPrecision(2);
    if(_formatNumber=="Number")
        format=KSpreadCell::Number;
    else if(_formatNumber=="Money")
        format=KSpreadCell::Money;
    else if(_formatNumber=="Percentage")
        {
        format=KSpreadCell::Percentage;
        cell->setFaktor( 100.0);
        }
    else if(_formatNumber=="Scientific")
        format=KSpreadCell::Scientific;
    else if(_formatNumber=="ShortDate")
        format=KSpreadCell::ShortDate;
    else if(_formatNumber=="TextDate")
        format=KSpreadCell::TextDate;
    else if(_formatNumber=="Time")
        format=KSpreadCell::Time;
    else if(_formatNumber=="SecondeTime")
        format=KSpreadCell::SecondeTime;
    else if(_formatNumber=="fraction_half")
        format=KSpreadCell::fraction_half;
    else if(_formatNumber=="fraction_quarter")
        format=KSpreadCell::fraction_quarter;
    else if(_formatNumber=="fraction_eighth")
        format=KSpreadCell::fraction_eighth;
    else if(_formatNumber=="fraction_sixteenth")
        format=KSpreadCell::fraction_sixteenth;
    else if(_formatNumber=="fraction_tenth")
        format=KSpreadCell::fraction_tenth;
    else if(_formatNumber=="fraction_hundredth")
        format=KSpreadCell::fraction_hundredth;
    else if(_formatNumber=="fraction_one_digit")
        format=KSpreadCell::fraction_one_digit;
    else if(_formatNumber=="fraction_two_digits")
        format=KSpreadCell::fraction_two_digits;
    else if(_formatNumber=="fraction_three_digits")
        format=KSpreadCell::fraction_three_digits;
    else
        format=KSpreadCell::Number;
    cell->setFormatNumber( format);
    cell->update();
}

QString KSpreadCellIface::getFormatNumber() const
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    QString stringFormat;
    switch( cell->getFormatNumber(m_point.x(), m_point.y()))
        {
        case KSpreadCell::Number:
                stringFormat="Number";
                break;
        case KSpreadCell::Money:
                stringFormat="Money";
                break;
        case KSpreadCell::Percentage:
                stringFormat="Percentage";
                break;
        case KSpreadCell::Scientific:
                stringFormat="Scientific";
                break;
        case KSpreadCell::ShortDate:
                stringFormat="ShortDate";
                break;
        case KSpreadCell::TextDate:
                stringFormat="TextDate";
                break;
        case KSpreadCell::date_format1:
        case KSpreadCell::date_format2:
        case KSpreadCell::date_format3:
        case KSpreadCell::date_format4:
        case KSpreadCell::date_format5:
        case KSpreadCell::date_format6:
        case KSpreadCell::date_format7:
        case KSpreadCell::date_format8:
        case KSpreadCell::date_format9:
        case KSpreadCell::date_format10:
        case KSpreadCell::date_format11:
        case KSpreadCell::date_format12:
        case KSpreadCell::date_format13:
        case KSpreadCell::date_format14:
        case KSpreadCell::date_format15:
        case KSpreadCell::date_format16:
                stringFormat="date format";
                break;
        case KSpreadCell::Time:
                stringFormat="Time";
                break;
        case KSpreadCell::SecondeTime:
                stringFormat="SecondeTime";
                break;
        case KSpreadCell::fraction_half:
                stringFormat="fraction_half";
                break;
        case KSpreadCell::fraction_quarter:
                stringFormat="fraction_quarter";
                break;
        case KSpreadCell::fraction_eighth:
                stringFormat="fraction_eighth";
                break;
        case KSpreadCell::fraction_sixteenth:
                stringFormat="fraction_sixteenth";
                break;
        case KSpreadCell::fraction_tenth:
                stringFormat="fraction_tenth";
                break;
        case KSpreadCell::fraction_hundredth:
                stringFormat="fraction_hundredth";
                break;
        case KSpreadCell::fraction_one_digit:
                stringFormat="fraction_one_digit";
                break;
        case KSpreadCell::fraction_two_digits:
                stringFormat="fraction_two_digits";
                break;
        case KSpreadCell::fraction_three_digits:
                stringFormat="fraction_three_digits";
                break;
        }
    return stringFormat;
}

void KSpreadCellIface::setPrecision(int _p)
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    cell->setPrecision( _p );
    cell->update();
}

int KSpreadCellIface::precision() const
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    return cell->precision( m_point.x(), m_point.y() );
}

void KSpreadCellIface::setTextFontBold(bool _b)
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    cell->setTextFontBold( _b );
    cell->update();
}

bool KSpreadCellIface::textFontBold() const
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    return cell->textFontBold( m_point.x(), m_point.y() );
}

void KSpreadCellIface::setTextFontItalic(bool _b)
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    cell->setTextFontItalic( _b );
    cell->update();
}

bool KSpreadCellIface::textFontItalic() const
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    return cell->textFontItalic( m_point.x(), m_point.y() );
}


void KSpreadCellIface::setTextFontUnderline(bool _b)
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    cell->setTextFontUnderline( _b );
    cell->update();
}

bool KSpreadCellIface::textFontUnderline() const
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    return cell->textFontUnderline( m_point.x(), m_point.y() );
}

void KSpreadCellIface::setTextFontStrike(bool _b)
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    cell->setTextFontStrike( _b );
    cell->update();
}

bool KSpreadCellIface::textFontStrike() const
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    return cell->textFontStrike( m_point.x(), m_point.y() );
}

void KSpreadCellIface::setTextFontSize( int _size )
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    cell->setTextFontSize( _size );
    cell->update();
}

int KSpreadCellIface::textFontSize() const
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    return cell->textFontSize( m_point.x(), m_point.y() );
}

void KSpreadCellIface::setTextFontFamily( const QString& _font )
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    cell->setTextFontFamily( _font );
    cell->update();
}

QString KSpreadCellIface::textFontFamily() const
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    return cell->textFontFamily( m_point.x(), m_point.y() );
}

void KSpreadCellIface::setFaktor( double _factor )
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    cell->setFaktor( _factor );
    cell->update();
}

double KSpreadCellIface::faktor() const
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    return cell->faktor( m_point.x(), m_point.y() );
}

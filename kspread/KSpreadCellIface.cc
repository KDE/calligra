#include "KSpreadCellIface.h"

#include "kspread_table.h"
#include "kspread_cell.h"
#include <qpen.h>

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

QString KSpreadCellIface::comment() const
{
    KSpreadCell* cell = m_table->cellAt( m_point );
    return cell->comment(m_point.x(), m_point.y());
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
	case KSpreadCell::date_format17:
                stringFormat="date format";
                break;
        case KSpreadCell::Time:
                stringFormat="Time";
                break;
        case KSpreadCell::SecondeTime:
                stringFormat="SecondeTime";
                break;
        case KSpreadCell::Time_format1:
        case KSpreadCell::Time_format2:
        case KSpreadCell::Time_format3:
                stringFormat="time format";
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

//border left
void KSpreadCellIface::setLeftBorderStyle( const QString& _style )
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    if(_style=="DotLine")
        cell->setLeftBorderStyle(Qt::DotLine);
    else if(_style=="DashLine")
        cell->setLeftBorderStyle(Qt::DashLine);
    else if(_style=="DashDotLine")
        cell->setLeftBorderStyle(Qt::DashDotLine);
    else if(_style=="DashDotDotLine")
        cell->setLeftBorderStyle(Qt::DashDotDotLine);
    else if(_style=="SolidLine")
        cell->setLeftBorderStyle(Qt::SolidLine);
    else
        cell->setLeftBorderStyle(Qt::SolidLine);
    cell->update();

}

void KSpreadCellIface::setLeftBorderColor(const QString& _c)
{
    QColor c(_c);
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    cell->setLeftBorderColor(c );
    cell->update();

}
void KSpreadCellIface::setLeftBorderColor(int r,int g,int b)
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    QColor c(r,g,b);
    cell->setLeftBorderColor(c );
    cell->update();
}

void KSpreadCellIface::setLeftBorderWidth( int _size )
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    cell->setLeftBorderWidth( _size );
    cell->update();

}


int  KSpreadCellIface::leftBorderWidth() const
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    return cell->leftBorderWidth(m_point.x(), m_point.y());
}

QString KSpreadCellIface::leftBorderColor() const
{
    KSpreadCell* cell = m_table->cellAt( m_point );
    return cell->leftBorderColor( m_point.x(), m_point.y() ).name();
}

QString KSpreadCellIface::leftBorderStyle() const
{
    KSpreadCell* cell = m_table->cellAt( m_point );
    Qt::PenStyle penStyle=cell->leftBorderStyle( m_point.x(), m_point.y() );
    QString tmp;
    if( penStyle==Qt::DotLine)
        tmp="DotLine";
    else if( penStyle==Qt::DashLine)
        tmp="DashLine";
    else if( penStyle==Qt::DashDotLine)
        tmp="DashDotLine";
    else if( penStyle==Qt::DashDotDotLine)
        tmp="DashDotDotLine";
    else if( penStyle==Qt::SolidLine)
        tmp="SolidLine";
    else
        tmp="SolidLine";
    return tmp;
}

//border right
void KSpreadCellIface::setRightBorderStyle( const QString& _style )
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    if(_style=="DotLine")
        cell->setRightBorderStyle(Qt::DotLine);
    else if(_style=="DashLine")
        cell->setRightBorderStyle(Qt::DashLine);
    else if(_style=="DashDotLine")
        cell->setRightBorderStyle(Qt::DashDotLine);
    else if(_style=="DashDotDotLine")
        cell->setRightBorderStyle(Qt::DashDotDotLine);
    else if(_style=="SolidLine")
        cell->setRightBorderStyle(Qt::SolidLine);
    else
        cell->setRightBorderStyle(Qt::SolidLine);
    cell->update();

}

void KSpreadCellIface::setRightBorderColor(const QString& _c)
{
    QColor c(_c);
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    cell->setRightBorderColor(c );
    cell->update();

}
void KSpreadCellIface::setRightBorderColor(int r,int g,int b)
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    QColor c(r,g,b);
    cell->setRightBorderColor(c );
    cell->update();
}

void KSpreadCellIface::setRightBorderWidth( int _size )
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    cell->setRightBorderWidth( _size );
    cell->update();

}

int  KSpreadCellIface::rightBorderWidth() const
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    return cell->rightBorderWidth(m_point.x(), m_point.y());
}

QString KSpreadCellIface::rightBorderColor() const
{
    KSpreadCell* cell = m_table->cellAt( m_point );
    return cell->rightBorderColor( m_point.x(), m_point.y() ).name();
}

QString KSpreadCellIface::rightBorderStyle() const
{
    KSpreadCell* cell = m_table->cellAt( m_point );
    Qt::PenStyle penStyle=cell->rightBorderStyle( m_point.x(), m_point.y() );
    QString tmp;
    if( penStyle==Qt::DotLine)
        tmp="DotLine";
    else if( penStyle==Qt::DashLine)
        tmp="DashLine";
    else if( penStyle==Qt::DashDotLine)
        tmp="DashDotLine";
    else if( penStyle==Qt::DashDotDotLine)
        tmp="DashDotDotLine";
    else if( penStyle==Qt::SolidLine)
        tmp="SolidLine";
    else
        tmp="SolidLine";
    return tmp;
}

//border top
void KSpreadCellIface::setTopBorderStyle( const QString& _style )
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    if(_style=="DotLine")
        cell->setTopBorderStyle(Qt::DotLine);
    else if(_style=="DashLine")
        cell->setTopBorderStyle(Qt::DashLine);
    else if(_style=="DashDotLine")
        cell->setTopBorderStyle(Qt::DashDotLine);
    else if(_style=="DashDotDotLine")
        cell->setTopBorderStyle(Qt::DashDotDotLine);
    else if(_style=="SolidLine")
        cell->setTopBorderStyle(Qt::SolidLine);
    else
        cell->setTopBorderStyle(Qt::SolidLine);
    cell->update();

}

void KSpreadCellIface::setTopBorderColor(const QString& _c)
{
    QColor c(_c);
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    cell->setTopBorderColor(c );
    cell->update();

}
void KSpreadCellIface::setTopBorderColor(int r,int g,int b)
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    QColor c(r,g,b);
    cell->setTopBorderColor(c );
    cell->update();
}

void KSpreadCellIface::setTopBorderWidth( int _size )
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    cell->setTopBorderWidth( _size );
    cell->update();
}

int  KSpreadCellIface::topBorderWidth() const
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    return cell->topBorderWidth(m_point.x(), m_point.y());
}

QString KSpreadCellIface::topBorderColor() const
{
    KSpreadCell* cell = m_table->cellAt( m_point );
    return cell->topBorderColor( m_point.x(), m_point.y() ).name();
}

QString KSpreadCellIface::topBorderStyle() const
{
    KSpreadCell* cell = m_table->cellAt( m_point );
    Qt::PenStyle penStyle=cell->topBorderStyle( m_point.x(), m_point.y() );
    QString tmp;
    if( penStyle==Qt::DotLine)
        tmp="DotLine";
    else if( penStyle==Qt::DashLine)
        tmp="DashLine";
    else if( penStyle==Qt::DashDotLine)
        tmp="DashDotLine";
    else if( penStyle==Qt::DashDotDotLine)
        tmp="DashDotDotLine";
    else if( penStyle==Qt::SolidLine)
        tmp="SolidLine";
    else
        tmp="SolidLine";
    return tmp;
}

//border bottom
void KSpreadCellIface::setBottomBorderStyle( const QString& _style )
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    if(_style=="DotLine")
        cell->setBottomBorderStyle(Qt::DotLine);
    else if(_style=="DashLine")
        cell->setBottomBorderStyle(Qt::DashLine);
    else if(_style=="DashDotLine")
        cell->setBottomBorderStyle(Qt::DashDotLine);
    else if(_style=="DashDotDotLine")
        cell->setBottomBorderStyle(Qt::DashDotDotLine);
    else if(_style=="SolidLine")
        cell->setBottomBorderStyle(Qt::SolidLine);
    else
        cell->setBottomBorderStyle(Qt::SolidLine);
    cell->update();

}

void KSpreadCellIface::setBottomBorderColor(const QString& _c)
{
    QColor c(_c);
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    cell->setBottomBorderColor(c );
    cell->update();

}
void KSpreadCellIface::setBottomBorderColor(int r,int g,int b)
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    QColor c(r,g,b);
    cell->setBottomBorderColor(c );
    cell->update();
}

void KSpreadCellIface::setBottomBorderWidth( int _size )
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    cell->setBottomBorderWidth( _size );
    cell->update();
}

int  KSpreadCellIface::bottomBorderWidth() const
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    return cell->bottomBorderWidth(m_point.x(), m_point.y());
}

QString KSpreadCellIface::bottomBorderColor() const
{
    KSpreadCell* cell = m_table->cellAt( m_point );
    return cell->bottomBorderColor( m_point.x(), m_point.y() ).name();
}

QString KSpreadCellIface::bottomBorderStyle() const
{
    KSpreadCell* cell = m_table->cellAt( m_point );
    Qt::PenStyle penStyle=cell->bottomBorderStyle( m_point.x(), m_point.y() );
    QString tmp;
    if( penStyle==Qt::DotLine)
        tmp="DotLine";
    else if( penStyle==Qt::DashLine)
        tmp="DashLine";
    else if( penStyle==Qt::DashDotLine)
        tmp="DashDotLine";
    else if( penStyle==Qt::DashDotDotLine)
        tmp="DashDotDotLine";
    else if( penStyle==Qt::SolidLine)
        tmp="SolidLine";
    else
        tmp="SolidLine";
    return tmp;
}

//fall back diagonal
void KSpreadCellIface::setFallDiagonalStyle( const QString& _style )
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    if(_style=="DotLine")
        cell->setFallDiagonalStyle(Qt::DotLine);
    else if(_style=="DashLine")
        cell->setFallDiagonalStyle(Qt::DashLine);
    else if(_style=="DashDotLine")
        cell->setFallDiagonalStyle(Qt::DashDotLine);
    else if(_style=="DashDotDotLine")
        cell->setFallDiagonalStyle(Qt::DashDotDotLine);
    else if(_style=="SolidLine")
        cell->setFallDiagonalStyle(Qt::SolidLine);
    else
        cell->setFallDiagonalStyle(Qt::SolidLine);
    cell->update();

}

void KSpreadCellIface::setFallDiagonalColor(const QString& _c)
{
    QColor c(_c);
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    cell->setFallDiagonalColor(c );
    cell->update();

}
void KSpreadCellIface::setFallDiagonalColor(int r,int g,int b)
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    QColor c(r,g,b);
    cell->setFallDiagonalColor(c );
    cell->update();
}

void KSpreadCellIface::setFallDiagonalWidth( int _size )
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    cell->setFallDiagonalWidth( _size );
    cell->update();
}

int  KSpreadCellIface::fallDiagonalWidth() const
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    return cell->fallDiagonalWidth(m_point.x(), m_point.y());
}

QString KSpreadCellIface::fallDiagonalColor() const
{
    KSpreadCell* cell = m_table->cellAt( m_point );
    return cell->fallDiagonalColor( m_point.x(), m_point.y() ).name();
}

QString KSpreadCellIface::fallDiagonalStyle() const
{
    KSpreadCell* cell = m_table->cellAt( m_point );
    Qt::PenStyle penStyle=cell->fallDiagonalStyle( m_point.x(), m_point.y() );
    QString tmp;
    if( penStyle==Qt::DotLine)
        tmp="DotLine";
    else if( penStyle==Qt::DashLine)
        tmp="DashLine";
    else if( penStyle==Qt::DashDotLine)
        tmp="DashDotLine";
    else if( penStyle==Qt::DashDotDotLine)
        tmp="DashDotDotLine";
    else if( penStyle==Qt::SolidLine)
        tmp="SolidLine";
    else
        tmp="SolidLine";
    return tmp;
}


//GoUpDiagonal
void KSpreadCellIface::setGoUpDiagonalStyle( const QString& _style )
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    if(_style=="DotLine")
        cell->setGoUpDiagonalStyle(Qt::DotLine);
    else if(_style=="DashLine")
        cell->setGoUpDiagonalStyle(Qt::DashLine);
    else if(_style=="DashDotLine")
        cell->setGoUpDiagonalStyle(Qt::DashDotLine);
    else if(_style=="DashDotDotLine")
        cell->setGoUpDiagonalStyle(Qt::DashDotDotLine);
    else if(_style=="SolidLine")
        cell->setGoUpDiagonalStyle(Qt::SolidLine);
    else
        cell->setGoUpDiagonalStyle(Qt::SolidLine);
    cell->update();

}

void KSpreadCellIface::setGoUpDiagonalColor(const QString& _c)
{
    QColor c(_c);
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    cell->setGoUpDiagonalColor(c );
    cell->update();

}
void KSpreadCellIface::setGoUpDiagonalColor(int r,int g,int b)
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    QColor c(r,g,b);
    cell->setGoUpDiagonalColor(c );
    cell->update();
}

void KSpreadCellIface::setGoUpDiagonalWidth( int _size )
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    cell->setGoUpDiagonalWidth( _size );
    cell->update();
}

int  KSpreadCellIface::goUpDiagonalWidth() const
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    return cell->goUpDiagonalWidth(m_point.x(), m_point.y());
}

QString KSpreadCellIface::goUpDiagonalColor() const
{
    KSpreadCell* cell = m_table->cellAt( m_point );
    return cell->goUpDiagonalColor( m_point.x(), m_point.y() ).name();
}

QString KSpreadCellIface::goUpDiagonalStyle() const
{
    KSpreadCell* cell = m_table->cellAt( m_point );
    Qt::PenStyle penStyle=cell->goUpDiagonalStyle( m_point.x(), m_point.y() );
    QString tmp;
    if( penStyle==Qt::DotLine)
        tmp="DotLine";
    else if( penStyle==Qt::DashLine)
        tmp="DashLine";
    else if( penStyle==Qt::DashDotLine)
        tmp="DashDotLine";
    else if( penStyle==Qt::DashDotDotLine)
        tmp="DashDotDotLine";
    else if( penStyle==Qt::SolidLine)
        tmp="SolidLine";
    else
        tmp="SolidLine";
    return tmp;
}

void KSpreadCellIface::setIndent(int indent)
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    if(indent>=0)
        cell->setIndent(indent);
    else
        cell->setIndent(0);
    cell->update();
}

int  KSpreadCellIface::getIndent() const
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    return cell->getIndent(m_point.x(), m_point.y());
}

void KSpreadCellIface::setDontPrintText ( bool _print)
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    cell->setDontPrintText ( _print);
}

bool KSpreadCellIface::getDontprintText() const
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    return cell->getDontprintText(m_point.x(), m_point.y());
}

/* This file is part of the KDE project

   Copyright 2006 Fredrik Edemar <f_edemar@linux.se>
   Copyright 2003 Lukas Tinkl <lukas@kde.org>
   Copyright 2000, 2002-2003 Laurent Montel <montel@kde.org>
   Copyright 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright 2002 Ariya Hidayat <ariya@kde.org>
   Copyright 2002 Harri Porten <porten@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 2001 Simon Hausmann <hausmann@kde.org>
   Copyright 2000 Werner Trobin <trobin@kde.org>
   Copyright 1999 Torben Weis <weis@kde.org>

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

#include "Sheet.h"
#include "SheetPrint.h"
#include "Util.h"
#include "Doc.h"
#include "DataManipulators.h"

#include <kapplication.h>
#include <kdebug.h>

// #include "KSpreadCellIface.h"
#include "Region.h"

#include "SheetAdaptor.h"

using namespace KSpread;

SheetAdaptor::SheetAdaptor( Sheet* t )
: QDBusAbstractAdaptor( t )
{
    setAutoRelaySignals(true);
    m_sheet = t;
}

SheetAdaptor::~SheetAdaptor()
{
}

QString SheetAdaptor::cellName( int x, int y )
{
    /* sebsauer 20061206: commented out cause if someone starts to believe that
    counting starts by 0,0 then he may very surprised why the first col/row
    got returned twice. That happened to me and I was believing that's a KSpread
    bug too :-(
    // if someone calls us with either x or y 0 he _most_ most likely doesn't
    // know that the cell counting starts with 1 (Simon)
    // P.S.: I did that mistake for weeks and already started looking for the
    // "bug" in kspread ;-)
    if ( x == 0 ) x = 1;
    if ( y == 0 ) y = 1;
    */
    return Cell::name( x, y );
}

int SheetAdaptor::cellRow( const QString& cellname )
{
    return cellLocation(cellname).x();
}

int SheetAdaptor::cellColumn( const QString& cellname )
{
    return cellLocation(cellname).y();
}

QPoint SheetAdaptor::cellLocation( const QString& cellname )
{
    const QRect rect = (*Region( m_sheet->doc()->map(), cellname, m_sheet ).constBegin())->rect();
    if ( rect.isNull() ) return QPoint();
    return rect.topLeft();
}

QString SheetAdaptor::text( int x, int y )
{
    Cell* cell = m_sheet->cellAt(x, y);
    return cell ? cell->text() : QString();
}

QString SheetAdaptor::text( const QString& cellname )
{
    const QPoint location = cellLocation(cellname);
    return text(location.x(), location.y());
}

bool SheetAdaptor::setText( int x, int y, const QString& text, bool parse )
{
    //FIXME: there is some problem with asString parameter, when it's set
	//to true KSpread says: ASSERT: "f" in Dependencies.cpp (621)
	//kspread: Cell at row 6, col 1 marked as formula, but formula is NULL

	KSpread::ProtectedCheck prot;
	prot.setSheet(m_sheet);
	prot.add(QPoint(x, y));
	if(prot.check()) return false;

	KSpread::DataManipulator *dm = new KSpread::DataManipulator();
	dm->setSheet(m_sheet);
	dm->setValue(Value(text));
	dm->setParsing(parse);
	dm->add(QPoint(x, y));
	dm->execute();
	return true;
}

bool SheetAdaptor::setText( const QString& cellname, const QString& text, bool parse )
{
    const QPoint location = cellLocation(cellname);
    return setText(location.x(), location.y(), text, parse);
}

QVariant valueToVariant(const KSpread::Value& value)
{
	//Should we use following value-format enums here?
	//fmt_None, fmt_Boolean, fmt_Number, fmt_Percent, fmt_Money, fmt_DateTime, fmt_Date, fmt_Time, fmt_String
	switch(value.type()) {
		case KSpread::Value::Empty:
			return QVariant();
		case KSpread::Value::Boolean:
			return QVariant( value.asBoolean() );
		case KSpread::Value::Integer:
			return static_cast<Q_LLONG>(value.asInteger());
		case KSpread::Value::Float:
			return (float)value.asFloat();
		case KSpread::Value::String:
			return value.asString();
		case KSpread::Value::Array: {
			QVariantList colarray;
			for(uint j = 0; j < value.rows(); j++) {
				QVariantList rowarray;
				for( uint i = 0; i < value.columns(); i++) {
					KSpread::Value v = value.element(i,j);
					rowarray.append( valueToVariant(v) );
				}
				colarray.append(rowarray);
			}
			return colarray;
		} break;
		case KSpread::Value::CellRange:
			//FIXME: not yet used
			return QVariant();
		case KSpread::Value::Error:
			return QVariant();
	}
	return QVariant();
}

QVariant SheetAdaptor::value( int x, int y )
{
    Cell* cell = m_sheet ? m_sheet->cellAt(x, y) : 0;
    return cell ? valueToVariant( cell->value() ) : QVariant();
}

QVariant SheetAdaptor::value( const QString& cellname )
{
    const QPoint location = cellLocation(cellname);
    return value(location.x(), location.y());
}

bool SheetAdaptor::setValue( int x, int y, const QVariant& value )
{
	Cell* cell = m_sheet ? m_sheet->cellAt(x, y) : 0;
	if(! cell) return false;
	KSpread::Value v = cell->value();
	switch( value.type() ) {
		case QVariant::Bool: v.setValue( value.toBool() ); break;
		case QVariant::ULongLong: v.setValue( (long)value.toLongLong() ); break;
		case QVariant::Int: v.setValue( value.toInt() ); break;
		case QVariant::Double: v.setValue( value.toDouble() ); break;
		case QVariant::String: v.setValue( value.toString() ); break;
		//case QVariant::Date: v.setValue( value.toDate() ); break;
		//case QVariant::Time: v.setValue( value.toTime() ); break;
		//case QVariant::DateTime: v.setValue( value.toDateTime() ); break;
		default: return false;
	}
	return true;
}

bool SheetAdaptor::setValue( const QString& cellname, const QVariant& value )
{
    const QPoint location = cellLocation(cellname);
    return setValue(location.x(), location.y(), value);
}

QString SheetAdaptor::column( int _col )
{
    //First col number = 1
    if(_col <1)
        return QString();
    return "";//m_sheet->nonDefaultColumnFormat( _col )->getName/*objectName*/();/*dcopObject()->objId()*/

}

QString SheetAdaptor::row( int _row )
{
    //First row number = 1
    if(_row <1)
        return QString();
    return "";//m_sheet->nonDefaultRowFormat( _row )->/*dcopObject()->*/getName/*objectName*/();
}

int SheetAdaptor::lastColumn() const
{
    Cell* cell = m_sheet->firstCell();
    return cell ? cell->column() : 0;
}

int SheetAdaptor::lastRow() const
{
    Cell* cell = m_sheet->firstCell();
    return cell ? cell->row() : 0;
}

int SheetAdaptor::maxColumn() const
{
    return m_sheet->maxColumn();

}

int SheetAdaptor::maxRow() const
{
    return m_sheet->maxRow();
}

// bool SheetAdaptor::processDynamic( const DCOPCString& fun, const QByteArray&/*data*/,
//                                         DCOPCString& replyType, QByteArray &replyData )
// {
//     kDebug(36001) << "Calling '" << fun.data() << '\'' << endl;
//     // Does the name follow the pattern "foobar()" ?
//     uint len = fun.length();
//     if ( len < 3 )
//         return false;
// 
//     if ( fun[ len - 1 ] != ')' || fun[ len - 2 ] != '(' )
//         return false;
// 
//     // Is the function name a valid cell like "B5" ?
//     Point p( fun.left( len - 2 ).data() );
//     if ( !p.isValid() )
//         return false;
// 
//     DCOPCString str = objId() + '/' + fun.left( len - 2 );
// 
//     replyType = "DCOPRef";
//     QDataStream out( &replyData,QIODevice::WriteOnly );
//     out.setVersion(QDataStream::Qt_3_1);
//     out << DCOPRef( kapp->dcopClient()->appId(), str );
//     return true;
// }

QString SheetAdaptor::sheetName() const
{
    return m_sheet->sheetName();
}

bool SheetAdaptor::setSheetName( const QString & name)
{
    return m_sheet->setSheetName( name);
}

bool SheetAdaptor::insertColumn( int col,int nbCol )
{
    return m_sheet->insertColumn(col,nbCol);
}

bool SheetAdaptor::insertRow( int row,int nbRow)
{
    return m_sheet->insertRow(row,nbRow);
}

void SheetAdaptor::removeColumn( int col,int nbCol )
{
    m_sheet->removeColumn( col,nbCol );
}

void SheetAdaptor::removeRow( int row,int nbRow )
{
    m_sheet->removeRow( row,nbRow );
}


bool SheetAdaptor::isHidden()const
{
    return m_sheet->isHidden();
}


bool SheetAdaptor::showGrid() const
{
    return m_sheet->getShowGrid();
}

bool SheetAdaptor::showFormula() const
{
    return m_sheet->getShowFormula();
}

bool SheetAdaptor::lcMode() const
{
    return m_sheet->getLcMode();
}

bool SheetAdaptor::autoCalc() const
{
    return m_sheet->getAutoCalc();
}

bool SheetAdaptor::showColumnNumber() const
{
    return m_sheet->getShowColumnNumber();
}

bool SheetAdaptor::hideZero() const
{
    return m_sheet->getHideZero();
}

bool SheetAdaptor::firstLetterUpper() const
{
    return m_sheet->getFirstLetterUpper();
}

void SheetAdaptor::setShowPageBorders( bool b )
{
    m_sheet->setShowPageBorders( b );
    m_sheet->doc()->updateBorderButton();
}

float SheetAdaptor::paperHeight()const
{
    return m_sheet->print()->paperHeight();
}

float SheetAdaptor::paperWidth()const
{
    return m_sheet->print()->paperWidth();
}

float SheetAdaptor::leftBorder()const
{
    return m_sheet->print()->leftBorder();
}

float SheetAdaptor::rightBorder()const
{
    return m_sheet->print()->rightBorder();
}

float SheetAdaptor::topBorder()const
{
    return m_sheet->print()->topBorder();
}

float SheetAdaptor::bottomBorder()const
{
    return m_sheet->print()->bottomBorder();
}

QString SheetAdaptor::paperFormatString() const
{
    return m_sheet->print()->paperFormatString();
}

QString SheetAdaptor::headLeft()const
{
    return m_sheet->print()->headLeft();
}

QString SheetAdaptor::headMid()const
{
    return m_sheet->print()->headMid();
}

QString SheetAdaptor::headRight()const
{
    return m_sheet->print()->headRight();
}

QString SheetAdaptor::footLeft()const
{
    return m_sheet->print()->footLeft();
}

QString SheetAdaptor::footMid()const
{
    return m_sheet->print()->footMid();
}

QString SheetAdaptor::footRight()const
{
    return m_sheet->print()->footRight();
}

void SheetAdaptor::setHeaderLeft(const QString & text)
{
    m_sheet->print()->setHeadFootLine( text,       headMid(), headRight(),
                                       footLeft(), footMid(), footRight() );
}

void SheetAdaptor::setHeaderMiddle(const QString & text)
{
    m_sheet->print()->setHeadFootLine( headLeft(), text,      headRight(),
                                       footLeft(), footMid(), footRight() );

}

void SheetAdaptor::setHeaderRight(const QString & text)
{
    m_sheet->print()->setHeadFootLine( headLeft(), headMid(), text,
                                       footLeft(), footMid(), footRight() );
}

void SheetAdaptor::setFooterLeft(const QString & text)
{
    m_sheet->print()->setHeadFootLine( headLeft(), headMid(), headRight(),
                                       text,       footMid(), footRight() );
}

void SheetAdaptor::setFooterMiddle(const QString & text)
{
    m_sheet->print()->setHeadFootLine( headLeft(), headMid(), headRight(),
                                       footLeft(), text,      footRight() );
}

void SheetAdaptor::setFooterRight(const QString & text)
{
    m_sheet->print()->setHeadFootLine( headLeft(), headMid(), headRight(),
                                       footLeft(), footMid(), text );
}

bool SheetAdaptor::isProtected() const
{
    return m_sheet->isProtected();
}

#include "SheetAdaptor.moc"

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

#include <kapplication.h>
#include <kdebug.h>

// #include "KSpreadCellIface.h"
#include "Region.h"

#include "KSpreadSheetAdaptor.h"

using namespace KSpread;

/*********************************************
 *
 * CellProxy
 *
 *********************************************/

// class KSpread::CellProxy : public DCOPObjectProxy
// {
// public:
//     CellProxy( Sheet* sheet, const QByteArray& prefix );
//     ~CellProxy();
// 
//     virtual bool process( const DCOPCString& obj, const DCOPCString& fun, const QByteArray& data,
//                           DCOPCString& replyType, QByteArray &replyData );
// 
// private:
//     QByteArray m_prefix;
//     CellIface* m_cell;
//     Sheet* m_sheet;
// };

// KSpread::CellProxy::CellProxy( Sheet* sheet, const QByteArray& prefix )
//     : DCOPObjectProxy(), m_prefix( prefix )
// {
//     m_cell = new CellIface;
//     m_sheet = sheet;
// }
// 
// KSpread::CellProxy::~CellProxy()
// {
//     delete m_cell;
// }
// 
// bool KSpread::CellProxy::process( const DCOPCString& obj, const DCOPCString& fun,
//                                   const QByteArray& data,
//                                   DCOPCString& replyType, QByteArray &replyData )
// {
// 
// 	kDebug()<<"CellProxy::process: requested object:"<<obj<<endl;
// 	kDebug()<<"CellProxy::process: prefix:"<<m_prefix<<endl;
//     if ( strncmp( m_prefix.data(), obj.data(), m_prefix.length() ) != 0 )
//         return false;
// 
// 	if ( fun == "functions()" ) {
//         	replyType = "DCOPCStringList";
// 	        QDataStream reply( &replyData,QIODevice::WriteOnly );
// 	        reply.setVersion(QDataStream::Qt_3_1);
// 		 DCOPCStringList repList=m_cell->functions();
// 		reply<<repList;
// 	        return true;
// 	}
// 
//     QString cellID=QString::fromUtf8(obj.data() + m_prefix.length());
//     cellID=m_sheet->sheetName()+'!'+cellID;
// 
//     kDebug()<<"CellProxy::process: cellID="<<cellID<<endl;
// 
//     Point p( cellID); //obj.data() + m_prefix.length() );
//     if ( p.pos().x()<0 ) {
// 	kDebug(36001)<<"CellProyxy::process: resulting Point is not valid"<<endl;
//         return false;
//     }
// 
//     kDebug(36001)<<"all checks finsihed, trying to access cell (x):"<<p.pos().x()<<endl;
// 
//     m_cell->setCell( m_sheet, p.pos() );
//     return m_cell->process( fun, data, replyType, replyData );
// }

/************************************************
 *
 * SheetAdaptor
 *
 ************************************************/

SheetAdaptor::SheetAdaptor( Sheet* t )
: QDBusAbstractAdaptor( t )
{
    setAutoRelaySignals(true);
//     m_proxy=0;
    m_sheet = t;

//     sheetNameHasChanged();
}

// void SheetAdaptor::sheetNameHasChanged() {
//   ident.resize(1);
//   QObject *currentObj = m_sheet;
//     while (currentObj != 0) {
//         ident.prepend( currentObj->objectName().toUtf8() );
//         ident.prepend('/');
//         currentObj = currentObj->parent();
//     }
//     if ( ident[0] == '/' )
//         ident = ident.mid(1);
// 
//    if (qstrcmp(ident,objId())!=0) {
// 	   setObjId(ident);
// 
// //            delete m_proxy;
//            QString str = objId();
//            str += '/';
// 	   kDebug(36001)<<"SheetAdaptor::sheetNameHasChanged(): new DCOP-ID:"<<objId()<<endl;
// //            m_proxy = new CellProxy( m_sheet, str );
//    }
// 
// }


SheetAdaptor::~SheetAdaptor()
{
//     delete m_proxy;
}

QString SheetAdaptor::cell( int x, int y )
{
    // if someone calls us with either x or y 0 he _most_ most likely doesn't
    // know that the cell counting starts with 1 (Simon)
    // P.S.: I did that mistake for weeks and already started looking for the
    // "bug" in kspread ;-)
    if ( x == 0 )
        x = 1;
    if ( y == 0 )
        y = 1;

    return /*objId() + '/' + */Cell::name( x, y ).toLatin1();
}

QString SheetAdaptor::cell( const QString& name )
{
  Q_UNUSED(name)
//     QString str = objeId();
//     str += '/';
//     str += name.toLatin1();

    return "str";
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


QString SheetAdaptor::name() const
{
    return m_sheet->sheetName();
}


int SheetAdaptor::maxColumn() const
{
    return m_sheet->maxColumn();

}

bool SheetAdaptor::areaHasNoContent(QRect area) const
{
	kDebug(36001) << "SheetAdaptor::areaHasNoContent("<<area<<");"<<endl;
	return m_sheet->areaIsEmpty(area);
}

bool SheetAdaptor::areaHasNoComments(QRect area) const
{
	return m_sheet->areaIsEmpty(area, Sheet::Comment);
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

#include "KSpreadSheetAdaptor.moc"

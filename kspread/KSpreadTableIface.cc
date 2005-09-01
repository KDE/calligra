/* This file is part of the KDE project

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "KSpreadTableIface.h"

#include "kspread_sheet.h"
#include "kspread_sheetprint.h"
#include "kspread_util.h"
#include "kspread_doc.h"

#include "KSpreadCellIface.h"

#include <kapplication.h>
#include <dcopclient.h>
#include <kdebug.h>

/*********************************************
 *
 * KSpreadCellProxy
 *
 *********************************************/

class KSpreadCellProxy : public DCOPObjectProxy
{
public:
    KSpreadCellProxy( KSpreadSheet* sheet, const QCString& prefix );
    ~KSpreadCellProxy();

    virtual bool process( const QCString& obj, const QCString& fun, const QByteArray& data,
                          QCString& replyType, QByteArray &replyData );

private:
    QCString m_prefix;
    KSpreadCellIface* m_cell;
    KSpreadSheet* m_sheet;
};

KSpreadCellProxy::KSpreadCellProxy( KSpreadSheet* sheet, const QCString& prefix )
    : DCOPObjectProxy( kapp->dcopClient() ), m_prefix( prefix )
{
    m_cell = new KSpreadCellIface;
    m_sheet = sheet;
}

KSpreadCellProxy::~KSpreadCellProxy()
{
    delete m_cell;
}

bool KSpreadCellProxy::process( const QCString& obj, const QCString& fun, const QByteArray& data,
                                        QCString& replyType, QByteArray &replyData )
{

	kdDebug()<<"KSpreadCellProxy::process: requested object:"<<obj<<endl;
	kdDebug()<<"KSpreadCellProxy::process: prefix:"<<m_prefix<<endl;
    if ( strncmp( m_prefix.data(), obj.data(), m_prefix.length() ) != 0 )
        return false;

	if ( fun == "functions()" ) {
        	replyType = "QCStringList";
	        QDataStream reply( replyData, IO_WriteOnly );
		 QCStringList repList=m_cell->functions();
		reply<<repList;
	        return true;
	}

    QString cellID=QString::fromUtf8(obj.data() + m_prefix.length());
    cellID=m_sheet->sheetName()+"!"+cellID;

    kdDebug()<<"KSpreadCellProxy::process: cellID="<<cellID<<endl;

    KSpreadPoint p( cellID); //obj.data() + m_prefix.length() );
    if ( p.pos.x()<0 ) {
	kdDebug(36001)<<"KSpreadCellProyxy::process: resulting KSpreadPoint is not valid"<<endl;
        return false;
    }

    kdDebug(36001)<<"all checks finsihed, trying to access cell (x):"<<p.pos.x()<<endl;

    m_cell->setCell( m_sheet, p.pos );
    return m_cell->process( fun, data, replyType, replyData );
}

/************************************************
 *
 * KSpreadSheetIface
 *
 ************************************************/

KSpreadSheetIface::KSpreadSheetIface( KSpreadSheet* t )
    : DCOPObject()
{
    m_proxy=0;
    m_sheet = t;

    sheetNameHasChanged();

}

void KSpreadSheetIface::sheetNameHasChanged() {
  ident.resize(1);
  QObject *currentObj = m_sheet;
    while (currentObj != 0L) {
        ident.prepend( currentObj->name() );
        ident.prepend("/");
        currentObj = currentObj->parent();
    }
    if ( ident[0] == '/' )
        ident = ident.mid(1);

   if (qstrcmp(ident,objId())!=0) {
	   setObjId(ident);

           delete m_proxy;
           QCString str = objId();
           str += "/";
	   kdDebug(36001)<<"KSpreadSheetIface::tableNameHasChanged(): new DCOP-ID:"<<objId()<<endl;
           m_proxy = new KSpreadCellProxy( m_sheet, str );
   }

}


KSpreadSheetIface::~KSpreadSheetIface()
{
    delete m_proxy;
}

DCOPRef KSpreadSheetIface::cell( int x, int y )
{
    // if someone calls us with either x or y 0 he _most_ most likely doesn't
    // know that the cell counting starts with 1 (Simon)
    // P.S.: I did that mistake for weeks and already started looking for the
    // "bug" in kspread ;-)
    if ( x == 0 )
        x = 1;
    if ( y == 0 )
        y = 1;

    QCString str = objId() + '/' + KSpreadCell::name( x, y ).latin1();

    return DCOPRef( kapp->dcopClient()->appId(), str );
}

DCOPRef KSpreadSheetIface::cell( const QString& name )
{
    QCString str = objId();
    str += "/";
    str += name.latin1();

    return DCOPRef( kapp->dcopClient()->appId(), str );
}

DCOPRef KSpreadSheetIface::column( int _col )
{
    //First col number = 1
    if(_col <1)
        return DCOPRef();
    return DCOPRef( kapp->dcopClient()->appId(),
		    m_sheet->nonDefaultColumnFormat( _col )->dcopObject()->objId() );

}

DCOPRef KSpreadSheetIface::row( int _row )
{
    //First row number = 1
    if(_row <1)
        return DCOPRef();
    return DCOPRef( kapp->dcopClient()->appId(),
		    m_sheet->nonDefaultRowFormat( _row )->dcopObject()->objId() );
}


QString KSpreadSheetIface::name() const
{
    return m_sheet->sheetName();
}


int KSpreadSheetIface::maxColumn() const
{
    return m_sheet->maxColumn();

}

bool KSpreadSheetIface::areaHasNoContent(QRect area) const
{
	kdDebug(36001) << "KSpreadSheetIface::areaHasNoContent("<<area<<");"<<endl;
	return m_sheet->areaIsEmpty(area);
}

bool KSpreadSheetIface::areaHasNoComments(QRect area) const
{
	return m_sheet->areaIsEmpty(area,KSpreadSheet::Comment);
}

int KSpreadSheetIface::maxRow() const
{
    return m_sheet->maxRow();
}

bool KSpreadSheetIface::processDynamic( const QCString& fun, const QByteArray&/*data*/,
                                        QCString& replyType, QByteArray &replyData )
{
    kdDebug(36001) << "Calling '" << fun.data() << "'" << endl;
    // Does the name follow the pattern "foobar()" ?
    uint len = fun.length();
    if ( len < 3 )
        return false;

    if ( fun[ len - 1 ] != ')' || fun[ len - 2 ] != '(' )
        return false;

    // Is the function name a valid cell like "B5" ?
    KSpreadPoint p( fun.left( len - 2 ).data() );
    if ( !p.isValid() )
        return false;

    QCString str = objId() + "/" + fun.left( len - 2 );

    replyType = "DCOPRef";
    QDataStream out( replyData, IO_WriteOnly );
    out << DCOPRef( kapp->dcopClient()->appId(), str );
    return true;
}

bool KSpreadSheetIface::setSheetName( const QString & name)
{
    return m_sheet->setSheetName( name);
}

bool KSpreadSheetIface::insertColumn( int col,int nbCol )
{
    return m_sheet->insertColumn(col,nbCol);
}

bool KSpreadSheetIface::insertRow( int row,int nbRow)
{
    return m_sheet->insertRow(row,nbRow);
}

void KSpreadSheetIface::removeColumn( int col,int nbCol )
{
    m_sheet->removeColumn( col,nbCol );
}

void KSpreadSheetIface::removeRow( int row,int nbRow )
{
    m_sheet->removeRow( row,nbRow );
}


bool KSpreadSheetIface::isHidden()const
{
    return m_sheet->isHidden();
}


bool KSpreadSheetIface::showGrid() const
{
    return m_sheet->getShowGrid();
}

bool KSpreadSheetIface::showFormula() const
{
    return m_sheet->getShowFormula();
}

bool KSpreadSheetIface::lcMode() const
{
    return m_sheet->getLcMode();
}

bool KSpreadSheetIface::autoCalc() const
{
    return m_sheet->getAutoCalc();
}

bool KSpreadSheetIface::showColumnNumber() const
{
    return m_sheet->getShowColumnNumber();
}

bool KSpreadSheetIface::hideZero() const
{
    return m_sheet->getHideZero();
}

bool KSpreadSheetIface::firstLetterUpper() const
{
    return m_sheet->getFirstLetterUpper();
}

void KSpreadSheetIface::setShowPageBorders( bool b )
{
    m_sheet->setShowPageBorders( b );
    m_sheet->doc()->updateBorderButton();
}

float KSpreadSheetIface::paperHeight()const
{
    return m_sheet->print()->paperHeight();
}

float KSpreadSheetIface::paperWidth()const
{
    return m_sheet->print()->paperWidth();
}

float KSpreadSheetIface::leftBorder()const
{
    return m_sheet->print()->leftBorder();
}

float KSpreadSheetIface::rightBorder()const
{
    return m_sheet->print()->rightBorder();
}

float KSpreadSheetIface::topBorder()const
{
    return m_sheet->print()->topBorder();
}

float KSpreadSheetIface::bottomBorder()const
{
    return m_sheet->print()->bottomBorder();
}

QString KSpreadSheetIface::paperFormatString() const
{
    return m_sheet->print()->paperFormatString();
}

QString KSpreadSheetIface::headLeft()const
{
    return m_sheet->print()->headLeft();
}

QString KSpreadSheetIface::headMid()const
{
    return m_sheet->print()->headMid();
}

QString KSpreadSheetIface::headRight()const
{
    return m_sheet->print()->headRight();
}

QString KSpreadSheetIface::footLeft()const
{
    return m_sheet->print()->footLeft();
}

QString KSpreadSheetIface::footMid()const
{
    return m_sheet->print()->footMid();
}

QString KSpreadSheetIface::footRight()const
{
    return m_sheet->print()->footRight();
}

void KSpreadSheetIface::setHeaderLeft(const QString & text)
{
    m_sheet->print()->setHeadFootLine( text,       headMid(), headRight(),
                                       footLeft(), footMid(), footRight() );
}

void KSpreadSheetIface::setHeaderMiddle(const QString & text)
{
    m_sheet->print()->setHeadFootLine( headLeft(), text,      headRight(),
                                       footLeft(), footMid(), footRight() );

}

void KSpreadSheetIface::setHeaderRight(const QString & text)
{
    m_sheet->print()->setHeadFootLine( headLeft(), headMid(), text,
                                       footLeft(), footMid(), footRight() );
}

void KSpreadSheetIface::setFooterLeft(const QString & text)
{
    m_sheet->print()->setHeadFootLine( headLeft(), headMid(), headRight(),
                                       text,       footMid(), footRight() );
}

void KSpreadSheetIface::setFooterMiddle(const QString & text)
{
    m_sheet->print()->setHeadFootLine( headLeft(), headMid(), headRight(),
                                       footLeft(), text,      footRight() );
}

void KSpreadSheetIface::setFooterRight(const QString & text)
{
    m_sheet->print()->setHeadFootLine( headLeft(), headMid(), headRight(),
                                       footLeft(), footMid(), text );
}

bool KSpreadSheetIface::isProtected() const
{
    return m_sheet->isProtected();
}

#include "KSpreadTableIface.h"
#include "KSpreadCellIface.h"
#include "KSpreadColumnIface.h"
#include "KSpreadRowIface.h"


#include "kspread_table.h"
#include "kspread_util.h"
#include "kspread_doc.h"

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
    KSpreadCellProxy( KSpreadSheet* table, const QCString& prefix );
    ~KSpreadCellProxy();

    virtual bool process( const QCString& obj, const QCString& fun, const QByteArray& data,
                          QCString& replyType, QByteArray &replyData );

private:
    QCString m_prefix;
    KSpreadCellIface* m_cell;
    KSpreadSheet* m_table;
};

KSpreadCellProxy::KSpreadCellProxy( KSpreadSheet* table, const QCString& prefix )
    : DCOPObjectProxy( kapp->dcopClient() ), m_prefix( prefix )
{
    m_cell = new KSpreadCellIface;
    m_table = table;
}

KSpreadCellProxy::~KSpreadCellProxy()
{
    delete m_cell;
}

bool KSpreadCellProxy::process( const QCString& obj, const QCString& fun, const QByteArray& data,
                                        QCString& replyType, QByteArray &replyData )
{
    if ( strncmp( m_prefix.data(), obj.data(), m_prefix.length() ) != 0 )
        return FALSE;

    KSpreadPoint p( obj.data() + m_prefix.length() );
    if ( !p.isValid() )
        return FALSE;

    m_cell->setCell( m_table, p.pos );
    return m_cell->process( fun, data, replyType, replyData );
}

/************************************************
 *
 * KSpreadSheetIface
 *
 ************************************************/

KSpreadSheetIface::KSpreadSheetIface( KSpreadSheet* t )
    : DCOPObject( t )
{
    m_table = t;

    QCString str = objId();
    str += "/";
    m_proxy = new KSpreadCellProxy( t, str );

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
		    m_table->nonDefaultColumnLayout( _col )->dcopObject()->objId() );

}

DCOPRef KSpreadSheetIface::row( int _row )
{
    //First row number = 1
    if(_row <1)
        return DCOPRef();
    return DCOPRef( kapp->dcopClient()->appId(),
		    m_table->nonDefaultRowLayout( _row )->dcopObject()->objId() );
}


QString KSpreadSheetIface::name() const
{
    return m_table->tableName();
}


int KSpreadSheetIface::maxColumn() const
{
    return m_table->maxColumn();

}

int KSpreadSheetIface::maxRow() const
{
    return m_table->maxRow();
}

bool KSpreadSheetIface::processDynamic( const QCString& fun, const QByteArray&/*data*/,
                                        QCString& replyType, QByteArray &replyData )
{
    kdDebug(36001) << "Calling '" << fun.data() << "'" << endl;
    // Does the name follow the pattern "foobar()" ?
    uint len = fun.length();
    if ( len < 3 )
        return FALSE;

    if ( fun[ len - 1 ] != ')' || fun[ len - 2 ] != '(' )
        return FALSE;

    // Is the function name a valid cell like "B5" ?
    KSpreadPoint p( fun.left( len - 2 ).data() );
    if ( !p.isValid() )
        return FALSE;

    QCString str = objId() + "/" + fun.left( len - 2 );

    replyType = "DCOPRef";
    QDataStream out( replyData, IO_WriteOnly );
    out << DCOPRef( kapp->dcopClient()->appId(), str );
    return TRUE;
}

bool KSpreadSheetIface::setTableName( const QString & name)
{
    return m_table->setTableName( name);
}

bool KSpreadSheetIface::insertColumn( int col,int nbCol )
{
    return m_table->insertColumn(col,nbCol);
}

bool KSpreadSheetIface::insertRow( int row,int nbRow)
{
    return m_table->insertRow(row,nbRow);
}

void KSpreadSheetIface::removeColumn( int col,int nbCol )
{
    m_table->removeColumn( col,nbCol );
}

void KSpreadSheetIface::removeRow( int row,int nbRow )
{
    m_table->removeRow( row,nbRow );
}


bool KSpreadSheetIface::isHidden()const
{
    return m_table->isHidden();
}


bool KSpreadSheetIface::showGrid() const
{
    return m_table->getShowGrid();
}

bool KSpreadSheetIface::showFormula() const
{
    return m_table->getShowFormula();
}

bool KSpreadSheetIface::lcMode() const
{
    return m_table->getLcMode();
}

bool KSpreadSheetIface::autoCalc() const
{
    return m_table->getAutoCalc();
}

bool KSpreadSheetIface::showColumnNumber() const
{
    return m_table->getShowColumnNumber();
}

bool KSpreadSheetIface::hideZero() const
{
    return m_table->getHideZero();
}

bool KSpreadSheetIface::firstLetterUpper() const
{
    return m_table->getFirstLetterUpper();
}

void KSpreadSheetIface::setShowPageBorders( bool b )
{
    m_table->setShowPageBorders( b );
    m_table->doc()->updateBorderButton();
}

float KSpreadSheetIface::paperHeight()const
{
    return m_table->paperHeight();
}

float KSpreadSheetIface::paperWidth()const
{
    return m_table->paperWidth();
}

float KSpreadSheetIface::leftBorder()const
{
    return m_table->leftBorder();
}

float KSpreadSheetIface::rightBorder()const
{
    return m_table->rightBorder();
}

float KSpreadSheetIface::topBorder()const
{
    return m_table->topBorder();
}

float KSpreadSheetIface::bottomBorder()const
{
    return m_table->bottomBorder();
}

QString KSpreadSheetIface::paperFormatString() const
{
    return m_table->paperFormatString();
}

QString KSpreadSheetIface::headLeft()const
{
    return m_table->headLeft();
}

QString KSpreadSheetIface::headMid()const
{
    return m_table->headMid();
}

QString KSpreadSheetIface::headRight()const
{
    return m_table->headRight();
}

QString KSpreadSheetIface::footLeft()const
{
    return m_table->footLeft();
}

QString KSpreadSheetIface::footMid()const
{
    return m_table->footMid();
}

QString KSpreadSheetIface::footRight()const
{
    return m_table->footRight();
}

void KSpreadSheetIface::setHeaderLeft(const QString & text)
{
    m_table->setHeadFootLine( text,headMid(), headRight(),
                              footLeft(), footMid(),footRight() );
}

void KSpreadSheetIface::setHeaderMiddle(const QString & text)
{
    m_table->setHeadFootLine( headLeft(),text, headRight(),
                              footLeft(), footMid(),footRight() );

}

void KSpreadSheetIface::setHeaderRight(const QString & text)
{
    m_table->setHeadFootLine( headLeft(),headMid(), text,
                              footLeft(), footMid(),footRight() );
}

void KSpreadSheetIface::setFooterLeft(const QString & text)
{
    m_table->setHeadFootLine( headLeft(),headMid(), headRight(),
                              text, footMid(),footRight() );
}

void KSpreadSheetIface::setFooterMiddle(const QString & text)
{
    m_table->setHeadFootLine( headLeft(),headMid(), headRight(),
                              footLeft(), text,footRight() );
}

void KSpreadSheetIface::setFooterRight(const QString & text)
{
    m_table->setHeadFootLine( headLeft(),headMid(), headRight(),
                              footLeft(), footMid(),text );
}


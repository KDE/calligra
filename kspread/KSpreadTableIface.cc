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
    KSpreadCellProxy( KSpreadTable* table, const QCString& prefix );
    ~KSpreadCellProxy();

    virtual bool process( const QCString& obj, const QCString& fun, const QByteArray& data,
                          QCString& replyType, QByteArray &replyData );

private:
    QCString m_prefix;
    KSpreadCellIface* m_cell;
    KSpreadTable* m_table;
};

KSpreadCellProxy::KSpreadCellProxy( KSpreadTable* table, const QCString& prefix )
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
 * KSpreadTableIface
 *
 ************************************************/

KSpreadTableIface::KSpreadTableIface( KSpreadTable* t )
    : DCOPObject( t )
{
    m_table = t;

    QCString str = objId();
    str += "/";
    m_proxy = new KSpreadCellProxy( t, str );

}

KSpreadTableIface::~KSpreadTableIface()
{
    delete m_proxy;
}

DCOPRef KSpreadTableIface::cell( int x, int y )
{
    // if someone calls us with either x or y 0 he _most_ most likely doesn't
    // know that the cell counting starts with 1 (Simon)
    // P.S.: I did that mistake for weeks and already started looking for the
    // "bug" in kspread ;-)
    if ( x == 0 )
        x = 1;
    if ( y == 0 )
        y = 1;

    QCString str = objId();
    str += '/';
    str += util_cellName( x, y ).latin1();

    return DCOPRef( kapp->dcopClient()->appId(), str );
}

DCOPRef KSpreadTableIface::cell( const QString& name )
{
    QCString str = objId();
    str += "/";
    str += name.latin1();

    return DCOPRef( kapp->dcopClient()->appId(), str );
}

DCOPRef KSpreadTableIface::column( int _col )
{
    //First col number = 1
    if(_col <1)
        return DCOPRef();
    return DCOPRef( kapp->dcopClient()->appId(),
		    m_table->nonDefaultColumnLayout( _col )->dcopObject()->objId() );

}

DCOPRef KSpreadTableIface::row( int _row )
{
    //First row number = 1
    if(_row <1)
        return DCOPRef();
    return DCOPRef( kapp->dcopClient()->appId(),
		    m_table->nonDefaultRowLayout( _row )->dcopObject()->objId() );
}


QString KSpreadTableIface::name() const
{
    return m_table->tableName();
}


int KSpreadTableIface::maxColumn() const
{
    return m_table->maxColumn();

}

int KSpreadTableIface::maxRow() const
{
    return m_table->maxRow();
}

bool KSpreadTableIface::processDynamic( const QCString& fun, const QByteArray&/*data*/,
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

bool KSpreadTableIface::setTableName( const QString & name)
{
    return m_table->setTableName( name);
}

bool KSpreadTableIface::insertColumn( int col,int nbCol )
{
    return m_table->insertColumn(col,nbCol);
}

bool KSpreadTableIface::insertRow( int row,int nbRow)
{
    return m_table->insertRow(row,nbRow);
}

void KSpreadTableIface::removeColumn( int col,int nbCol )
{
    m_table->removeColumn( col,nbCol );
}

void KSpreadTableIface::removeRow( int row,int nbRow )
{
    m_table->removeRow( row,nbRow );
}


bool KSpreadTableIface::isHidden()const
{
    return m_table->isHidden();
}


bool KSpreadTableIface::showGrid() const
{
    return m_table->getShowGrid();
}

bool KSpreadTableIface::showFormula() const
{
    return m_table->getShowFormula();
}

bool KSpreadTableIface::lcMode() const
{
    return m_table->getLcMode();
}

bool KSpreadTableIface::autoCalc() const
{
    return m_table->getAutoCalc();
}

bool KSpreadTableIface::showColumnNumber() const
{
    return m_table->getShowColumnNumber();
}

bool KSpreadTableIface::hideZero() const
{
    return m_table->getHideZero();
}

bool KSpreadTableIface::firstLetterUpper() const
{
    return m_table->getFirstLetterUpper();
}

void KSpreadTableIface::setShowPageBorders( bool b )
{
    m_table->setShowPageBorders( b );
    m_table->doc()->updateBorderButton();
}

float KSpreadTableIface::paperHeight()const
{
    return m_table->paperHeight();
}

float KSpreadTableIface::paperWidth()const
{
    return m_table->paperWidth();
}

float KSpreadTableIface::leftBorder()const
{
    return m_table->leftBorder();
}

float KSpreadTableIface::rightBorder()const
{
    return m_table->rightBorder();
}

float KSpreadTableIface::topBorder()const
{
    return m_table->topBorder();
}

float KSpreadTableIface::bottomBorder()const
{
    return m_table->bottomBorder();
}

QString KSpreadTableIface::paperFormatString() const
{
    return m_table->paperFormatString();
}

QString KSpreadTableIface::headLeft()const
{
    return m_table->headLeft();
}

QString KSpreadTableIface::headMid()const
{
    return m_table->headMid();
}

QString KSpreadTableIface::headRight()const
{
    return m_table->headRight();
}

QString KSpreadTableIface::footLeft()const
{
    return m_table->footLeft();
}

QString KSpreadTableIface::footMid()const
{
    return m_table->footMid();
}

QString KSpreadTableIface::footRight()const
{
    return m_table->footRight();
}

void KSpreadTableIface::setHeaderLeft(const QString & text)
{
    m_table->setHeadFootLine( text,headMid(), headRight(),
                              footLeft(), footMid(),footRight() );
}

void KSpreadTableIface::setHeaderMiddle(const QString & text)
{
    m_table->setHeadFootLine( headLeft(),text, headRight(),
                              footLeft(), footMid(),footRight() );

}

void KSpreadTableIface::setHeaderRight(const QString & text)
{
    m_table->setHeadFootLine( headLeft(),headMid(), text,
                              footLeft(), footMid(),footRight() );
}

void KSpreadTableIface::setFooterLeft(const QString & text)
{
    m_table->setHeadFootLine( headLeft(),headMid(), headRight(),
                              text, footMid(),footRight() );
}

void KSpreadTableIface::setFooterMiddle(const QString & text)
{
    m_table->setHeadFootLine( headLeft(),headMid(), headRight(),
                              footLeft(), text,footRight() );
}

void KSpreadTableIface::setFooterRight(const QString & text)
{
    m_table->setHeadFootLine( headLeft(),headMid(), headRight(),
                              footLeft(), footMid(),text );
}


#ifndef KSPREAD_TABLE_IFACE_H
#define KSPREAD_TABLE_IFACE_H

#include <dcopobject.h>
#include <dcopref.h>

#include <qstring.h>
#include <qrect.h>
#include <qcolor.h>

#include "kspread_selection.h"

class KSpreadTable;
class KSpreadCellProxy;

class KSpreadTableIface : virtual public DCOPObject
{
    K_DCOP
public:
    KSpreadTableIface( KSpreadTable* );
    ~KSpreadTableIface();

    bool processDynamic( const QCString& fun, const QByteArray& data,
			 QCString& replyType, QByteArray &replyData );

k_dcop:
    virtual DCOPRef cell( int x, int y );
    virtual DCOPRef cell( const QString& name );

    virtual DCOPRef column( int _col );
    virtual DCOPRef row( int _row );

    virtual QString name() const;
    virtual int maxColumn() const;
    virtual int maxRow() const;
    virtual bool setTableName( const QString & name);


    virtual bool insertColumn( int col,int nbCol );
    virtual bool insertRow( int row,int nbRow);
    virtual void removeColumn( int col,int nbCol );
    virtual void removeRow( int row,int nbRow );

    virtual bool isHidden()const;

    virtual bool showGrid() const;
    virtual bool showFormula() const;
    virtual bool lcMode() const;
    virtual bool autoCalc() const;
    virtual bool showColumnNumber() const;
    virtual bool hideZero() const;
    virtual bool firstLetterUpper() const;
    virtual void setShowPageBorders( bool b );

    virtual float paperHeight() const;
    virtual float paperWidth() const ;
    virtual float leftBorder() const;
    virtual float rightBorder() const;
    virtual float topBorder() const;
    virtual float bottomBorder() const;
    QString paperFormatString() const;

    QString headLeft()const;
    QString headMid()const;
    QString headRight()const;
    QString footLeft()const;
    QString footMid()const;
    QString footRight()const;

    void setHeaderLeft(const QString & text);
    void setHeaderMiddle(const QString & text);
    void setHeaderRight(const QString & text);
    void setFooterLeft(const QString & text);
    void setFooterMiddle(const QString & text);
    void setFooterRight(const QString & text);

private:
    KSpreadTable* m_table;
    KSpreadCellProxy* m_proxy;
};

#endif

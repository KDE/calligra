#ifndef KSPREAD_TABLE_IFACE_H
#define KSPREAD_TABLE_IFACE_H

#include <dcopobject.h>
#include <dcopref.h>

#include <qstring.h>
#include <qrect.h>
#include <qcolor.h>

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
    virtual QRect selection() const;
    virtual void setSelection( const QRect& selection );
    virtual QString name() const;
    virtual int maxColumn() const;
    virtual int maxRow() const;
    virtual bool setTableName( const QString & name);

    virtual void setSelectionMoneyFormat( bool b );
    virtual void setSelectionPrecision( int _delta );
    virtual void setSelectionPercent( bool b );
    virtual void setSelectionMultiRow( bool enable );
    virtual void setSelectionSize(int _size );
    virtual void setSelectionUpperLower( int _type );
    virtual void setSelectionfirstLetterUpper( );
    virtual void setSelectionVerticalText( bool _b);
    virtual void setSelectionComment( const QString &_comment);
    virtual void setSelectionRemoveComment();
    virtual void setSelectionAngle(int _value);
    virtual void setSelectionTextColor( const QColor &tbColor );
    virtual void setSelectionbgColor( const QColor &bg_Color );
    virtual void setSelectionBorderColor(const QColor &bd_Color );

private:
    KSpreadTable* m_table;
    KSpreadCellProxy* m_proxy;
};

#endif

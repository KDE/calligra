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

    virtual void deleteSelection();
    virtual void copySelection();
    virtual void cutSelection();
    virtual void clearTextSelection();
    virtual void clearValiditySelection();
    virtual void clearConditionalSelection();

    virtual bool insertColumn( int col,int nbCol );
    virtual bool insertRow( int row,int nbRow);
    virtual void removeColumn( int col,int nbCol );
    virtual void removeRow( int row,int nbRow );
    virtual void borderLeft(const QColor &_color );
    virtual void borderTop( const QColor &_color );
    virtual void borderOutline( const QColor &_color );
    virtual void borderAll( const QColor &_color );
    virtual void borderRemove( );
    virtual void borderBottom(const QColor &_color );
    virtual void borderRight( const QColor &_color );

    virtual bool isHidden()const;

    virtual void increaseIndent();
    virtual void decreaseIndent();

    virtual bool showGrid() const;
    virtual bool showFormula() const;
    virtual bool lcMode() const;
    virtual bool autoCalc() const;
    virtual bool showColumnNumber() const;
    virtual bool hideZero() const;
    virtual bool firstLetterUpper() const;


private:
    KSpreadTable* m_table;
    KSpreadCellProxy* m_proxy;
};

#endif

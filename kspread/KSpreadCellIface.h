#ifndef KSPREAD_CELL_IFACE_H
#define KSPREAD_CELL_IFACE_H

class QPoint;
class KSpreadTable;
class KSpreadCell;
#include <qcolor.h>
#include <dcopobject.h>

class KSpreadCellIface
{
    K_DCOP
public:
    KSpreadCellIface();

    void setCell( KSpreadTable* table, const QPoint& point );
k_dcop:
    virtual QString text() const;
    virtual void setText( const QString& text );
    virtual void setValue( int value );
    virtual void setValue( double value );
    virtual double value() const;
    virtual void setBgColor(const QString& _c);
    virtual void setBgColor(int r,int g,int b);
    virtual QString bgColor() const;
    virtual void setTextColor(const QString& _c);
    virtual void setTextColor(int r,int g,int b);
    virtual QString textColor()const ;
    virtual void setComment(const QString &_comment);
    virtual void setAngle(int angle);
    virtual int angle() const ; 

private:
    QPoint m_point;
    KSpreadTable* m_table;
};

#endif

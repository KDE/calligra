#ifndef KSPREAD_CELL_IFACE_H
#define KSPREAD_CELL_IFACE_H

class QPoint;
class KSpreadTable;
class KSpreadCell;

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
    
private:
    QPoint m_point;
    KSpreadTable* m_table;
};

#endif

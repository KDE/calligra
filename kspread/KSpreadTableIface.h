#ifndef KSPREAD_TABLE_IFACE_H
#define KSPREAD_TABLE_IFACE_H

#include <dcopobject.h>
#include <dcopref.h>

#include <qstring.h>
#include <qrect.h>

class KSpreadTable;

class KSpreadTableIface : virtual public DCOPObject
{
    K_DCOP
public:
    KSpreadTableIface( KSpreadTable* );

k_dcop:
    virtual DCOPRef cell( int x, int y );
    virtual DCOPRef cell( const QString& name );
    virtual QRect selection() const;
    virtual void setSelection( const QRect& selection );
    virtual QString name() const;
    
private:
    KSpreadTable* m_table;
};

#endif

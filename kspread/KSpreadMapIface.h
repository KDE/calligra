#ifndef KSPREAD_MAP_IFACE_H
#define KSPREAD_MAP_IFACE_H

#include <dcopobject.h>
#include <dcopref.h>

#include <qvaluelist.h>
#include <qstringlist.h>

class KSpreadMap;

class KSpreadMapIface : virtual public DCOPObject
{
    K_DCOP
public:
    KSpreadMapIface( KSpreadMap* );

k_dcop:
    virtual DCOPRef table( const QString& name );
    virtual DCOPRef table( int index );
    virtual int tableCount() const;
    virtual QStringList tableNames() const;
    virtual QValueList<DCOPRef> tables();

private:
    KSpreadMap* m_map;
};

#endif

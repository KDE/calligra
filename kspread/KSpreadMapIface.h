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

    virtual bool processDynamic(const QCString &fun, const QByteArray &data,
				QCString& replyType, QByteArray &replyData);

k_dcop:
    virtual DCOPRef table( const QString& name );
    virtual DCOPRef tableByIndex( int index );
    virtual int tableCount() const;
    virtual QStringList tableNames() const;
    virtual QValueList<DCOPRef> tables();
    virtual DCOPRef insertTable( const QString& name );

private:
    KSpreadMap* m_map;
};

#endif

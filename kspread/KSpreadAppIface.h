#ifndef KSPREAD_APP_IFACE_H
#define KSPREAD_APP_IFACE_H

#include <dcopobject.h>
#include <dcopref.h>

#include <qmap.h>
#include <qstring.h>

class KSpreadAppIface : public DCOPObject
{
    K_DCOP
public:
    KSpreadAppIface();

k_dcop:
    virtual DCOPRef createDoc();
    virtual DCOPRef createDoc( const QString& name );
    virtual QMap<QString,DCOPRef> documents();
    virtual DCOPRef document( const QString& name );
};

#endif

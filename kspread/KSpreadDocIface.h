#ifndef KSPREAD_DOC_IFACE_H
#define KSPREAD_DOC_IFACE_H

#include <dcopobject.h>
#include <dcopref.h>

#include <qstring.h>

class KSpreadDoc;

class KSpreadDocIface : virtual public DCOPObject
{
    K_DCOP
public:
    KSpreadDocIface( KSpreadDoc* );

k_dcop:
    virtual DCOPRef map();
    virtual QString url();
    virtual bool isModified();
    virtual bool save();
    virtual bool saveAs( const QString& url );

private:
    KSpreadDoc* m_doc;
};

#endif

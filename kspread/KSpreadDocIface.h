#ifndef KSPREAD_DOC_IFACE_H
#define KSPREAD_DOC_IFACE_H

#include <dcopref.h>
#include <KoDocumentIface.h>

#include <qstring.h>

class KSpreadDoc;

class KSpreadDocIface : virtual public KoDocumentIface
{
    K_DCOP
public:
    KSpreadDocIface( KSpreadDoc* );

k_dcop:
    virtual DCOPRef map();
    virtual bool save();
    virtual bool saveAs( const QString& url );
};

#endif

#ifndef KPRESENTER_DOC_IFACE_H
#define KPRESENTER_DOC_IFACE_H

#include <dcopobject.h>
#include <dcopref.h>

#include <qstring.h>

class KPresenterDoc;

class KPresenterDocIface : virtual public DCOPObject
{
    K_DCOP
public:
    KPresenterDocIface( KPresenterDoc *doc_ );

k_dcop:
    virtual QString url();
    virtual DCOPRef firstView();
    virtual DCOPRef nextView();
    virtual int getNumObjects();
    virtual DCOPRef getObject( int num );
    virtual int getNumPages();
    virtual DCOPRef getPage( int num );
    
private:
    KPresenterDoc *doc;

};

#endif

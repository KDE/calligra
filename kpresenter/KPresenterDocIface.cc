#include "KPresenterDocIface.h"

#include "kpresenter_doc.h"

#include <kapp.h>
#include <dcopclient.h>

KPresenterDocIface::KPresenterDocIface( KPresenterDoc *doc_ )
    : DCOPObject( doc_ )
{
   doc = doc_;
}

QString KPresenterDocIface::url()
{
    return doc->url();
}

DCOPRef KPresenterDocIface::firstView()
{
    return DCOPRef( kapp->dcopClient()->appId(), 
		    doc->getFirstView()->dcopObject()->objId() );
}

DCOPRef KPresenterDocIface::nextView()
{
    return DCOPRef( kapp->dcopClient()->appId(), 
		    doc->getNextView()->dcopObject()->objId() );
}

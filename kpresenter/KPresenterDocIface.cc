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

int KPresenterDocIface::getNumObjects()
{
    return doc->objNums();
}

DCOPRef KPresenterDocIface::getObject( int num )
{
    return DCOPRef( kapp->dcopClient()->appId(), 
		    doc->objectList()->at( num )->dcopObject()->objId() );
}

int KPresenterDocIface::getNumPages()
{
    return doc->getPageNums();
}

DCOPRef KPresenterDocIface::getPage( int num )
{
    return DCOPRef( kapp->dcopClient()->appId(), 
		    doc->backgroundList()->at( num )->dcopObject()->objId() );
}

int KPresenterDocIface::getPageOfObj( int obj )
{
    return doc->getPageOfObj( obj, 0, 0, 0 ) - 1;
}

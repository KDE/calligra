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


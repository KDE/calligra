#include "KSpreadDocIface.h"

#include "kspread_doc.h"
#include "kspread_map.h"

#include <kapp.h>
#include <dcopclient.h>

KSpreadDocIface::KSpreadDocIface( KSpreadDoc* doc )
    : DCOPObject( doc )
{
    m_doc = doc;
}

DCOPRef KSpreadDocIface::map()
{
    return DCOPRef( kapp->dcopClient()->appId(), m_doc->map()->dcopObject()->objId() );
}

QString KSpreadDocIface::url()
{
    return m_doc->url().url();
}

bool KSpreadDocIface::isModified()
{
    return m_doc->isModified();
}

bool KSpreadDocIface::save()
{
    // TODO
  return false;
}

bool KSpreadDocIface::saveAs( const QString& url )
{
    // TODO
  return false;
}



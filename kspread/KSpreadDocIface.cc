#include "KSpreadDocIface.h"
#include <KoDocumentIface.h>

#include "kspread_doc.h"
#include "kspread_map.h"

#include <kapp.h>
#include <dcopclient.h>

KSpreadDocIface::KSpreadDocIface( KSpreadDoc* doc )
    : KoDocumentIface( doc )
{
}

DCOPRef KSpreadDocIface::map()
{
    return DCOPRef( kapp->dcopClient()->appId(),
                    static_cast<KSpreadDoc*>(m_pDoc)->map()->dcopObject()->objId() );
}

bool KSpreadDocIface::save()
{
    // TODO
  return false;
}

bool KSpreadDocIface::saveAs( const QString& )
{
    // TODO
  return false;
}



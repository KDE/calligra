#include "KSpreadDocIface.h"

#include "kspread_doc.h"
#include "kspread_map.h"

KSpreadDocIface::KSpreadDocIface( KSpreadDoc* doc )
    : DCOPObject( doc )
{
    m_doc = doc;
}

DCOPRef KSpreadDocIface::map()
{
    return DCOPRef( m_doc->map()->dcopObject() );
}

QString KSpreadDocIface::url()
{
    return m_doc->url();
}

bool KSpreadDocIface::isModified()
{
    return m_doc->isModified();
}

bool KSpreadDocIface::save()
{
    // TODO
}

bool KSpreadDocIface::saveAs( const QString& url )
{
    // TODO
}



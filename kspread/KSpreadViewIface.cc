#include "KSpreadViewIface.h"

#include "kspread_view.h"
#include "kspread_table.h"
#include "kspread_doc.h"
#include "kspread_map.h"

#include <kapp.h>
#include <dcopclient.h>
#include <dcopref.h>

/************************************************
 *
 * KSpreadViewIface
 *
 ************************************************/

KSpreadViewIface::KSpreadViewIface( KSpreadView* t )
    : KoViewIface( t )
{
    m_view = t;
}

KSpreadViewIface::~KSpreadViewIface()
{
}

DCOPRef KSpreadViewIface::doc() const
{
    return DCOPRef( kapp->dcopClient()->appId(), m_view->doc()->dcopObject()->objId() );
}

DCOPRef KSpreadViewIface::map() const
{
    return DCOPRef( kapp->dcopClient()->appId(), m_view->doc()->map()->dcopObject()->objId() );
}

DCOPRef KSpreadViewIface::table() const
{
    return DCOPRef( kapp->dcopClient()->appId(), m_view->activeTable()->dcopObject()->objId() );
}

void KSpreadViewIface::hide()
{
    m_view->hide();
}

void KSpreadViewIface::show()
{
    m_view->show();
}

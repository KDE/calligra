#include "KSpreadViewIface.h"

#include "kspread_view.h"
#include "kspread_doc.h"
#include "kspread_map.h"

#include <kapplication.h>
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

void KSpreadViewIface::find()
{
    m_view->find();
}

void KSpreadViewIface::replace()
{
    m_view->replace();
}

void KSpreadViewIface::conditional()
{
    m_view->conditional();
}

void KSpreadViewIface::validity()
{
    m_view->validity();
}

void KSpreadViewIface::insertSeries()
{
    m_view->insertSeries();
}

void KSpreadViewIface::insertHyperlink()
{
    m_view->insertHyperlink();
}

void KSpreadViewIface::gotoCell()
{
    m_view->gotoCell();
}

void KSpreadViewIface::changeAngle()
{
    m_view->changeAngle();
}

void KSpreadViewIface::preference()
{
    m_view->preference();
}

void KSpreadViewIface::nextTable()
{
    m_view->nextTable();
}

void KSpreadViewIface::previousTable()
{
    m_view->previousTable();
}

void KSpreadViewIface::sortList()
{
    m_view->sortList();
}

void KSpreadViewIface::setAreaName()
{
    m_view->setAreaName();
}

void KSpreadViewIface::showAreaName()
{
    m_view->showAreaName();
}


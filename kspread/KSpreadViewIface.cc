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

void KSpreadViewIface::changeNbOfRecentFiles(int _nb)
{
    if(_nb<0)
        return;
    m_view->changeNbOfRecentFiles(_nb);
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

void KSpreadViewIface::mergeCell()
{
    m_view->mergeCell();
}

void KSpreadViewIface::dissociateCell()
{
    m_view->dissociateCell();
}

void KSpreadViewIface::consolidate()
{
    m_view->consolidate();
}

void KSpreadViewIface::deleteColumn()
{
    m_view->deleteColumn();
}

void KSpreadViewIface::insertColumn()
{
    m_view->insertColumn();
}

void KSpreadViewIface::deleteRow()
{
    m_view->deleteRow();
}

void KSpreadViewIface::insertRow()
{
    m_view->insertRow();
}

void KSpreadViewIface::hideRow()
{
    m_view->hideRow();
}

void KSpreadViewIface::showRow()
{
    m_view->showRow();
}

void KSpreadViewIface::hideColumn()
{
    m_view->hideColumn();
}

void KSpreadViewIface::showColumn()
{
    m_view->showColumn();
}

void KSpreadViewIface::upper()
{
    m_view->upper();
}

void KSpreadViewIface::lower()
{
    m_view->lower();
}

void KSpreadViewIface::equalizeColumn()
{
    m_view->equalizeColumn();
}

void KSpreadViewIface::equalizeRow()
{
    m_view->equalizeRow();
}

void KSpreadViewIface::clearTextSelection()
{
    m_view->clearTextSelection();
}

void KSpreadViewIface::clearCommentSelection()
{
    m_view->clearCommentSelection();
}

void KSpreadViewIface::clearValiditySelection()
{
    m_view->clearValiditySelection();
}

void KSpreadViewIface::clearConditionalSelection()
{
    m_view->clearConditionalSelection();
}

void KSpreadViewIface::goalSeek()
{
    m_view->goalSeek();
}

void KSpreadViewIface::insertFromDatabase()
{
    m_view->insertFromDatabase();
}

void KSpreadViewIface::insertFromTextfile()
{
    m_view->insertFromTextfile();
}

void KSpreadViewIface::insertFromClipboard()
{
    m_view->insertFromClipboard();
}

void KSpreadViewIface::textToColumns()
{
    m_view->textToColumns();
}

void KSpreadViewIface::copyAsText()
{
    m_view->copyAsText();
}

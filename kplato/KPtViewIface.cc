
#include "KPtViewIface.h"
#include "kptview.h"

#include <kapplication.h>
#include <dcopclient.h>
#include <dcopref.h>

/************************************************
 *
 * KPtViewIface
 *
 ************************************************/

KPtViewIface::KPtViewIface( KPTView* t )
    : KoViewIface( t )
{
    m_view = t;
}

KPtViewIface::~KPtViewIface()
{
}

void KPtViewIface::slotEditResource()
{
  m_view->slotEditResource();
}

void KPtViewIface::slotEditCut()
{
  m_view->slotEditCut();
}

void KPtViewIface::slotEditCopy()
{
  m_view->slotEditCopy();
}

void KPtViewIface::slotEditPaste()
{
  m_view->slotEditPaste();
}

void KPtViewIface::slotViewGantt()
{
  m_view->slotViewGantt();
}

void KPtViewIface::slotViewPert()
{
  m_view->slotViewPert();
}

void KPtViewIface::slotViewResources()
{
  m_view->slotViewResources();
}

void KPtViewIface::slotAddTask()
{
  m_view->slotAddTask();
}

void KPtViewIface::slotAddSubTask()
{
  m_view->slotAddSubTask();
}

void KPtViewIface::slotAddMilestone()
{
  m_view->slotAddMilestone();
}

void KPtViewIface::slotProjectEdit()
{
  m_view->slotProjectEdit();
}

void KPtViewIface::slotConfigure()
{
  m_view->slotConfigure();
}


#include "KPtViewIface.h"
#include "kptview.h"

#include <kapplication.h>
#include <dcopclient.h>
#include <dcopref.h>

namespace KPlato
{

/************************************************
 *
 * ViewIface
 *
 ************************************************/

ViewIface::ViewIface( View* t )
    : KoViewIface( t )
{
    m_view = t;
}

ViewIface::~ViewIface()
{
}

void ViewIface::slotEditResource()
{
  m_view->slotEditResource();
}

void ViewIface::slotEditCut()
{
  m_view->slotEditCut();
}

void ViewIface::slotEditCopy()
{
  m_view->slotEditCopy();
}

void ViewIface::slotEditPaste()
{
  m_view->slotEditPaste();
}

void ViewIface::slotViewGantt()
{
  m_view->slotViewGantt();
}

void ViewIface::slotViewPert()
{
  m_view->slotViewPert();
}

void ViewIface::slotViewResources()
{
  m_view->slotViewResources();
}

void ViewIface::slotAddTask()
{
  m_view->slotAddTask();
}

void ViewIface::slotAddSubTask()
{
  m_view->slotAddSubTask();
}

void ViewIface::slotAddMilestone()
{
  m_view->slotAddMilestone();
}

void ViewIface::slotProjectEdit()
{
  m_view->slotProjectEdit();
}

void ViewIface::slotConfigure()
{
  m_view->slotConfigure();
}

}  //KPlato namespace

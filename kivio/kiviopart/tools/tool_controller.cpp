/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000 theKompany.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include "tool_controller.h"
#include "tool.h"
#include "kivio_view.h"
#include "kivio_canvas.h"
#include "kivio_page.h"

#include <ktoolbarbutton.h>
#include <ktoolbar.h>
#include <kxmlguiclient.h>

ToolSelectAction::ToolSelectAction( QObject* parent, const char* name )
:KActionMenu("",parent,name)
{
  m_actSelf = false;
  m_init = false;
  m_def = 0L;
  m_count = 0;
}

void ToolSelectAction::insert( KAction* a, int index )
{
  m_count++;
  KActionMenu::insert(a,index);
  if (!m_init) {
    setDefaultAction(a);
    m_init = true;
  }
  connect(a,SIGNAL(activated()),SLOT(childActivated()));
}

void ToolSelectAction::remove( KAction* a )
{
  m_count--;
  KActionMenu::remove(a);
  a->disconnect(this,SIGNAL(activated()));
}

int ToolSelectAction::plug( QWidget* widget, int index )
{
  if ( widget->inherits("KToolBar") ) {
    KToolBar* bar = (KToolBar*)widget;
    int i = ( m_count == 1 ) ? KAction::plug(widget,index):KActionMenu::plug(widget,index);
    bar->setToggle(itemId(i),true);
    return i;
  }
  return -1;
}

void ToolSelectAction::setDefaultAction( KAction* a )
{
  KAction::setText(a->text());
  setAccel(a->accel());
  setGroup(a->group());
  setWhatsThis(a->whatsThis());
  setToolTip(a->toolTip());
  setStatusText(a->statusText());
  setEnabled(a->isEnabled());
  setIcon(a->icon());

  m_def = a;
}

void ToolSelectAction::slotActivated()
{
  emit activated();

  if (m_def) {
    m_actSelf = true;
    if ( m_def->inherits("KToggleAction") ) {
      KToggleAction* ta = (KToggleAction*)m_def;
      ta->setChecked(false);
      ta->activate();
      ta->setChecked(true);
    } else {
      m_def->activate();
    }
    m_actSelf = false;
  }
}

void ToolSelectAction::childActivated()
{
  setDefaultAction((KAction*)sender());
  if (!m_actSelf)
    activate();
}

void ToolSelectAction::setToggleState( bool state )
{
  int len = containerCount();
  for( int id = 0; id < len; ++id ) {
    KToolBar* w = (KToolBar*)container( id );
    KToolBarButton* b = w->getButton(itemId(id));
    b->on(state);
  }
}
/********************************************************************************/
ToolController::ToolController( KivioView* view )
: QObject(view)
{
  m_pDefaultTool = 0L;
  m_pActiveTool = 0L;
  m_pActiveView = 0L;
  m_bInit = false;
}

ToolController::~ToolController()
{
}

void ToolController::setDefaultTool( Tool* tool )
{
  m_pDefaultTool = tool;
}

void ToolController::registerTool( Tool* tool )
{
  tools.append(tool);
}

void ToolController::init()
{
  QWidget* tb = m_pActiveView->factory()->container("ToolsToolBar",m_pActiveView);
  tb->hide();

  tools.sort();
  QListIterator<Tool> it(tools);
  while ( it.current() ) {
    KAction* ta = it.current()->action();
    if ( ta && tb ) {
      ta->plug(tb);
      connect(ta,SIGNAL(activated()),SLOT(toolActivated()));
    }
    ++it;
  }
  tb->show();

  connect(m_pActiveView->factory(),SIGNAL(clientAdded(KXMLGUIClient*)),SLOT(activateToolGUI(KXMLGUIClient*)));
  connect(m_pActiveView->factory(),SIGNAL(clientRemoved(KXMLGUIClient*)),SLOT(deactivateToolGUI(KXMLGUIClient*)));
}

void ToolController::activateToolGUI( KXMLGUIClient* tool )
{
  KXMLGUIFactory* f = (KXMLGUIFactory*)sender();
  Tool* t = dynamic_cast<Tool*>(tool);
  if (t) {
    t->activateGUI(f);
  }
}

void ToolController::deactivateToolGUI( KXMLGUIClient* tool )
{
  KXMLGUIFactory* f = (KXMLGUIFactory*)sender();
  Tool* t = dynamic_cast<Tool*>(tool);
  if (t) {
    t->deactivateGUI(f);
  }
}

Tool* ToolController::getActiveTool()
{
  return m_pActiveTool;
}

void ToolController::delegateEvent( QEvent* e, KivioCanvas* canvas )
{
  if ( m_pActiveTool && m_pActiveTool->canvasWidget() == canvas )
    m_pActiveTool->processEvent(e);
}

void ToolController::selectTool( Tool* t )
{
  if ( m_pActiveTool == t || !t )
    return;

  KivioCanvas* canvas = m_pActiveView->canvasWidget();

  if (m_pActiveTool) {
    m_pActiveTool->deactivate();
    m_pActiveTool->setCanvasWidget(0L);
  }

  m_pActiveTool = t;

  QListIterator<Tool> it(tools);
  while ( it.current() ) {
    if (it.current()->action()){
      if ( it.current() != m_pActiveTool )
        it.current()->action()->setToggleState(false);
      else
        it.current()->action()->setToggleState(true);
    }
    ++it;
  }

  m_pActiveTool->setCanvasWidget(canvas);
  m_pActiveTool->activate();
}

void ToolController::activateView( KivioView* v )
{
  m_pActiveView = v;
  if (!m_bInit) {
    init();
    m_bInit = true;
  }

  Tool* def = m_pDefaultTool;
  if (m_pActiveTool) {
    // setup default tools in old canvas
    m_pActiveTool->deactivate();
    if (def) {
      def->setCanvasWidget(m_pActiveTool->canvasWidget());
      def->activate();
    }
    m_pActiveTool->setCanvasWidget(0L);

    def = m_pActiveTool;
    m_pActiveTool = 0L;
  }

  // activate action in new canvas
  if (def)
    if (def->action())
      def->action()->activate();
    else
      def->activate();
}

void ToolController::configureTool( Tool* t )
{
  if (t)
    t->configure();
}

void ToolController::toolActivated()
{
  ToolSelectAction* ta = (ToolSelectAction*)sender();
  ta->setToggleState(true);

  QListIterator<Tool> it(tools);
  while ( it.current() ) {
    if ( it.current()->action() == ta ) {
      selectTool(it.current());
      break;
    }
    ++it;
  }
}

void ToolController::activateDefault()
{
  m_pDefaultTool->action()->activate();
}

Tool* ToolController::findTool(const QString& name)
{
  QListIterator<Tool> it(tools);
  while ( it.current() ) {
    if ( QString(it.current()->name()) == name ) {
      return it.current();
    }
    ++it;
  }
  return 0;
}

#include "tool_controller.moc"

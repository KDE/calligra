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
#ifndef TOOL_CONTROLLER_H
#define TOOL_CONTROLLER_H

#include <qsortedlist.h>
#include <qevent.h>

#include <kparts/part.h>
#include <kaction.h>

class KivioPage;
class KivioCanvas;
class KivioView;

class Tool;

class ToolSelectAction : public KActionMenu
{ Q_OBJECT
public:
  ToolSelectAction( QObject* parent = 0, const char* name = 0 );
  ~ToolSelectAction(){};

  virtual void insert( KAction*, int index = -1 );
  virtual void remove( KAction* );

  virtual int plug( QWidget* widget, int index = -1 );

  void setDefaultAction( KAction* );
  KAction* defaultAction() { return m_def; }

  int count() { return m_count; }

public slots:
  virtual void slotActivated();
  void setToggleState( bool );

protected slots:
  void childActivated();

protected:
  bool m_init;
  KAction* m_def;
  int m_count;
  bool m_actSelf;
};
/********************************************************************************/
class ToolController : public QObject, public KParts::PartBase
{ Q_OBJECT
public:
  ToolController( KivioView* view );
  ~ToolController();
  
  void init();

  void registerTool( Tool* );
  void setDefaultTool( Tool* );

  Tool* getActiveTool();
  
  void delegateEvent( QEvent* e, KivioCanvas* canvas );

  void activateView( KivioView* );

  Tool* findTool(const QString&);

public slots:
  void activateDefault();
  void selectTool( Tool* );
  void configureTool( Tool* );

protected slots:
  void toolActivated();
  void activateToolGUI( KXMLGUIClient* );
  void deactivateToolGUI( KXMLGUIClient* );

private:
  QSortedList<Tool> tools;
  Tool* m_pDefaultTool;
  Tool* m_pActiveTool;
  KivioView* m_pActiveView;
  bool m_bInit;
};

#endif

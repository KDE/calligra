/*
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
#ifndef TOOLDOCKMANAGER_H
#define TOOLDOCKMANAGER_H

#include <qobject.h>
#include <qwidget.h>
#include <qlist.h>
#include <qtimer.h>

class ToolDockBase;

class ToolDockManager : public QObject
{ Q_OBJECT
public:
  ToolDockManager( QWidget* parent, const char* name = 0 );
  ~ToolDockManager();

  bool eventFilter( QObject*, QEvent* );
  ToolDockBase* createToolDock( QWidget* view, const QString& caption , const char* name = 0 );

protected:
  void addToolWindow( ToolDockBase* );

protected slots:
  void removeToolWindow();
  void slotTimeOut();
  void removeToolWindow( ToolDockBase* );

private:
  QTimer timer;
  QWidget* m_pView;
  QList<ToolDockBase> tools;
};
#endif


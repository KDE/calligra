/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2001 Igor Janssen (rm@linux.ru.net)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef __Tool_h__
#define __Tool_h__

#include <kaction.h>
#include <qobject.h>
#include <kxmlguiclient.h>

class ToolController;

class ToolSelectAction : public KActionMenu
{
  Q_OBJECT
public:
  ToolSelectAction(QObject *parent = 0, const char *name = 0);

  void insert(KAction *, int index = -1);
  void remove(KAction* );

  int plug(QWidget *widget, int index = -1);

  void setDefaultAction(KAction* );
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

class Tool : public QObject, public KXMLGUIClient
{
  Q_OBJECT
public:
  Tool(QString aId, ToolController *tc);
  virtual ~Tool();

  ToolController *toolController() const {return mToolController; }
  QString id() const {return mId; }

  virtual void activate() = 0;
  virtual void deactivate() = 0;
  virtual void processEvent(QEvent *e) = 0;
  
  ToolSelectAction *action();
  
private:
  ToolController *mToolController;
  QString mId;
};

#endif

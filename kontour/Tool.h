/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2001-2002 Igor Janssen (rm@linux.ru.net)

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

namespace Kontour {

class ToolController;

class ToolSelectAction : public KActionMenu
{
  Q_OBJECT
public:
  ToolSelectAction(QObject *parent = 0L, const char *name = 0L);

  void insert(KAction *a, int index = -1);
  void remove(KAction *a);

  int plug(QWidget *widget, int index = -1);

  void setDefaultAction(KAction *a);
  KAction* defaultAction() {return mDef; }

  int count() {return mCount; }

public slots:
  virtual void slotActivated();
  void setToggleState(bool state);

protected slots:
  void childActivated();

protected:
  bool        mInit;
  KAction    *mDef;
  int         mCount;
  bool        mActSelf;
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
};
using namespace Kontour;

#endif

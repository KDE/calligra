/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

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

#ifndef ToolGroup_h_
#define ToolGroup_h_

#include <qobject.h>
#include <qvector.h>

#include "ToolButton.h"

class ToolGroup : public QObject {
  Q_OBJECT

public:
  ToolGroup ();
  
  void insertButton (int id, ToolButton* button);

signals:
  void toolSelected (int id);
  void toolConfigActivated (int id);

private slots:
  void buttonPressed ();
  void rightMouseButtonPressed ();

private:
  QVector<ToolButton> buttons;
  unsigned int current_id;
};

#endif

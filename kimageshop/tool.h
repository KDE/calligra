/*
 *  tool.h - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter  <me@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __tool_h__
#define __tool_h__

#include <qobject.h>

class KImageShopDoc;
class KImageShopView;

class Tool : public QObject
{
  Q_OBJECT;

 public:
  Tool(KImageShopDoc *doc, KImageShopView *view = 0L);
  ~Tool();

  QString toolName();
  void optionsDialog();

 public slots:
  virtual void mousePress(QMouseEvent*); 
  virtual void mouseMove(QMouseEvent*);
  virtual void mouseRelease(QMouseEvent*);
    
 protected:
  KImageShopDoc  *m_pDoc;
  KImageShopView *m_pView;
};

#endif


/*
 *  kis_tool_brush.h - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter
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

#ifndef __brushtool_h__
#define __brushtool_h__

#include <qpoint.h>

#include "kis_tool.h"

class KisBrush;

class BrushTool : public KisTool
{
 public:
  BrushTool(KisDoc *doc, KisView *view, const KisBrush *_brush);
  ~BrushTool();
  
  QString toolName() { return QString("BrushTool"); }

  void setBrush(const KisBrush *_brush);
  bool paintMonochrome(QPoint pos);
  bool paintColor(QPoint pos);

 public slots:
  virtual void mousePress(QMouseEvent*); 
  virtual void mouseMove(QMouseEvent*);
  virtual void mouseRelease(QMouseEvent*);

 protected:
  QPoint 	m_dragStart;
  bool   	m_dragging;
  const KisBrush  	*m_pBrush;
  float         m_dragdist;
};

#endif //__brushtool_h__

/*
 *  gradienttool.h - part of KImageShop
 *
 *  Copyright (c) 1999 Michael Koch <koch@kde.org>
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

#ifndef __gradienttool_h__
#define __gradienttool_h__

#include <qpoint.h>

#include "kis_tool.h"

class KisDoc;
class KisCanvas;
class KisGradient;

class GradientTool : public KisTool
{
public:

  GradientTool( KisDoc* _doc, KisCanvas* _canvas, KisGradient* _gradient );
  ~GradientTool();

  virtual QString toolName() { return QString( "GradientTool" ); }

  virtual void mousePress( QMouseEvent* event );
  virtual void mouseMove( QMouseEvent* event );
  virtual void mouseRelease( QMouseEvent* event );

protected:

  void drawLine( const QPoint&, const QPoint& );

protected:

  QPoint       m_dragStart;
  QPoint       m_dragEnd;
  bool         m_dragging;
  KisCanvas   *m_pCanvas;
  KisGradient *m_gradient;
};

#endif //__gradienttool_h__

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

#include "tool.h"
#include "canvasview.h"

class KImageShopDoc;
class Gradient;

class GradientTool : public Tool
{
public:

  GradientTool( KImageShopDoc *_doc, KImageShopView *_view, CanvasView *_view, Gradient *_gradient );
  ~GradientTool();

  virtual QCString toolName() { return QCString("GradientTool"); }

  virtual void mousePress( const KImageShop::MouseEvent &_event );
  virtual void mouseMove( const KImageShop::MouseEvent &_event );
  virtual void mouseRelease( const KImageShop::MouseEvent &_event );


protected:
 
  KImageShopView *m_pView; 
  CanvasView     *m_pCanvasView;
  QWidget        *m_helpWidget;
  QPoint          m_dragStart;
  QPoint          m_pointEnd;
  bool            m_dragging;
  Gradient       *m_gradient;
};

#endif //__gradienttool_h__




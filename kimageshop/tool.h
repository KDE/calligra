/*
 *  tool.h - part of KImageShop
 *
 *  Copyright (c) 1999 The KImageShop team (see file AUTHORS)
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

#include "qmessagebox.h"
#include "kimageshop.h"

class Canvas;

class Tool : public KImageShop::Tool_skel
{
 public:
  Tool(Canvas *_canvas = 0L);
  ~Tool();

  virtual char* toolName();
  virtual void optionsDialog();

  // Perhaps we should make these KOM slots and let KImageShopView emit 
  // KOM signals but the mouse-move resolution might decrease?!
  virtual void mousePress(const KImageShop::MouseEvent& e) = 0; 
  virtual void mouseMove(const KImageShop::MouseEvent& e) = 0;
  virtual void mouseRelease(const KImageShop::MouseEvent& e) = 0;
    
 protected:
  Canvas  *m_pCanvas;
};

#endif


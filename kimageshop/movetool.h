/*
 *  movetool.h - part of KImageShop
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

#ifndef __movetool_h__
#define __movetool_h__

#include <qpoint.h>

#include "tool.h"
#include "kimageshop_undo.h"

class MoveCommand : public KImageShopCommand
{
public:

  MoveCommand( KImageShopDoc *_doc, int _layer, QPoint _oldpos, QPoint _newpos );

  virtual void execute();
  virtual void unexecute();

private:

  void moveTo( QPoint _pos );

  int m_layer;
  QPoint m_oldPos;
  QPoint m_newPos;
};

class MoveTool : public Tool
{
public:
  MoveTool( KImageShopDoc *doc );
  ~MoveTool();

  virtual char* toolName() { return CORBA::string_dup( "MoveTool" ); }

  virtual void mousePress( const KImageShop::MouseEvent& e ); 
  virtual void mouseMove( const KImageShop::MouseEvent& e );
  virtual void mouseRelease( const KImageShop::MouseEvent& e );

protected:
  QPoint m_dragStart;
  QPoint m_dragPosition;
  bool   m_dragging;
};

#endif //__movetool_h__

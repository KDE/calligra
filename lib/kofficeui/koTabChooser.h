/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
 
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
 
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
 
   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/     

// Description: Tabulator chooser (header)

/******************************************************************/

#ifndef koTabChooser_h
#define koTabChooser_h

#include <qframe.h>
#include <qevent.h>
#include <qpainter.h>
#include <qpen.h>
#include <qbrush.h>
#include <qcolor.h>
#include <qpopupmenu.h>
#include <qpoint.h>

#include <kapp.h>

/******************************************************************/
/* Class: KoTabChooser                                            */
/******************************************************************/

class KoTabChooser : public QFrame
{
  Q_OBJECT

public:
  static const int TAB_LEFT = 1;
  static const int TAB_CENTER = 2;
  static const int TAB_RIGHT = 4;
  static const int TAB_DEC_PNT = 8;
  static const int TAB_ALL = TAB_LEFT | TAB_CENTER | TAB_RIGHT | TAB_DEC_PNT;

  KoTabChooser(QWidget *parent,int _flags);

  int getCurrTabType() { return currType; }

protected:
  void mousePressEvent(QMouseEvent *e);
  void drawContents(QPainter *painter);
  void setupMenu();

  int flags;
  int currType;
  QPopupMenu *rb_menu;
  int mLeft,mRight,mCenter,mDecPoint;

protected slots:
  void rbLeft() { currType = TAB_LEFT; repaint(true); }
  void rbCenter() { currType = TAB_CENTER; repaint(true); }
  void rbRight() { currType = TAB_RIGHT; repaint(true); }
  void rbDecPoint() { currType = TAB_DEC_PNT; repaint(true); }
  
};

#endif

/******************************************************************/
/* KOffice Library - (c) by Reginald Stadlbauer 1998              */
/* Version: 1.0                                                   */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Ruler (header)                                         */
/******************************************************************/

#ifndef koRuler_h
#define koRuler_h

#include "koPageLayoutDia.h"

#include "kapp.h"

#include <qframe.h>
#include <qwidget.h>
#include <qpainter.h>
#include <qcolor.h>
#include <qpen.h>
#include <qbrush.h>
#include <qstring.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qrect.h>
#include <qevent.h>
#include <qcursor.h>
#include <qpixmap.h>

#define _MM_TO_POINT 2.83465
#define _POINT_TO_MM 0.3527772388    

/******************************************************************/
/* Class: KoRuler                                                 */
/******************************************************************/

class KoRuler : public QFrame
{
  Q_OBJECT

public:
  enum Orientation {HORIZONTAL,VERTICAL};

  static const int F_TABS = 1;
  static const int F_INDENTS = 2;

  KoRuler(QWidget *_parent,QWidget *_canvas,Orientation _orientation,
	  KoPageLayout _layout,int _flags);
  ~KoRuler();

  void setPageLayout(KoPageLayout _layout)
    { layout = _layout; repaint(false); }

  void showMousePos(bool _showMPos)
    { showMPos = _showMPos; hasToDelete = false; mposX = -1; mposY = -1; repaint(false); }
  void setMousePos(int mx,int my);

  void setOffset(int _diffx,int _diffy)
    { diffx = _diffx; diffy = _diffy; repaint(false); }

  void setLeftIndent(int _left)
    { i_left = _left; repaint(false); }
  void setFirstIndent(int _first)
    { i_first = _first; repaint(false); }

signals:
  void newPageLayout(KoPageLayout);
  void newLeftIndent(int);
  void newFirstIndent(int);
  void openPageLayoutDia();

protected:
  enum Action {A_NONE,A_BR_LEFT,A_BR_RIGHT,A_BR_TOP,A_BR_BOTTOM,A_LEFT_INDENT,A_FIRST_INDENT};

  void drawContents(QPainter *_painter)
    { if (orientation == HORIZONTAL) drawHorizontal(_painter); else drawVertical(_painter); }
  
  void drawHorizontal(QPainter *_painter);
  void drawVertical(QPainter *_painter);

  void mousePressEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseDoubleClickEvent(QMouseEvent*)
    { emit openPageLayoutDia(); }

  void resizeEvent(QResizeEvent *e);

  Orientation orientation;
  QWidget *canvas;
  KoPageLayout layout;
  int flags;
  int diffx,diffy;
  bool showMPos;
  int mposX,mposY;
  int i_left,i_first;

  int oldMx,oldMy;
  bool mousePressed;
  Action action;
  bool hasToDelete;
  QPixmap buffer;
  bool whileMovingBorderLeft,whileMovingBorderRight,whileMovingBorderTop,whileMovingBorderBottom;
  QPixmap pmFirst,pmLeft;
  
};

#endif
  

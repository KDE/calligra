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
/* Module: Tabulator chooser (header)                             */
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

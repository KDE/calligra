/******************************************************************/
/* KCharSelect - (c) by Reginald Stadlbauer 1998                  */
/* Version: 0.0.4                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* KCharSelect is under GNU GPL                                   */
/******************************************************************/
/* Module: Character Selector (header)                            */
/******************************************************************/

#ifndef KCHARSELECT_H
#define KCHARSELECT_H

#include <stdio.h>

#include <qwidget.h>
#include <krect.h>
#include <qcolor.h>
#include <qfont.h>
#include <ksize.h>
#include <qtablevw.h>
#include <qpainter.h>
#include <kpoint.h>
#include <qevent.h>

#include <kapp.h>

/******************************************************************/
/* Class: KCharSelect                                             */
/******************************************************************/

class KCharSelect : public QTableView
{
  Q_OBJECT

public:

  // constructor - destrcutor
  KCharSelect(QWidget*,const char*,QFont,int);
  ~KCharSelect() {}

  KSize sizeHint();

  void setFont(QFont __font)
    { _font = __font; repaint(true); }

  void setChar(int __c);

protected:

  // paint cell
  void paintCell(class QPainter*,int,int);

  // mouse events
  void mousePressEvent(QMouseEvent *e)
    { mousePressed = true; mouseMoveEvent(e); }
  void mouseReleaseEvent(QMouseEvent *e)
    { mouseMoveEvent(e); mousePressed = false; }
  void mouseMoveEvent(QMouseEvent*);

  // values
  QFont _font;
  int _c;
  KPoint currentC; 
  bool mousePressed;

signals:
  void activated(int);

};

#endif // KCHARSELECT

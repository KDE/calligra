/******************************************************************/
/* KCharSelect - (c) by Reginald Stadlbauer 1998                  */
/* Version: 0.0.2                                                 */
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
#include <qrect.h>
#include <qcolor.h>
#include <qfont.h>
#include <qsize.h>
#include <qtablevw.h>
#include <qpainter.h>
#include <qpoint.h>
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
  KCharSelect(QWidget*,const char*,QFont,QColor,int);
  ~KCharSelect() {}

  QSize sizeHint();

  void setFont(QFont __font)
    { _font = __font; repaint(true); }

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
  QColor _color;
  int _c;
  QPoint currentC; 
  bool mousePressed;

signals:
  void activated(int);

};

#endif // KCHARSELECT

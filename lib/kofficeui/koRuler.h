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
#include <qlist.h>
#include <qpopupmenu.h>
#include <qpoint.h>

#include "koTabChooser.h"

#define _MM_TO_POINT 2.83465
#define _INCH_TO_POINT 72.0

#define cPOINT_TO_MM(px) ((float)px/2.83465)
#define cMM_TO_POINT(mm) (int((float)mm*2.83465))
#define cPOINT_TO_INCH(px) ((float)px/72.0)
#define cINCH_TO_POINT(inch) (int((float)inch*72.0))
#define cMM_TO_INCH(mm) (mm/25.4)
#define cINCH_TO_MM(inch) (inch*25.4)

/******************************************************************/
/* Struct: KoTabulator                                            */
/******************************************************************/

enum KoTabulators {T_LEFT = 0,T_CENTER = 1,T_RIGHT = 2,T_DEC_PNT = 3};

struct KoTabulator
{
  int ptPos;
  float mmPos;
  float inchPos;
  KoTabulators type;
};

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
	  KoPageLayout _layout,int _flags,KoTabChooser *_tabChooser = 0L);
  ~KoRuler();

  void setUnit(QString _unit);

  void setPageLayout(KoPageLayout _layout)
    { layout = _layout; repaint(false); }

  void showMousePos(bool _showMPos)
    { showMPos = _showMPos; hasToDelete = false; mposX = -1; mposY = -1; repaint(false); }
  void setMousePos(int mx,int my);

  void setOffset(int _diffx,int _diffy)
    { diffx = _diffx; diffy = _diffy; repaint(false); }

  void setLeftIndent(float _left)
    { i_left = makeIntern(_left); repaint(false); }
  void setFirstIndent(float _first)
    { i_first = makeIntern(_first); repaint(false); }

  void setTabList(QList<KoTabulator>* _tabList);
  void setFrameStart(int _frameStart) { frameStart = _frameStart; repaint(false); }
  
  void setAllowUnits(bool _allow) { allowUnits = _allow; }

signals:
  void newPageLayout(KoPageLayout);
  void newLeftIndent(int);
  void newFirstIndent(int);
  void openPageLayoutDia();
  void tabListChanged(QList<KoTabulator>*);
  void unitChanged(QString);

protected:
  enum Action {A_NONE,A_BR_LEFT,A_BR_RIGHT,A_BR_TOP,A_BR_BOTTOM,A_LEFT_INDENT,A_FIRST_INDENT,A_TAB};

  void drawContents(QPainter *_painter)
    { if (orientation == HORIZONTAL) drawHorizontal(_painter); else drawVertical(_painter); }
  
  void drawHorizontal(QPainter *_painter);
  void drawVertical(QPainter *_painter);
  void drawTabs(QPainter &_painter);

  void mousePressEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseDoubleClickEvent(QMouseEvent*);
  void resizeEvent(QResizeEvent *e);

  unsigned int makeIntern(float _v);
  void setupMenu();
  void uncheckMenu();

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
  KoTabChooser *tabChooser;
  QList<KoTabulator> tabList;
  int currTab;
  int frameStart;
  bool allowUnits;
  QPopupMenu *rb_menu;
  int mMM,mPT,mINCH;

  QString unit;

protected slots:
  void rbPT() { setUnit("pt"); emit unitChanged("pt"); }
  void rbMM() { setUnit("mm"); emit unitChanged("mm"); }
  void rbINCH() { setUnit("inch"); emit unitChanged("inch"); }

};

#endif
  

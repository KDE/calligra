/******************************************************************/ 
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer, Torben Weis                       */
/* E-Mail: reggie@kde.org, weis@kde.org                           */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Paragraph Dialog (header)                              */
/******************************************************************/

#ifndef paragdia_h
#define paragdia_h

#include <qtabdialog.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qcombobox.h>
#include <qpen.h>
#include <qbrush.h>
#include <qpainter.h>
#include <qcolor.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qlineedit.h>
#include <qstrlist.h>

#include <kapp.h>
#include <krestrictedline.h>
#include <kbuttonbox.h>
#include <kcolorbtn.h>
#include <kspinbox.h>
#include "kcharselectdia.h"

#include <stdlib.h>

#include "defs.h"
#include "paraglayout.h"

/******************************************************************/
/* class KWPagePreview                                            */
/******************************************************************/

class KWPagePreview : public QGroupBox
{
  Q_OBJECT

public:
  KWPagePreview(QWidget*,const char*);
  ~KWPagePreview() {}
  
  void setLeft(float _left)
    { left = _left; repaint(false); }
  void setRight(float _right)
    { right = _right; repaint(false); }
  void setFirst(float _first)
    { first = _first; repaint(false); }
  void setSpacing(float _spacing)
    { spacing = _spacing; repaint(false); }
  void setBefore(float _before)
    { before = _before; repaint(false); }
  void setAfter(float _after)
    { after = _after; repaint(false); }

protected:
  void drawContents(QPainter*);

  float left,right,first,spacing,before,after;

};

/******************************************************************/
/* class KWPagePreview2                                           */
/******************************************************************/

class KWPagePreview2 : public QGroupBox
{
  Q_OBJECT

public:
  KWPagePreview2(QWidget*,const char*);
  ~KWPagePreview2() {}
  
  void setFlow(KWParagLayout::Flow _flow)
    { flow = _flow; repaint(false); }

protected:
  void drawContents(QPainter*);

  KWParagLayout::Flow flow;

};

/******************************************************************/
/* class KWBorderPreview                                          */
/******************************************************************/

class KWBorderPreview : public QGroupBox
{
  Q_OBJECT

public:
  KWBorderPreview(QWidget*,const char*);
  ~KWBorderPreview() {}

  KWParagLayout::Border getLeftBorder() { return leftBorder; }
  void setLeftBorder(KWParagLayout::Border _leftBorder) { leftBorder = _leftBorder; repaint(true); }
  KWParagLayout::Border getRightBorder() { return rightBorder; }
  void setRightBorder(KWParagLayout::Border _rightBorder) { rightBorder = _rightBorder; repaint(true); }
  KWParagLayout::Border getTopBorder() { return topBorder; }
  void setTopBorder(KWParagLayout::Border _topBorder) { topBorder = _topBorder; repaint(true); }
  KWParagLayout::Border getBottomBorder() { return bottomBorder; }
  void setBottomBorder(KWParagLayout::Border _bottomBorder) { bottomBorder = _bottomBorder; repaint(true); }
  
protected:
  void drawContents(QPainter*);
  QPen setBorderPen(KWParagLayout::Border _brd);

  KWParagLayout::Border leftBorder,rightBorder,topBorder,bottomBorder;

};

/******************************************************************/
/* class KWNumPreview                                             */
/******************************************************************/

class KWNumPreview : public QGroupBox
{
  Q_OBJECT

public:
  KWNumPreview(QWidget*,const char*);
  ~KWNumPreview() {}

  void setCounter(KWParagLayout::Counter _counter) { counter = _counter; repaint(true); }
  
protected:
  void drawContents(QPainter*);

  KWParagLayout::Counter counter;

};

/******************************************************************/
/* Class: KWParagDia                                              */
/******************************************************************/

class KWParagDia : public QTabDialog
{
  Q_OBJECT

public:
  static const int PD_SPACING = 1;
  static const int PD_FLOW = 2;
  static const int PD_BORDERS = 4;
  static const int PD_NUMBERING = 8;

  KWParagDia(QWidget*,const char*,QStrList _fontList,int _flags = PD_SPACING | PD_FLOW | PD_BORDERS | PD_NUMBERING);
  ~KWParagDia();              

  int getFlags() { return flags; }

  void setLeftIndent(float _left);
  void setFirstLineIndent(float _first);
  void setSpaceAfterParag(float _after);
  void setSpaceBeforeParag(float _before);
  void setLineSpacing(unsigned int _spacing);

  void setFlow(KWParagLayout::Flow _flow);

  float getLeftIndent()
    { return atof(eLeft->text()); } 
  float getFirstLineIndent()
    { return atof(eFirstLine->text()); } 
  float getSpaceBeforeParag()
    { return atof(eBefore->text()); } 
  float getSpaceAfterParag()
    { return atof(eAfter->text()); } 
  unsigned int getLineSpacing()
    { return atoi(eSpacing->text()); } 

  KWParagLayout::Flow getFlow();

  KWParagLayout::Border getLeftBorder() { return leftBorder; }
  void setLeftBorder(KWParagLayout::Border _leftBorder) { leftBorder = _leftBorder; updateBorders(); }
  KWParagLayout::Border getRightBorder() { return rightBorder; }
  void setRightBorder(KWParagLayout::Border _rightBorder) { rightBorder = _rightBorder; updateBorders(); }
  KWParagLayout::Border getTopBorder() { return topBorder; }
  void setTopBorder(KWParagLayout::Border _topBorder) { topBorder = _topBorder; updateBorders(); }
  KWParagLayout::Border getBottomBorder() { return bottomBorder; }
  void setBottomBorder(KWParagLayout::Border _bottomBorder) { bottomBorder = _bottomBorder; updateBorders(); }

  void setCounter(KWParagLayout::Counter _counter);
  KWParagLayout::Counter getCounter() { return counter; }

protected:
  void setupTab1();
  void setupTab2();
  void setupTab3();
  void setupTab4();
  void clearFlows();
  void updateBorders();

  QWidget *tab1,*tab2,*tab3,*tab4;
  QGridLayout *grid1,*grid2,*grid3,*grid4,*indentGrid,*spacingGrid,*pSpaceGrid,*tgrid,*txtgrid,*ogrid;
  KRestrictedLine *eLeft,*eRight,*eFirstLine,*eSpacing,*eBefore,*eAfter;
  QLabel *lLeft,*lRight,*lFirstLine,*lBefore,*lAfter,*lFlow,*lStyle,*lWidth,*lColor,*lDepth,*lcLeft,*lcRight,*lStart;
  QGroupBox *indentFrame,*spacingFrame,*pSpaceFrame,*gType,*gText,*gOther;
  QComboBox *cSpacing,*cStyle,*cWidth;
  QRadioButton *rLeft,*rCenter,*rRight,*rBlock,*rANums,*rLRNums,*rURNums,*rLAlph,*rUAlph,*rBullets,*rList,*rChapter,*rNone;
  KWPagePreview *prev1;
  KWPagePreview2 *prev2;
  KButtonBox *bb;
  QPushButton *bLeft,*bRight,*bTop,*bBottom,*bBullets,*bFont;
  KWBorderPreview *prev3;
  KColorButton *bColor;
  KNumericSpinBox *sDepth;
  QButtonGroup *g1,*g2;
  KWNumPreview *prev4;
  QLineEdit *ecLeft,*ecRight,*eStart;

  KWParagLayout::Border leftBorder,rightBorder,topBorder,bottomBorder;
  int flags;
  KWParagLayout::Counter counter;
  QStrList fontList;

protected slots:
  void leftChanged(const char*);
  void rightChanged(const char*);
  void firstChanged(const char*);
  void spacingActivated(int);
  void spacingChanged(const char*);
  void beforeChanged(const char*);
  void afterChanged(const char*);
  void flowLeft();
  void flowCenter();
  void flowRight();
  void flowBlock();
  void brdLeftToggled(bool);
  void brdRightToggled(bool);
  void brdTopToggled(bool);
  void brdBottomToggled(bool);
  void brdStyleChanged(const char*);
  void brdWidthChanged(const char*);
  void brdColorChanged(const QColor&);
  void changeBullet();
  void typeChanged(int);
  void numTypeChanged(int);
  void leftTextChanged(const char*);
  void rightTextChanged(const char*);
  void startChanged(const char*);
  void depthChanged();

};

#endif



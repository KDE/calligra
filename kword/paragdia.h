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

#include <kapp.h>
#include <krestrictedline.h>

#include <stdlib.h>

#include "defs.h"

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
/* Class: KWParagDia                                              */
/******************************************************************/

class KWParagDia : public QTabDialog
{
  Q_OBJECT

public:
  KWParagDia(QWidget*,const char*);
  ~KWParagDia();              

  void setLeftIndent(float _left);
  void setFirstLineIndent(float _first);
  void setSpaceAfterParag(float _after);
  void setSpaceBeforeParag(float _before);

  float getLeftIndent()
    { return atof(eLeft->text()); } 
  float getFirstLineIndent()
    { return atof(eFirstLine->text()); } 
  float getSpaceBeforeParag()
    { return atof(eBefore->text()); } 
  float getSpaceAfterParag()
    { return atof(eAfter->text()); } 

protected:
  void setupTab1();
  void setupTab2();

  QWidget *tab1,*tab2;
  QGridLayout *grid1,*indentGrid,*spacingGrid,*pSpaceGrid;
  KRestrictedLine *eLeft,*eRight,*eFirstLine,*eSpacing,*eBefore,*eAfter;
  QLabel *lLeft,*lRight,*lFirstLine,*lBefore,*lAfter;
  QGroupBox *indentFrame,*spacingFrame,*pSpaceFrame;
  QComboBox *cSpacing;
  KWPagePreview *prev1;

protected slots:
  void leftChanged(const char*);
  void rightChanged(const char*);
  void firstChanged(const char*);
  void spacingActivated(int);
  void spacingChanged(const char*);
  void beforeChanged(const char*);
  void afterChanged(const char*);

};

#endif



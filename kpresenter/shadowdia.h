/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Shadow Dialog (header)                                 */
/******************************************************************/

#ifndef SHADOWDIA_H
#define SHADOWDIA_H

#include <stdlib.h>

#include <qdialog.h>
#include <qlabel.h>
#include <qpushbt.h>
#include <qgrpbox.h>
#include <qframe.h>
#include <qcolor.h>
#include <qpainter.h>
#include <qpen.h>
#include <qfont.h>
#include <qstring.h>
#include <qfontmet.h>

#include <kspinbox.h>
#include <kapp.h>
#include <kcolorbtn.h>
#include <krect.h>

#include "global.h"

#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

/******************************************************************/
/* class ShadowPreview                                            */
/******************************************************************/

class ShadowPreview : public QFrame
{
  Q_OBJECT

public:
  // constructor - destructor
  ShadowPreview(QWidget* parent,const char*);
  ~ShadowPreview() {}                                            

  void setShadowDirection(ShadowDirection sd) {shadowDirection = sd; repaint(true);}
  void setShadowDistance(int sd) {shadowDistance = sd; repaint(true);}
  void setShadowColor(QColor sc) {shadowColor = sc; repaint(true);}

protected:
  void drawContents(QPainter*);

  ShadowDirection shadowDirection;
  int shadowDistance;
  QColor shadowColor;

};

/******************************************************************/
/* class ShadowDia                                                */
/******************************************************************/

class ShadowDia : public QDialog
{
  Q_OBJECT

public:
  // constructor - destructor
  ShadowDia(QWidget* parent,const char*);
  ~ShadowDia();                                             

  void setShadowDirection(ShadowDirection sd);
  void setShadowDistance(int sd);
  void setShadowColor(QColor sc);

  ShadowDirection getShadowDirection() {return shadowDirection;}
  int getShadowDistance() {return shadowDistance;}
  QColor getShadowColor() {return shadowColor;}

protected:
  KNumericSpinBox *distance; 
  QGroupBox *shadow,*preview;
  ShadowPreview *sPreview;
  QPushButton *lu,*u,*ru,*r,*rb,*b,*lb,*l;
  QPushButton *okBut,*applyBut,*cancelBut;
  KColorButton *color;
  QLabel *lcolor,*ldirection,*ldistance;

  ShadowDirection shadowDirection;
  int shadowDistance;
  QColor shadowColor;

protected slots:
  void luChanged();
  void uChanged();
  void ruChanged();
  void rChanged();
  void rbChanged();
  void bChanged();
  void lbChanged();
  void lChanged();
  void colorChanged(const QColor&);
  void distanceChanged();
  void Apply() {emit shadowDiaOk();}
  
signals:
  void shadowDiaOk();

};

#endif

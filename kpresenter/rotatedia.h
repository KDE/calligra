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
/* Module: Rotate Dialog (header)                                 */
/******************************************************************/

#ifndef ROTATEDIA_H
#define ROTATEDIA_H

#include <stdlib.h>

#include <qdialog.h>
#include <qlabel.h>
#include <qpushbt.h>
#include <qgrpbox.h>
#include <qradiobt.h>
#include <qframe.h>
#include <qcolor.h>
#include <qpainter.h>
#include <qpen.h>
#include <qfont.h>
#include <qfontmet.h>
#include <qwmatrix.h>
#include <kpoint.h>
#include <krect.h>

#include <krestrictedline.h>
#include <kapp.h>

#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

/******************************************************************/
/* class RotatePreview                                            */
/******************************************************************/

class RotatePreview : public QFrame
{
  Q_OBJECT

public:
  // constructor - destructor
  RotatePreview(QWidget* parent,const char*);
  ~RotatePreview() {}

  void setAngle(float __angle) {_angle = __angle; repaint(true);}

protected:
  void drawContents(QPainter*);

  float _angle;

};

/******************************************************************/
/* class RotateDia                                                */
/******************************************************************/

class RotateDia : public QDialog
{
  Q_OBJECT

public:
  // constructor - destructor
  RotateDia(QWidget* parent,const char*);
  ~RotateDia();

  void setAngle(float __angle);
  float getAngle() {return _angle;}

protected:
  QRadioButton *deg0,*deg90,*deg180,*deg270,*degCustom;
  KRestrictedLine *custom;
  QGroupBox *angle,*preview;
  RotatePreview *rPreview;
  QPushButton *okBut,*applyBut,*cancelBut;

  float _angle;

protected slots:
  void deg0clicked();
  void deg90clicked();
  void deg180clicked();
  void deg270clicked();
  void degCustomclicked();
  void degCustomChanged(const QString &);
  void Apply() {emit rotateDiaOk();}

signals:
  void rotateDiaOk();

};

#endif



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
/* Module: Spacing Dialog (header)                                */
/******************************************************************/

#ifndef spacingdia_h
#define spacingdia_h

#include <stdio.h>
#include <stdlib.h>

#include <qdialog.h>
#include <qlabel.h>
#include <qpushbt.h>
#include <qstring.h>

#include <krestrictedline.h>
#include <kapp.h>

#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

/******************************************************************/
/* class SpacingDia                                               */
/******************************************************************/

class SpacingDia : public QDialog
{
  Q_OBJECT

public:
  SpacingDia(QWidget* parent,int _lineSpacing,int _distBefore,int _distAfter);

protected:
  QLabel *lLineSpacing,*lDistBefore,*lDistAfter;
  KRestrictedLine *eLineSpacing,*eDistBefore,*eDistAfter;
  QPushButton *cancelBut,*okBut;

public slots:
  void slotSpacingDiaOk();

signals:
  void spacingDiaOk(int,int,int);

};

#endif



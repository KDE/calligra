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
/* Module: Effect Dialog (header)                                 */
/******************************************************************/

#ifndef EFFECTDIA_H
#define EFFECTDIA_H

#include <qdialog.h>

#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

class KPresenterView;
class QWidget;
class QComboBox;
class QLabel;
class KRestrictedLine;
class QPushButton;
class QCheckBox;
class QVBox;
class QResizeEvent;

/******************************************************************/
/* class EffectDia                                                */
/******************************************************************/

class EffectDia : public QDialog
{
    Q_OBJECT

public:

    // constructor - destructor
    EffectDia( QWidget* parent, const char*, int, int, KPresenterView* );

protected:
    void resizeEvent( QResizeEvent *e );
    
    QComboBox *cEffect, *cEffect2, *cDisappear;
    QLabel *lEffect, *lEffect2, *lNum, *lDisappear, *lDEffect;
    QSpinBox *eNum,*eDisappear;
    QPushButton *cancelBut, *okBut;
    QCheckBox *disappear;
    QVBox *back;
    
    int objNum, pageNum;
    KPresenterView *view;

public slots:
    void slotEffectDiaOk();
        
signals:
    void effectDiaOk();

protected slots:
    void disappearChanged();
    void num1Changed( int num );
    void num2Changed( int num );
    
};

#endif //EFFECTDIA_H



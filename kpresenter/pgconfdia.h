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
/* Module: Page Configuration Dialog (header)                     */
/******************************************************************/

#ifndef PGCONFDIA_H
#define PGCONFDIA_H

#include <qdialog.h>
#include <qmap.h>

#include "global.h"

class QButtonGroup;
class QLabel;
class QPushButton;
class QVBox;
class QResizeEvent;
class QListView;
class KPresenterDoc;
class QRadioButton;
class QComboBox;
class QCheckBox;

/******************************************************************/
/* class PgConfDia                                                */
/******************************************************************/

class PgConfDia : public QDialog
{
    Q_OBJECT

public:

    // constructor - destructor
    PgConfDia( QWidget* parent, KPresenterDoc *doc, const char* name,
               bool infLoop, bool swMan, int pgNum, 
               PageEffect pageEffect, PresSpeed presSpeed,
               PresentSlides presSlides, const QMap<int,bool> &selectedSlides );
    bool getInfinitLoop();
    bool getManualSwitch();
    PageEffect getPageEffect();
    PresSpeed getPresSpeed();
    PresentSlides getPresentSlides();
    QMap<int,bool> getSelectedSlides();

protected:
    void resizeEvent( QResizeEvent *e );
    
    QButtonGroup *general, *page, *slides;
    QCheckBox *infinitLoop, *manualSwitch;
    QRadioButton *slidesAll, *slidesCurrent, *slidesSelected;
    QLabel *label1, *label2, *label3, *label4;
    QPushButton *cancelBut, *okBut;
    QComboBox *effectCombo, *speedCombo;
    QVBox *back;
    QListView *lSlides;
    
public slots:
    void confDiaOk() { emit pgConfDiaOk(); }

protected slots:
    void presSlidesChanged( int );
    
signals:
    void pgConfDiaOk();

};

#endif 

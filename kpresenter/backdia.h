/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998		  */
/* Version: 0.1.0						  */
/* Author: Reginald Stadlbauer					  */
/* E-Mail: reggie@kde.org					  */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs			  */
/* needs c++ library Qt (http://www.troll.no)			  */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)	  */
/* needs OpenParts and Kom (weis@kde.org)			  */
/* written for KDE (http://www.kde.org)				  */
/* License: GNU GPL						  */
/******************************************************************/
/* Module: Background Dialog (header)				  */
/******************************************************************/

#ifndef BACKDIA_H
#define BACKDIA_H

#include <qdialog.h>
#include <qcolor.h>
#include <qstring.h>
#include <qframe.h>

#include "global.h"

class QLabel;
class QGroupBox;
class QComboBox;
class QPushButton;
class KColorButton;
class QButtonGroup;
class QSlider;
class KPBackGround;
class KPresenterDoc;
class QResizeEvent;
class QPainter;
class QRadioButton;
class QCheckBox;

/*******************************************************************
 *
 * Class: BackPreview
 *
 *******************************************************************/

class BackPreview : public QFrame
{
    Q_OBJECT
    
public:
    BackPreview( QWidget *parent, KPresenterDoc *doc );

    KPBackGround *backGround() const {
	return back;
    }
    
protected:
    void resizeEvent( QResizeEvent *e );
    void drawContents( QPainter *p );
    
private:
    KPBackGround *back;
    
};
    
/******************************************************************/
/* class BackDia						  */
/******************************************************************/
class BackDia : public QDialog
{
    Q_OBJECT

public:
    BackDia( QWidget* parent, const char* name,
	     BackType backType, QColor backColor1,
	     QColor backColor2, BCType _bcType,
	     QString backPic, QString backClip,
	     BackView backPicView, bool _unbalanced,
	     int _xfactor, int _yfactor, KPresenterDoc *doc );

    QColor getBackColor1();
    QColor getBackColor2();
    BCType getBackColorType();
    BackType getBackType();
    QString getBackPixFilename();
    QString getBackClipFilename();
    BackView getBackView();
    bool getBackUnbalanced();
    int getBackXFactor();
    int getBackYFactor();

private:
    QLabel *lPicName, *picPreview, *lClipName;
    QCheckBox *unbalanced;
    QComboBox *cType, *backCombo, *picView;
    QPushButton *okBut, *applyBut, *applyGlobalBut, *cancelBut;
    QPushButton *picChoose, *clipChoose;
    KColorButton *color1Choose, *color2Choose;
    QSlider *xfactor, *yfactor;
    QString chosenPic;
    QString chosenClip;
    BackPreview *preview;
    bool picChanged, clipChanged, lockUpdate;
    
private slots:
    void selectPic();
    void selectClip();
    void updateConfiguration();
    
    void Ok() { emit backOk( FALSE ); }
    void Apply() { emit backOk( FALSE ); }
    void ApplyGlobal() { emit backOk( TRUE ); }

signals:
    void backOk( bool );

};
#endif //BACKDIA_H

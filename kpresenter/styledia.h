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
/* Module: Pen and Brush style Dialog (header)			  */
/******************************************************************/

#ifndef STYLEDIA_H
#define STYLEDIA_H

#include <stdlib.h>

#include <qtabdialog.h>
#include <qlabel.h>
#include <qpushbt.h>
#include <qframe.h>
#include <qbttngrp.h>
#include <qcombo.h>
#include <qpen.h>
#include <qbrush.h>
#include <qpainter.h>
#include <qcolor.h>
#include <qsize.h>
#include <qradiobutton.h>
#include <qslider.h>
#include <qspinbox.h>
#include <qcheckbox.h>

#include <kcolordlg.h>
#include <kcolorbtn.h>

#include "global.h"
#include "kpresenter_utils.h"
#include "kpgradient.h"

#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif
#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

/******************************************************************/
/* class Pen and Brush preview					  */
/******************************************************************/

class PBPreview : public QWidget
{
    Q_OBJECT

public:

    // constructor
    PBPreview( QWidget* parent=0, const char* name=0, int _paintType=0 );

    // set values
    void setPen( QPen _pen ) {pen = _pen; repaint( FALSE ); }
    void setBrush( QBrush _brush ) {brush = _brush; repaint( FALSE ); }
    void setLineBegin( LineEnd lb ) {lineBegin = lb; repaint( FALSE ); }
    void setLineEnd( LineEnd le ) {lineEnd = le; repaint( FALSE ); }
    void setGradient( KPGradient *g ) {gradient = g; repaint( FALSE ); }

protected:

    // paint event
    void paintEvent( QPaintEvent* );

private:

    int paintType;
    QPen pen;
    QBrush brush;
    LineEnd lineBegin, lineEnd;
    KPGradient *gradient;

};

/******************************************************************/
/* class StyleDia						  */
/******************************************************************/

static const int SD_PEN = 1;
static const int SD_BRUSH = 2;

class StyleDia : public QTabDialog
{
    Q_OBJECT

public:

    // constructor - destructor
    StyleDia( QWidget* parent = 0, const char* name = 0, int flags = SD_PEN | SD_BRUSH );
    ~StyleDia();

    // set values
    void setPen( QPen _pen );
    void setBrush( QBrush _brush );
    void setLineBegin( LineEnd lb );
    void setLineEnd( LineEnd le );
    void setFillType( FillType ft );
    void setGradient( QColor _c1, QColor _c2, BCType _t, bool _unbalanced, int _xfactor, int _yfactor );

    // get values
    QPen getPen() { return pen; }
    QBrush getBrush() { return brush; }
    LineEnd getLineBegin() { return lineBegin; }
    LineEnd getLineEnd() { return lineEnd; }
    FillType getFillType() { if ( fillStyle->isChecked() ) return FT_BRUSH; return FT_GRADIENT; }
    QColor getGColor1() { return gradient1->color(); }
    QColor getGColor2() { return gradient2->color(); }
    BCType getGType() { return static_cast<BCType>( gradients->currentItem() + 1 ); }
    bool getGUnbalanced() { return unbalanced->isChecked(); }
    int getGXFactor() { return xfactor->value(); }
    int getGYFactor() { return yfactor->value(); }
    
private:

    // dialog objects
    QWidget *penFrame, *brushFrame;
    QPushButton *choosePCol, *chooseBCol;
    QLabel *penStyle, *brushStyle, *penWidth, *llineBegin, *llineEnd;
    QComboBox *choosePStyle, *chooseBStyle, *clineBegin, *clineEnd;
    QSpinBox *choosePWidth;
    QPushButton *okBut, *applyBut, *cancelBut;
    PBPreview *penPrev, *brushPrev, *gPrev;
    QRadioButton *fillStyle, *fillGradient;
    QCheckBox *unbalanced;
    QComboBox *gradients;
    KColorButton *gradient1, *gradient2;
    QLabel *gColors, *gStyle;
    QFrame *line;
    QSlider *xfactor, *yfactor;
    
    // pen and brush
    QBrush brush;
    QPen pen;
    LineEnd lineBegin, lineEnd;
    KPGradient *gradient;

private slots:

    // slots
    void changePCol();
    void changeBCol();
    void changePStyle( int item );
    void changeBStyle( int item );
    void changePWidth( int item );
    void changeLineBegin( int item );
    void changeLineEnd( int item );
    void gColor1( const QColor &newColor );
    void gColor2( const QColor &newColor );
    void gcStyle( int item );
    void rBrush();
    void rGradient();
    void rUnbalanced();
    void gXFactor( int v );
    void gYFactor( int v );
    void styleDone() {emit styleOk(); }

signals:

    void styleOk();

};
#endif //STYLEDIA_H




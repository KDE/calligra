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

#include "global.h"

#include <qtabdialog.h>
#include <qframe.h>
#include <qpen.h>
#include <qbrush.h>

class QPainter;
class KColorButton;
class QComboBox;
class QSpinBox;
class QCheckBox;
class QSlider;
class KPGradient;
class QResizeEvent;
class QWidgetStack;

/******************************************************************/
/* class Pen and Brush preview					  */
/******************************************************************/

class PBPreview : public QFrame
{
    Q_OBJECT

public:
    enum PaintType {
	Pen,
	Brush,
	Gradient
    };

    PBPreview( QWidget* parent, const char* name, PaintType _paintType );

    void setPen( QPen _pen ) { pen = _pen; repaint( TRUE ); }
    void setBrush( QBrush _brush ) { brush = _brush; }
    void setLineBegin( LineEnd lb ) { lineBegin = lb; repaint( TRUE ); }
    void setLineEnd( LineEnd le ) { lineEnd = le; repaint( TRUE ); }
    void setGradient( KPGradient *g ) { gradient = g; }
    void setPaintType( PaintType pt ) { paintType = pt; }
    
protected:
    void drawContents( QPainter *p );
    void resizeEvent( QResizeEvent *e );
    
private:
    PaintType paintType;
    QPen pen;
    QBrush brush;
    LineEnd lineBegin, lineEnd;
    KPGradient *gradient;

};

/******************************************************************/
/* class StyleDia						  */
/******************************************************************/

class StyleDia : public QTabDialog
{
    Q_OBJECT

public:
    enum PbType {
	SdPen = 1,
	SdBrush = 2,
	SdAll = SdPen | SdBrush
    };

    StyleDia( QWidget* parent = 0, const char* name = 0, int flags = SdAll );
    ~StyleDia();

    void setPen( const QPen &_pen );
    void setBrush( const QBrush &_brush );
    void setLineBegin( LineEnd lb );
    void setLineEnd( LineEnd le );
    void setFillType( FillType ft );
    void setGradient( const QColor &_c1, const QColor &_c2, BCType _t,
		      bool _unbalanced, int _xfactor, int _yfactor );

    QPen getPen();
    QBrush getBrush();
    LineEnd getLineBegin();
    LineEnd getLineEnd();
    FillType getFillType();
    QColor getGColor1();
    QColor getGColor2();
    BCType getGType();
    bool getGUnbalanced();
    int getGXFactor();
    int getGYFactor();

private:
    void setupTab1();
    void setupTab2();

    QWidgetStack *stack;
    KColorButton *choosePCol, *chooseBCol;
    QComboBox *choosePStyle, *chooseBStyle, *clineBegin, *clineEnd, *cFillType;
    QSpinBox *choosePWidth;
    PBPreview *penPrev, *brushPrev;
    QCheckBox *unbalanced;
    QComboBox *gradients;
    KColorButton *gradient1, *gradient2;
    QSlider *xfactor, *yfactor;
    KPGradient *gradient;
    bool lockUpdate;
    int flags;
    
private slots:
    void styleDone() { emit styleOk(); }
    void updatePenConfiguration();
    void updateBrushConfiguration();

signals:
    void styleOk();

};
#endif //STYLEDIA_H




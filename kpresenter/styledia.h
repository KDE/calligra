/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef STYLEDIA_H
#define STYLEDIA_H

#include <global.h>

#include <qtabdialog.h>
#include <qframe.h>
#include <qpen.h>

class QPainter;
class KColorButton;
class QComboBox;
class QSpinBox;
class QCheckBox;
class QSlider;
class KPGradient;
class QResizeEvent;
class QWidgetStack;
class QLabel;

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

    void setPen( QPen _pen ) { pen = _pen; repaint( true ); }
    void setBrush( QBrush _brush ) { brush = _brush; }
    void setLineBegin( LineEnd lb ) { lineBegin = lb; repaint( true ); }
    void setLineEnd( LineEnd le ) { lineEnd = le; repaint( true ); }
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
        SdGradient = 4,
        SdAll = SdPen | SdBrush | SdGradient
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
    void setSticky( bool s );

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
    bool isSticky();

private:
    void setupTab1();
    void setupTab2();
    void setupTab3();

    QWidgetStack *stack;
    KColorButton *choosePCol, *chooseBCol;
    QComboBox *choosePStyle, *chooseBStyle, *clineBegin, *clineEnd, *cFillType;
    QSpinBox *choosePWidth;
    PBPreview *penPrev, *brushPrev;
    QCheckBox *unbalanced, *sticky;
    QComboBox *gradients;
    KColorButton *gradient1, *gradient2;
    QSlider *xfactor, *yfactor;
    QLabel *xfactorLabel, *yfactorLabel;
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

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
#include <koRect.h>
class QPainter;
class KColorButton;
class KComboBox;
class KIntNumInput;
class QCheckBox;
class QSlider;
class KPGradient;
class QResizeEvent;
class QWidgetStack;
class QLabel;
class KoZoomHandler;
class QLineEdit;
class KPresenterDoc;


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

    void setPen( const QPen &_pen ) { pen = _pen; repaint( true ); }
    void setBrush( const QBrush &_brush ) { brush = _brush; }
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
    KoZoomHandler *_zoomHandler;
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
	SdEndBeginLine = 8,
        SdAll = SdPen | SdBrush | SdGradient | SdEndBeginLine
    };

    StyleDia( QWidget* parent = 0, const char* name = 0, KPresenterDoc *_doc = 0, int flags = SdAll, bool _noStickyObj = true, bool _oneObject=true );
    ~StyleDia();

    void setPen( const QPen &_pen );
    void setBrush( const QBrush &_brush );
    void setLineBegin( LineEnd lb );
    void setLineEnd( LineEnd le );
    void setFillType( FillType ft );
    void setGradient( const QColor &_c1, const QColor &_c2, BCType _t,
		      bool _unbalanced, int _xfactor, int _yfactor );
    void setSticky( bool s );

    QPen getPen()const;
    QBrush getBrush()const;
    LineEnd getLineBegin()const;
    LineEnd getLineEnd()const;
    FillType getFillType() const;
    QColor getGColor1()const;
    QColor getGColor2()const;
    BCType getGType()const;
    bool getGUnbalanced()const;
    int getGXFactor() const;
    int getGYFactor() const;
    bool isSticky()const;

    bool isOneObject() {return oneObject;}
    void setProtected( bool p );
    bool isProtected()const;

    KoRect getNewSize() const;
    void setSize(const KoRect &);

private:
    void setupTab1();
    void setupTab2();
    void setupTab3();
    void setupTab4();

    QWidgetStack *stack;
    KColorButton *choosePCol, *chooseBCol;
    KComboBox *choosePStyle, *chooseBStyle, *clineBegin, *clineEnd, *cFillType;
    KIntNumInput *choosePWidth;
    PBPreview *penPrev, *brushPrev;
    QCheckBox *unbalanced, *sticky, *protect;
    KComboBox *gradients;
    KColorButton *gradient1, *gradient2;
    QSlider *xfactor, *yfactor;
    QLabel *xfactorLabel, *yfactorLabel;
    KPGradient *gradient;
    bool lockUpdate, stickyObj;
    int flags;

    QLineEdit *m_lineTop, *m_lineLeft, *m_lineWidth, *m_lineHeight;
    KPresenterDoc *m_doc;

    QPen oldPen;
    QBrush oldBrush;
    LineEnd oldLb;
    LineEnd oldLe;
    FillType oldFillType;
    QColor oldC1, oldC2;
    BCType oldBCType;
    bool oldUnbalanced;
    bool oneObject;
    bool oldSticky,  oldProtect;
    int oldXfactor, oldYfactor;
    KoRect oldRect;

private slots:
    void slotReset();
    void styleDone() { emit styleOk(); }
    void updatePenConfiguration();
    void updateBrushConfiguration();
    void protectChanged();

signals:
    void styleOk();

};

#endif //STYLEDIA_H

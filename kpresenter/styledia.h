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
class ConfPieDia;
class ConfRectDia;
class ConfPolygonDia;
class ConfPictureDia;
class KPrCanvas;
class KPresenterDoc;
class KDoubleNumInput;

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
/* class ConfPenDia                                               */
/******************************************************************/

class ConfPenDia : public QWidget
{
    Q_OBJECT

public:
    ConfPenDia( QWidget* parent, const char* name, int flags);
    ~ConfPenDia();

    void setPen( const QPen &_pen );
    void setLineBegin( LineEnd lb );
    void setLineEnd( LineEnd le );

    QPen getPen()const;
    LineEnd getLineBegin()const;
    LineEnd getLineEnd()const;

private:
    int m_flags;
    QPen oldPen;
    LineEnd oldLb;
    LineEnd oldLe;
    KColorButton *choosePCol;
    KIntNumInput *choosePWidth;
    PBPreview *penPrev;
    KComboBox *choosePStyle, *clineBegin, *clineEnd;

private slots:
    void slotReset();
    void updatePenConfiguration();
};

/******************************************************************/
/* class ConfBrushDia                                             */
/******************************************************************/

class ConfBrushDia : public QWidget
{
    Q_OBJECT

public:
    ConfBrushDia( QWidget* parent, const char* name, int flags);
    ~ConfBrushDia();

    void setBrush( const QBrush &_brush );
    void setFillType( FillType ft );
    void setGradient( const QColor &_c1, const QColor &_c2, BCType _t,
                      bool _unbalanced, int _xfactor, int _yfactor );

    QBrush getBrush()const;
    FillType getFillType() const;
    QColor getGColor1()const;
    QColor getGColor2()const;
    BCType getGType()const;
    bool getGUnbalanced()const;
    int getGXFactor() const;
    int getGYFactor() const;

private:
    bool oldUnbalanced;
    int m_flags, oldXfactor, oldYfactor;
    QCheckBox *unbalanced;
    QBrush oldBrush;
    FillType oldFillType;
    QColor oldC1, oldC2;
    BCType oldBCType;
    KComboBox *gradients, *chooseBStyle, *cFillType;
    KColorButton *gradient1, *gradient2;
    QSlider *xfactor, *yfactor;
    QLabel *xfactorLabel, *yfactorLabel;
    KPGradient *gradient;
    QWidgetStack *stack;
    KColorButton *chooseBCol;
    PBPreview *brushPrev;

private slots:
    void slotReset();
    void updateBrushConfiguration();
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
        SdPie = 16,
        SdPicture = 32,
        SdPolygon = 64,
        SdRectangle = 128,
        SdOther = 256,
        SdAll = SdPen | SdBrush | SdGradient | SdEndBeginLine
    };

    StyleDia( QWidget* parent = 0, const char* name = 0, KPresenterDoc *_doc = 0,
              bool _noStickyObj = true, bool _oneObject=true );
    ~StyleDia();

    void setSticky( bool s );
    bool isSticky()const;

    bool isOneObject() {return oneObject;}
    void setProtected( bool p );
    bool isProtected()const;

    void setKeepRatio( bool p );
    bool isKeepRatio()const;

    KoRect getNewSize() const;
    void setSize(const KoRect &);

    ConfPenDia* getConfPenDia() { return m_confPenDia; }
    ConfPieDia* getConfPieDia() { return m_confPieDia; }
    ConfBrushDia* getConfBrushDia() { return m_confBrushDia; }
    ConfRectDia* getConfRectangleDia() { return m_confRectDia; }
    ConfPolygonDia* getConfPolygonDia() { return m_confPolygonDia; }
    ConfPictureDia* getConfPictureDia() { return m_confPictureDia; }

private:
    void setupTabPen();
    void setupTabBrush();
    void setupTabGeneral();
    void setupTabGeometry();
    void setupTabPie();
    void setupTabPolygon();
    void setupTabPicture();
    void setupTabRectangle();

    KoRect oldRect;
    QCheckBox *sticky, *protect, *keepRatio;
    KDoubleNumInput *m_lineTop, *m_lineLeft, *m_lineWidth, *m_lineHeight;

    KPresenterDoc *m_doc;
    KPrCanvas *m_canvas;
    ConfPenDia *m_confPenDia;
    ConfPieDia *m_confPieDia;
    ConfRectDia *m_confRectDia;
    ConfBrushDia *m_confBrushDia;
    ConfPolygonDia *m_confPolygonDia;
    ConfPictureDia *m_confPictureDia;

    int flags;
    bool lockUpdate, stickyObj, oneObject;
    bool oldSticky,  oldProtect, oldKeepRatio;

private slots:
    void slotReset();
    void styleDone() { emit styleOk(); }
    void protectChanged();

signals:
    void styleOk();
};

#endif //STYLEDIA_H

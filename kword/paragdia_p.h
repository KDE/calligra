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

#ifndef paragdia_p_h
#define paragdia_p_h

// This file hides those definitions from "users" of paragdia.h
// to reduce compile-time dependencies.

#include <qgroupbox.h>
#include <counter.h>
class QWidget;
class QPainter;

/******************************************************************/
/* class KWPagePreview                                            */
/******************************************************************/
class KWPagePreview : public QGroupBox
{
    Q_OBJECT

public:
    KWPagePreview( QWidget*, const char* = 0L );
    ~KWPagePreview() {}

    void setLeft( float _left )
    { left = _left; repaint( false ); }
    void setRight( float _right )
    { right = _right; repaint( false ); }
    void setFirst( float _first )
    { first = _first; repaint( false ); }
    void setSpacing( float _spacing )
    { spacing = _spacing; repaint( false ); }
    void setBefore( float _before )
    { before = _before; repaint( false ); }
    void setAfter( float _after )
    { after = _after; repaint( false ); }

protected:
    void drawContents( QPainter* );

    float left, right, first, spacing, before, after;

};

/******************************************************************/
/* class KWPagePreview2                                           */
/******************************************************************/

class KWPagePreview2 : public QGroupBox
{
    Q_OBJECT

public:
    KWPagePreview2( QWidget*, const char* = 0L );
    ~KWPagePreview2() {}

    void setAlign( int _align )
    { align = _align; repaint( false ); }

protected:
    void drawContents( QPainter* );

    int align;

};

/******************************************************************/
/* class KWBorderPreview                                          */
/******************************************************************/

class KWBorderPreview : public QFrame/*QGroupBox*/
{
    Q_OBJECT

public:
    KWBorderPreview( QWidget*, const char* = 0L );
    ~KWBorderPreview() {}

    Border getLeftBorder() { return leftBorder; }
    void setLeftBorder( Border _leftBorder ) { leftBorder = _leftBorder; repaint( true ); }
    Border getRightBorder() { return rightBorder; }
    void setRightBorder( Border _rightBorder ) { rightBorder = _rightBorder; repaint( true ); }
    Border getTopBorder() { return topBorder; }
    void setTopBorder( Border _topBorder ) { topBorder = _topBorder; repaint( true ); }
    Border getBottomBorder() { return bottomBorder; }
    void setBottomBorder( Border _bottomBorder ) { bottomBorder = _bottomBorder; repaint( true ); }

protected:
    virtual void mousePressEvent( QMouseEvent* _ev );
    void drawContents( QPainter* );
    QPen setBorderPen( Border _brd );

    Border leftBorder, rightBorder, topBorder, bottomBorder;
signals:
    void choosearea(QMouseEvent * _ev);

};

/******************************************************************/
/* class KWNumPreview                                             */
/******************************************************************/

class KWNumPreview : public QGroupBox
{
    Q_OBJECT

public:
    KWNumPreview( QWidget*, const char* = 0L );
    ~KWNumPreview() {}

    void setCounter( Counter _counter ) { counter = _counter; repaint( true ); }

protected:
    void drawContents( QPainter* );

    Counter counter;

};

#endif

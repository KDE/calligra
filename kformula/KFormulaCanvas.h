/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
		 2006 Martin Pfeiffer <hubipete@gmx.net>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KFORMULACANVAS_H
#define KFORMULACANVAS_H

#include <QWidget>
#include <QPixmap>

class QMouseEvent;
class QFocusEvent;
class QKeyEvent;
class QPaintEvent;
/* not yet written but will come
namespace KFormula {
class FormulaShape;
}*/

/**
 * @short The canvas widget displaying the @ref FormulaShape
 * 
 * The KFormulaCanvas is a class derived from QWidget to display a @ref FormulaShape
 * and to direct all user inputs to the shape. KFormulaCanvas renders the formula
 * in its paintEvent method.
 */
class KFormulaCanvas : public QWidget
{
public:
    KFormulaCanvas( QWidget* parent = 0, Qt::WFlags f=0 );
    ~KFormulaCanvas();

//    void setFormulaShape( FormulaShape* shape );
//    FormulaShape* formulaShape() const;
    
protected:
    virtual void mousePressEvent( QMouseEvent* event );
    virtual void mouseReleaseEvent( QMouseEvent* event );
    virtual void mouseDoubleClickEvent( QMouseEvent* event );
    virtual void mouseMoveEvent( QMouseEvent* event );

    virtual void paintEvent( QPaintEvent* event );
    virtual void keyPressEvent( QKeyEvent* event );
    virtual void focusInEvent( QFocusEvent* event );
    virtual void focusOutEvent( QFocusEvent* event );

private:
    /// The @ref FormulaShape shown at the moment
//    FormulaShape* m_formulaShape;
   
    /// The buffer for painting - always updated when the formula changes 
    QPixmap m_paintBuffer;

    /// True when a formula repaint is needed
    bool m_dirtyBuffer;
};

#endif // KFORMULACANVAS_H

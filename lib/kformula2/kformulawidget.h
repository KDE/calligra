/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

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

#ifndef __KFORMULAWIDGET_H
#define __KFORMULAWIDGET_H

#include <qdom.h>
#include <qwidget.h>

class FormulaCursor;
class KFormulaContainer;


class KFormulaWidget : public QWidget {
    Q_OBJECT

public:
    KFormulaWidget(KFormulaContainer*, QWidget* parent=0, const char* name=0, WFlags f=0);
    ~KFormulaWidget();

protected:

    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseDoubleClickEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent* event);

    virtual void paintEvent(QPaintEvent* event);
    virtual void keyPressEvent(QKeyEvent* event);

protected slots:

    void formulaChanged();
    
private:

    int movementFlag(int state);

    KFormulaContainer* document;
    FormulaCursor* cursor;

    /**
     * This isn't meant to last.
     */
    QDomDocument clipboard;
};

#endif // __KFORMULAWIDGET_H

/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __kformula_view_h__
#define __kformula_view_h__

class KAction;
class KFormulaDoc;
class KFormulaPartView;
class KFormulaWidget;
class KSelectAction;
class QPaintEvent;
class QScrollView;

#include <koView.h>
#include <qlist.h>


/**
 * The parts view.
 */
class KFormulaPartView : public KoView
{
    Q_OBJECT

public:
    KFormulaPartView(KFormulaDoc* _doc, QWidget* _parent=0, const char* _name=0);
    virtual ~KFormulaPartView();

    virtual void setupPrinter( KPrinter &printer );
    virtual void print( KPrinter &printer );

protected:

    virtual void resizeEvent(QResizeEvent* _ev);
    virtual void focusInEvent(QFocusEvent*);

    virtual void updateReadWrite(bool);

protected slots:

    void cursorChanged(bool visible, bool selecting);

private:

    /**
     * Enables the actions that modify the formula.
     */
    void setEnabled(bool enabled);

    KFormulaDoc *m_pDoc;
    KFormulaWidget* formulaWidget;
    QScrollView* scrollview;

    KAction* cutAction;
    KAction* copyAction;
    KAction* pasteAction;

    KAction* addBracketAction;
    KAction* addFractionAction;
    KAction* addRootAction;
    KAction* addSumAction;
    KAction* addProductAction;
    KAction* addIntegralAction;
    KAction* addMatrixAction;
    KAction* addUpperLeftAction;
    KAction* addLowerLeftAction;
    KAction* addUpperRightAction;
    KAction* addLowerRightAction;
    KAction* addGenericUpperAction;
    KAction* addGenericLowerAction;
    KAction* removeEnclosingAction;
};

#endif

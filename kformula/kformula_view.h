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

class FormulaCursor;
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
    
public slots:

    // Document signals
    void modifyMatrix(QString str);

    void reduce();
    void enlarge();
    void fractionAlignU();
    void fractionAlignD();
    void fractionAlignM();
    void fractionAlignL();
    void fractionAlignR();
    void fractionAlignC();
    void fractionDist();
    void fractionDistMore();
    void fractionDistLess();
    void fontSwitch();
    void indexList();
    void sizeSelected(int);
    void fontSelected(const QString& );
    void bold(bool);
    void underline(bool);
    void italic(bool);
    void modeSelected(char*);
    void textFont();
    void textSplit();
    void togglePixmap();
    void integralLower();
    void integralHigher();
    void bracketType();
    void matrixSet();
    void matrixRemRow();
    void matrixRemCol();
    void matrixInsRow();
    void matrixInsCol();
    void fractionHAlign();
    void toggleMidline();
    void symbolType();
    void DecorationType();
    void remove();
    void elementColor();
    void generalColor();
    void generalFont();
    
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
    
    QPopupMenu *mn_indexList;

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

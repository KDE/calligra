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
class KFormulaView;
class KFormulaWidget;
class QPaintEvent;
class QScrollView;

#include <koView.h>
#include <qlist.h>


/**
 * The parts view.
 */
class KFormulaView : public KoView
{
    Q_OBJECT

public:
    KFormulaView(KFormulaDoc* _doc, QWidget* _parent=0, const char* _name=0);
    virtual ~KFormulaView();

    virtual bool printDlg();
    
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
    void delimiterLeft();
    void delimiterRight();
    
protected:

    virtual void init();
    virtual void cleanUp();

    virtual void resizeEvent(QResizeEvent* _ev);
    
    virtual void updateReadWrite( bool );

private:
    
    KFormulaDoc *m_pDoc;
    KFormulaWidget* formulaWidget;
    QScrollView* scrollview;
    
    QPopupMenu *mn_indexList;

    KAction *actionEditUndo;
    KAction *actionEditRedo;
    KAction *m_idButtonFormula_0;
    KAction *actionView_NewView;
    KAction *actionView_FontToolbar;
    KAction *actionView_TypeToolbar;
    KAction *actionView_TextToolbar;

    KAction *actionElement_AddIndex_TL;
    KAction *actionElement_AddIndex_BL;
    KAction *actionElement_AddIndex_TR;
    KAction *actionElement_AddIndex_BR;
    KAction *actionElement_AddElement_T;  // text
    KAction *actionElement_AddElement_R;  // root
    KAction *actionElement_AddElement_F;  // fraction
    KAction *actionElement_AddElement_V;  // vertSpace
    KAction *actionElement_AddElement_B;  // bracket
    KAction *actionElement_AddElement_I;  // integral
    KAction *actionElement_AddElement_D;  // decoration
    KAction *actionElement_AddElement_S;  // symbol
    KAction *actionElement_AddElement_M;  // matrix
    KAction *actionElement_AddElement_L;  // index list

    KAction *actionElement_Text_Font;
    KAction *actionElement_Text_Size;
    KAction *actionElement_Text_Bold;
    KAction *actionElement_Text_Italic;
    KAction *actionElement_Text_Under;
    KAction *actionElement_Font_Child;
    KAction *actionElement_Font_Next;
    KAction *actionElement_Font_Element;
    KAction *actionElement_Font_Index;

    KAction *actionElement_Text_Split;
    KAction *actionElement_Root_Pixmap;
    KAction *actionElement_Root_Index;
    KAction *actionElement_Integral_Pixmap;
    KAction *actionElement_Integral_Lower;
    KAction *actionElement_Integral_Higher;
    KAction *actionElement_Bracket_Type_Left;
    KAction *actionElement_Bracket_Type_Right;
    KAction *actionElement_Matrix_Set;
    KAction *actionElement_Matrix_Ins_Row;
    KAction *actionElement_Matrix_Ins_Col;
    KAction *actionElement_Matrix_Rem_Row;
    KAction *actionElement_Matrix_Rem_Col;
    KAction *actionElement_Fraction_VA;  //Vert Allign
    KAction *actionElement_Fraction_VA_U;
    KAction *actionElement_Fraction_VA_D;
    KAction *actionElement_Fraction_VA_M;
    KAction *actionElement_Fraction_HA;  //Horiz Allign
    KAction *actionElement_Fraction_HA_C;
    KAction *actionElement_Fraction_HA_L;
    KAction *actionElement_Fraction_HA_R;
    KAction *actionElement_Fraction_Near;
    KAction *actionElement_Fraction_Far;
    KAction *actionElement_Fraction_Dist;
    KAction *actionElement_Fraction_MidLine;
    KAction *actionElement_Decoration_Set;
    KAction *actionElement_Symbol_Set;
    KAction *actionElement_Color;
    KAction *actionElement_Remove;
    KAction *actionFormula_Color;
    KAction *actionFormula_Font;
//    KAction *actionHelp_Using;
};

#endif

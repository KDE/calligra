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

// has to be included first
#include <koprinter.h>

#include <qpainter.h>
#include <qpopupmenu.h>

#include <kaction.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kstdaction.h>
//#include <kglobal.h>

#include <kformulacontainer.h>
#include <kformuladocument.h>
#include <kformulawidget.h>

#include "kformula_view.h"
#include "kformula_factory.h"
#include "kformula_doc.h"
#include "matrixwidget.h"


KFormulaPartView::KFormulaPartView(KFormulaDoc* _doc, QWidget* _parent, const char* _name)
        : KoView( _doc, _parent, _name ), m_pDoc(_doc)
{
//    setWidget( this );
//    widget()->setFocusPolicy( QWidget::StrongFocus );

//    setBackgroundColor( white );

    setInstance(KFormulaFactory::global());
    setXMLFile("kformula.rc");

    scrollview = new QScrollView(this, "scrollview");
    formulaWidget = new KFormulaWidget(_doc->getFormula(), scrollview->viewport(), "formulaWidget");
    scrollview->addChild(formulaWidget);
    formulaWidget->setFocus();

    // Nice parts start in read only mode.
    formulaWidget->setReadOnly(true);

    //KFormulaContainer* formula = m_pDoc->getFormula();
    KFormulaDocument* document = m_pDoc->getDocument();

    // copy&paste
    cutAction   = KStdAction::cut(document, SLOT(cut()), actionCollection());
    copyAction  = KStdAction::copy(document, SLOT(copy()), actionCollection());
    pasteAction = KStdAction::paste(document, SLOT(paste()), actionCollection());
    cutAction->setEnabled(false);
    copyAction->setEnabled(false);

    // elements
    addBracketAction      = document->getAddBracketAction();
    addFractionAction     = document->getAddFractionAction();
    addRootAction         = document->getAddRootAction();
    addSumAction          = document->getAddSumAction();
    addProductAction      = document->getAddProductAction();
    addIntegralAction     = document->getAddIntegralAction();
    addMatrixAction       = document->getAddMatrixAction();
    addUpperLeftAction    = document->getAddUpperLeftAction();
    addLowerLeftAction    = document->getAddLowerLeftAction();
    addUpperRightAction   = document->getAddUpperRightAction();
    addLowerRightAction   = document->getAddLowerRightAction();
    addGenericUpperAction = document->getAddGenericUpperAction();
    addGenericLowerAction = document->getAddGenericLowerAction();
    removeEnclosingAction = document->getRemoveEnclosingAction();

    mn_indexList = new QPopupMenu();
    mn_indexList->insertItem(BarIcon("index0"),0);
    mn_indexList->insertSeparator();
    mn_indexList->insertItem(BarIcon("index1"),1);
    mn_indexList->insertSeparator();
    mn_indexList->insertItem(BarIcon("index2"),2);
    mn_indexList->insertSeparator();
    mn_indexList->insertItem(BarIcon("index3"),3);
    mn_indexList->setMouseTracking(true);
    mn_indexList->setCheckable(false);

    (void) KStdAction::selectAll(formulaWidget, SLOT(slotSelectAll()), actionCollection());

//     actionElement_AddElement_T = new KAction(i18n( "Add/change to simple text" ),
//                                              "mini-xy",
//                                              CTRL + Key_1 ,
//                                              this,SLOT(addText()),
//                                              actionCollection(),"addtext");

//     actionElement_AddElement_V = new KAction(i18n( "Add/change to vertical space" ),
//                                              "mini-vspace",
//                                              CTRL + Key_4 ,
//                                              this,SLOT(addVertSpace()),
//                                              actionCollection(),"addvspace");

//     actionElement_AddElement_L = new KAction(i18n( "Add index at pos..." ),
//                                              "index",
//                                              CTRL + Key_9 ,
//                                              this,SLOT(indexList()),
//                                              actionCollection(),"addindex");

    // font stuff
    KFontAction* actionElement_Text_Font = new KFontAction(i18n( "Font family" ),0,
                                              actionCollection(),"textfont");
    connect( actionElement_Text_Font, SIGNAL( activated( const QString& ) ), this, SLOT( fontSelected( const QString& ) ) );

    KFontSizeAction* actionElement_Text_Size = new KFontSizeAction(i18n( "Size" ),0,
                                                  actionCollection(),"textsize");

    connect( actionElement_Text_Size, SIGNAL( fontSizeChanged( int ) ), this, SLOT( sizeSelected( int ) ) );

    KToggleAction* actionElement_Text_Bold = new KToggleAction(i18n( "Bold" ),
                                                  "bold",
                                                  0,
                                                  actionCollection(),"textbold");
    connect( actionElement_Text_Bold, SIGNAL( toggled( bool ) ), this, SLOT( bold( bool ) ) );

    KToggleAction* actionElement_Text_Italic = new KToggleAction(i18n( "Italic" ),
                                                   "italic",
                                                   0,
                                                   actionCollection(),"textitalic");
    connect( actionElement_Text_Italic, SIGNAL( toggled( bool ) ), this, SLOT( italic( bool ) ) );

    KToggleAction* actionElement_Text_Under = new KToggleAction(i18n( "Underlined" ),
                                                  "underl",
                                                  0,
                                                  actionCollection(),"textunder");
    connect(actionElement_Text_Under, SIGNAL( toggled( bool ) ), this, SLOT( underline( bool ) ) );

//     KToggleAction* actionElement_Font_Element = new KToggleAction(i18n( "Resize element" ),
//                                                     "elementsw",
//                                                     CTRL + ALT + Key_E,
//                                                     this,SLOT(fontSwitch()),
//                                                     actionCollection(),"fontelement");

//     //actionElement_Font_Element->setEnabled(false);

//     actionElement_Font_Child = new KToggleAction(i18n( "Resize children" ),
//                                                  "childrensw",
//                                                  CTRL + ALT + Key_C,
//                                                  this,SLOT(fontSwitch()),
//                                                  actionCollection(),"fontchild");

//     actionElement_Font_Index = new KToggleAction(i18n( "Resize index" ),
//                                                  "indexsw",
//                                                  CTRL + ALT + Key_I,
//                                                  this,SLOT(fontSwitch()),
//                                                  actionCollection(),"fontindex");

//     actionElement_Font_Next = new KToggleAction(i18n( "Resize next" ),
//                                                 "nextsw",
//                                                 CTRL + ALT + Key_N,
//                                                 this,SLOT(fontSwitch()),
//                                                 actionCollection(),"fontnext");

//     // ************************* Text *********************

//     actionElement_Text_Split = new KAction(i18n( "Split text" ),
//                                            "split",
//                                            CTRL + Key_0,
//                                            this,SLOT(textSplit()),
//                                            actionCollection(),"textsplit");

//     // ************************** Root ********************

//     actionElement_Root_Index = new KAction(i18n( "Root index" ),
//                                            "rootindex",
//                                            CTRL + Key_0,
//                                            this,SLOT(addTopLeftIndex()),
//                                            actionCollection(),"rootindex");





//     // ************************** Fraction ********************

//     actionElement_Fraction_VA_M = new KToggleAction(i18n( "Middle align" ),
//                                                     "midalign",
//                                                     0,
//                                                     this,SLOT(fractionAlignM()),
//                                                     actionCollection(),"fracmiddle");

//     //((KToggleAction *)actionElement_Fraction_VA_M)->setExclusiveGroup("FractionVa");

//     actionElement_Fraction_VA_U = new KToggleAction(i18n( "Numerator align" ),
//                                                     "upalign",
//                                                     0,
//                                                     this,SLOT(fractionAlignU()),
//                                                     actionCollection(),"fracup");

//   // ((KToggleAction *)actionElement_Fraction_VA_U)->setExclusiveGroup("FractionVa");

//     actionElement_Fraction_VA_D = new KToggleAction(i18n( "Denominator align" ),
//                                                     "downalign",
//                                                     0,
//                                                     this,SLOT(fractionAlignD()),
//                                                     actionCollection(),"fracdown");

//   // ((KToggleAction *)actionElement_Fraction_VA_D)->setExclusiveGroup("FractionVa");

//     actionElement_Fraction_HA_C = new KToggleAction(i18n( "Center align" ),
//                                                     "centralign",
//                                                     0,
//                                                     this,SLOT(fractionAlignC()),
//                                                     actionCollection(),"fraccenter");

//   // ((KToggleAction *)actionElement_Fraction_HA_C)->setExclusiveGroup("FractionHa");

//     actionElement_Fraction_HA_L = new KToggleAction(i18n( "Left align" ),
//                                                     "leftalign",
//                                                     0,
//                                                     this,SLOT(fractionAlignL()),
//                                                     actionCollection(),"fracleft");

//     // ((KToggleAction *)actionElement_Fraction_HA_L)->setExclusiveGroup("FractionHa");

//     actionElement_Fraction_HA_R = new KToggleAction(i18n( "Right align" ),
//                                                     "rightalign",
//                                                     0,
//                                                     this,SLOT(fractionAlignR()),
//                                                     actionCollection(),"fracright");


//     // ((KToggleAction *)actionElement_Fraction_HA_R)->setExclusiveGroup("FractionHa");

//     actionElement_Fraction_MidLine = new KToggleAction(i18n( "Draw midline" ),
//                                                        "midline",
//                                                        0,
//                                                        this,SLOT(toggleMidline()),
//                                                        actionCollection(),"fracmidline");

//     actionElement_Fraction_Far = new KAction(i18n( "More distant" ),
//                                              "far",
//                                              0,
//                                              this,SLOT(fractionDistMore()),
//                                              actionCollection(),"fracfar");

//     actionElement_Fraction_Near = new KAction(i18n( "Less distant" ),
//                                               "near",
//                                               0,
//                                               this,SLOT(fractionDistLess()),
//                                               actionCollection(),"fracnear");


//     actionElement_Integral_Lower = new KAction(i18n( "Low limit" ),
//                                                "Ilower",
//                                                0,
//                                                this,SLOT(integralLower()),
//                                                actionCollection(),"ilower");

//     actionElement_Integral_Higher = new KAction(i18n( "Hi limit" ),
//                                                 "Ihigher",
//                                                 0,
//                                                 this,SLOT(integralHigher()),
//                                                 actionCollection(),"ihigher");

//     actionElement_Matrix_Set = new KAction(i18n( "Matrix setup" ),
//                                            "matrix",
//                                            0,
//                                            this,SLOT(matrixSet()),
//                                            actionCollection(),"matrixset");

//     actionElement_Matrix_Ins_Row = new KAction(i18n( "Insert row" ),
//                                                "insrow",
//                                                0,
//                                                this,SLOT(matrixInsRow()),
//                                                actionCollection(),"matrixinsrow");
//     actionElement_Matrix_Ins_Col = new KAction(i18n( "Insert col" ),
//                                                "inscol",
//                                                0,
//                                                this,SLOT(matrixInsCol()),
//                                                actionCollection(),"matrixinscol");

//     actionElement_Matrix_Rem_Row = new KAction(i18n( "Remove row" ),
//                                                "remrow",
//                                                0,
//                                                this,SLOT(matrixRemRow()),
//                                                actionCollection(),"matrixremrow");
//     actionElement_Matrix_Rem_Col = new KAction(i18n( "Remove col" ),
//                                                "remcol",
//                                                0,
//                                                this,SLOT(matrixRemCol()),
//                                                actionCollection(),"matrixremcol");


    // notify on cursor change
    connect(formulaWidget, SIGNAL(cursorChanged(bool, bool)),
            this, SLOT(cursorChanged(bool, bool)));
}


KFormulaPartView::~KFormulaPartView()
{
    delete mn_indexList;
}

void KFormulaPartView::focusInEvent(QFocusEvent*)
{
    // After saving the focus doesn't come back to the formulaWidget
    // so the cursor is not shown. (very bad)
    //cerr << "KFormulaPartView::focusInEvent(QFocusEvent*)\n";
    formulaWidget->setFocus();
}

void KFormulaPartView::updateReadWrite(bool readwrite)
{
    formulaWidget->setReadOnly(!readwrite);
    setEnabled(readwrite);
}


void KFormulaPartView::setEnabled(bool enabled)
{
    addBracketAction->setEnabled(enabled);
    addFractionAction->setEnabled(enabled);
    addRootAction->setEnabled(enabled);
    addSumAction->setEnabled(enabled);
    addIntegralAction->setEnabled(enabled);
    addMatrixAction->setEnabled(enabled);
    addUpperLeftAction->setEnabled(enabled);
    addLowerLeftAction->setEnabled(enabled);
    addUpperRightAction->setEnabled(enabled);
    addLowerRightAction->setEnabled(enabled);
    addGenericUpperAction->setEnabled(enabled);
    addGenericLowerAction->setEnabled(enabled);
    removeEnclosingAction->setEnabled(enabled);
}

void KFormulaPartView::resizeEvent( QResizeEvent * )
{
    scrollview->setGeometry(0, 0, width(), height());
}


// void KFormulaPartView::slotTypeChanged( const BasicElement *elm)
// {

// //    actionElement_Font_Element->setEnabled(false);

//     bool isText, isBracket, isFraction, isPrefixed, isMatrix, isRoot;
//     if (elm)
//       {
// 	const type_info& type = typeid(*elm);
// 	isText = type == typeid(TextElement);
// 	isBracket = type == typeid(BracketElement);
// 	isFraction = type == typeid(FractionElement);
// 	isPrefixed = type == typeid(PrefixedElement);
// 	isMatrix = type == typeid(MatrixElement);
// 	isRoot = type == typeid(RootElement);
//       }
//     else
//       {
// 	isRoot = isMatrix = isPrefixed =isFraction = isBracket = isText = false;
//       }

//     actionElement_Text_Split->setEnabled(isText);
//     // It remains deactivated !!!
//     //    m_rMenuBar->setItemEnabled(m_idMenuElement_Fraction,isFraction);
//     if(elm->getIndex(0)==0) //in Root element you can't have more one element
//         (actionElement_Root_Index)->setEnabled(isRoot);
//     //kdDebug(39001) <<"Toggle\n";
//     (actionElement_Bracket_Type_Left)->setEnabled(isBracket);
//     (actionElement_Bracket_Type_Right)->setEnabled(isBracket);

//     (actionElement_Fraction_VA_U)->setEnabled(isFraction);
//     (actionElement_Fraction_VA_D)->setEnabled(isFraction);
//     (actionElement_Fraction_VA_M)->setEnabled(isFraction);
//     (actionElement_Fraction_HA_C)->setEnabled(isFraction);
//     (actionElement_Fraction_HA_L)->setEnabled(isFraction);
//     (actionElement_Fraction_HA_R)->setEnabled(isFraction);

//     //kdDebug(39001) <<"End\n";
//     (actionElement_Fraction_Near)->setEnabled(isFraction);
//     (actionElement_Fraction_Far)->setEnabled(isFraction);
//     (actionElement_Fraction_MidLine)->setEnabled(isFraction);

//     (actionElement_Integral_Lower)->setEnabled(isPrefixed);
//     (actionElement_Integral_Higher)->setEnabled(isPrefixed);
//     (actionElement_Matrix_Set)->setEnabled(isMatrix);
//     (actionElement_Matrix_Ins_Row)->setEnabled(isMatrix);
//     (actionElement_Matrix_Ins_Col)->setEnabled(isMatrix);
//     (actionElement_Matrix_Rem_Row)->setEnabled(isMatrix);
//     (actionElement_Matrix_Rem_Col)->setEnabled(isMatrix);

//     if (elm)
//         {
// 	  ((KFontSizeAction *)actionElement_Text_Size)->setFontSize(elm->getNumericFont());
// 	  kdDebug(39001) <<"Size font :"<<elm->getNumericFont()<<endl;
// 	  BasicElement *el=m_pDoc->currentElement();
// 	  TextElement *te = dynamic_cast<TextElement*>(el);
// 	  if (te==0)
// 	    {
// 	      ((KToggleAction*)actionElement_Text_Bold)->setChecked(false);
// 	      ((KToggleAction*)actionElement_Text_Italic)->setChecked(false);
// 	      ((KToggleAction*)actionElement_Text_Under)->setChecked(false);
// 	    }
// 	  else
// 	    {
// 	      ((KToggleAction*)actionElement_Text_Bold)->setChecked(te->isBold());
// 	      ((KToggleAction*)actionElement_Text_Italic)->setChecked(te->isItalic());
// 	      ((KToggleAction*)actionElement_Text_Under)->setChecked(te->isUnderline());
// 	      ((KFontAction*)actionElement_Text_Font)->setFont(te->textFontFamily());
// 	    }
//         }

//       if (isFraction)
//       {
// 	QString content=elm->getContent();
// 	((KToggleAction *)actionElement_Fraction_HA_C)->setChecked(content[2]=='C');
// 	((KToggleAction *)actionElement_Fraction_HA_L)->setChecked(content[2]=='L');
// 	((KToggleAction *)actionElement_Fraction_HA_R)->setChecked(content[2]=='R');
// 	((KToggleAction *)actionElement_Fraction_VA_M)->setChecked(content[1]=='M');
// 	((KToggleAction *)actionElement_Fraction_VA_U)->setChecked(content[1]=='U');
// 	((KToggleAction *)actionElement_Fraction_VA_D)->setChecked(content[1]=='D');
// 	((KToggleAction *)actionElement_Fraction_MidLine)->setChecked(content[0]=='F');
//       }
//     else if(isMatrix)
//       {
// 	QString content=elm->getContent();
// 	((KToggleAction *)actionElement_Fraction_HA_C)->setChecked(content[2]=='C');
// 	((KToggleAction *)actionElement_Fraction_HA_L)->setChecked(content[2]=='L');
// 	((KToggleAction *)actionElement_Fraction_HA_R)->setChecked(content[2]=='R');
//       }
//     else
//       {
// 	((KToggleAction *)actionElement_Fraction_HA_C)->setChecked(false);
// 	((KToggleAction *)actionElement_Fraction_HA_L)->setChecked(false);
// 	((KToggleAction *)actionElement_Fraction_HA_R)->setChecked(false);
//         ((KToggleAction *)actionElement_Fraction_VA_M)->setChecked(false);
// 	((KToggleAction *)actionElement_Fraction_VA_U)->setChecked(false);
// 	((KToggleAction *)actionElement_Fraction_VA_D)->setChecked(false);
// 	((KToggleAction *)actionElement_Fraction_MidLine)->setChecked(false);
//       }

// //    actionElement_Font_Element->setEnabled(false);
//}


void KFormulaPartView::modifyMatrix(QString str)
{
//     int x,y,old;
//     BasicElement *el=m_pDoc->currentElement();
//     if (el==0)
// 	return;
//     MatrixElement *elm = dynamic_cast<MatrixElement*>(el);
//     if (elm==0)
// 	return;

//     QString oldc=elm->getContent();
//     x=oldc.mid(3,3).toInt();
//     y=oldc.mid(6,3).toInt();
//     old=x*y;
//     x=str.mid(3,3).toInt();
//     y=str.mid(6,3).toInt();
//     elm->setChildrenNumber(x*y);
//     for(int i=old;i<x*y;i++)
// 	elm->setChild(new BasicElement(m_pDoc,elm,i+4),i);
//     for(int i=x*y;i<old;i++)
// 	delete    elm->getChild(i); //code to remove unused children...

//     elm->setContent(str);
//     update();
}


void KFormulaPartView::reduce()
{
//     if(m_pDoc->currentElement()==0)
//       return;
//     int level;
//     level= FN_REDUCE | FN_P43;
//     if(((KToggleAction*)actionElement_Font_Element)->isChecked())
//       level=level | FN_ELEMENT;
//     if(((KToggleAction*)actionElement_Font_Index)->isChecked())
//       level=level | FN_INDEXES;
//     if(((KToggleAction*)actionElement_Font_Child)->isChecked())
//       level=level | FN_CHILDREN;
//     if(((KToggleAction*)actionElement_Font_Next)->isChecked())
//       level=level | FN_NEXT;

//     m_pDoc->currentElement()->scaleNumericFont(level);

//     update();
}

void KFormulaPartView::enlarge()
{
//     if(m_pDoc->currentElement()==0)
//       return;
//     int level;
//     level= FN_ENLARGE | FN_P43;

//     //kdDebug(39001) <<m_vToolBarFont->comboItem(m_idComboFont_ScaleMode)<<endl;
//     if(((KToggleAction*)actionElement_Font_Element)->isChecked())
//       level=level | FN_ELEMENT;

//     if(((KToggleAction*)actionElement_Font_Index)->isChecked())
//       level=level | FN_INDEXES;

//     if(((KToggleAction*)actionElement_Font_Child)->isChecked())
//       level=level | FN_CHILDREN;

//     if(((KToggleAction*)actionElement_Font_Next)->isChecked())
//       level=level | FN_NEXT;

//     m_pDoc->currentElement()->scaleNumericFont(level);
//     update();
}


void KFormulaPartView::fractionAlignM()
{
//   if(!((KToggleAction*)actionElement_Fraction_VA_M)->isChecked())
//       if( (((KToggleAction*)actionElement_Fraction_VA_D)->isChecked())
// 	|| (((KToggleAction*)actionElement_Fraction_VA_U)->isChecked())
//       ) return;

//     kdDebug(39001) <<"M\n";
//     QString content=m_pDoc->currentElement()->getContent();
//     content[1]='M';
//     m_pDoc->currentElement()->setContent(content);

//     ((KToggleAction *)actionElement_Fraction_VA_M)->setChecked(true);
//     ((KToggleAction *)actionElement_Fraction_VA_D)->setChecked(false);
//     ((KToggleAction *)actionElement_Fraction_VA_U)->setChecked(false);
//     update();
}

void KFormulaPartView::fractionAlignU()
{
//     if(!((KToggleAction*)actionElement_Fraction_VA_U)->isChecked())
//     if( (((KToggleAction*)actionElement_Fraction_VA_D)->isChecked())
//       || (((KToggleAction*)actionElement_Fraction_VA_M)->isChecked())
//       ) return;

//     kdDebug(39001) <<"U\n";
//     QString content=m_pDoc->currentElement()->getContent();
//     content[1]='U';
//     m_pDoc->currentElement()->setContent(content);
//     ((KToggleAction *)actionElement_Fraction_VA_U)->setChecked(true);
//     ((KToggleAction *)actionElement_Fraction_VA_D)->setChecked(false);
//     ((KToggleAction *)actionElement_Fraction_VA_M)->setChecked(false);

//     update();
}

void KFormulaPartView::fractionAlignD()
{
//     if(!((KToggleAction*)actionElement_Fraction_VA_D)->isChecked())
//     if( (((KToggleAction*)actionElement_Fraction_VA_M)->isChecked())
//       || (((KToggleAction*)actionElement_Fraction_VA_U)->isChecked())
//       ) return;

//     kdDebug(39001) <<"D\n";
//     QString content=m_pDoc->currentElement()->getContent();
//     content[1]='D';
//     m_pDoc->currentElement()->setContent(content);
//     ((KToggleAction *)actionElement_Fraction_VA_D)->setChecked(true);
//     ((KToggleAction *)actionElement_Fraction_VA_M)->setChecked(false);
//     ((KToggleAction *)actionElement_Fraction_VA_U)->setChecked(false);
//     update();
}

void KFormulaPartView::fractionAlignL()
{
//     QString content=m_pDoc->currentElement()->getContent();
//     content[2]='L';
//     m_pDoc->currentElement()->setContent(content);
//     ((KToggleAction *)actionElement_Fraction_HA_L)->setChecked(true);
//     ((KToggleAction *)actionElement_Fraction_HA_C)->setChecked(false);
//     ((KToggleAction *)actionElement_Fraction_HA_R)->setChecked(false);
//     update();
}

void KFormulaPartView::fractionAlignR()
{
//     QString content=m_pDoc->currentElement()->getContent();
//     content[2]='R';
//     m_pDoc->currentElement()->setContent(content);
//     ((KToggleAction *)actionElement_Fraction_HA_L)->setChecked(false);
//     ((KToggleAction *)actionElement_Fraction_HA_C)->setChecked(false);
//     ((KToggleAction *)actionElement_Fraction_HA_R)->setChecked(true);
//     update();
}

void KFormulaPartView::fractionAlignC()
{
//     QString content=m_pDoc->currentElement()->getContent();
//     content[2]='C';
//     m_pDoc->currentElement()->setContent(content);
//     ((KToggleAction *)actionElement_Fraction_HA_L)->setChecked(false);
//     ((KToggleAction *)actionElement_Fraction_HA_C)->setChecked(true);
//     ((KToggleAction *)actionElement_Fraction_HA_R)->setChecked(false);
//     update();
}

void KFormulaPartView::fractionDist()
{

}

void KFormulaPartView::fractionDistMore()
{
//     QString content=m_pDoc->currentElement()->getContent();

//     QString tmpContent=content;
//     content=tmpContent.left(3);
//     content+=QString::number(tmpContent.right(tmpContent.length()-3).toInt()+1);

//     kdDebug(39001) <<content<<endl;
//     m_pDoc->currentElement()->setContent(content);
//     update();
}

void KFormulaPartView::fractionDistLess()
{
//     QString content=m_pDoc->currentElement()->getContent();
//     int space=content.right(content.length()-3).toInt()-1;
//     if (space<1)
//         space =1;

//     content=content.left(3);
//     content+=QString::number(space);

//     kdDebug(39001) <<content<<endl;
//     m_pDoc->currentElement()->setContent(content);
//     update();
}

void KFormulaPartView::fontSwitch()
{
//   if(m_vToolBarFont->isButtonOn(m_idButtonFont_2)) warning("element");
//    if(m_vToolBarFont->isButtonOn(m_idButtonFont_3)) warning("index");
//    if(m_vToolBarFont->isButtonOn(m_idButtonFont_4)) warning("children");
//    if(m_vToolBarFont->isButtonOn(m_idButtonFont_5)) warning("next");
}

void KFormulaPartView::bold(bool /*b*/)
{
//     BasicElement *el=m_pDoc->currentElement();
//     if (el==0)
// 	return;

//     TextElement *te = dynamic_cast<TextElement*>(el);
//     if (te==0)
// 	return;
//     te->changePropertieFont(b);
//     kdDebug(39001) <<"bold : "<<b<<endl;
//     update();
}

void KFormulaPartView::italic(bool /*b*/)
{
//     BasicElement *el=m_pDoc->currentElement();
//     if (el==0)
// 	return;

//     TextElement *te = dynamic_cast<TextElement*>(el);
//     if (te==0)
// 	return;
//     te->changePropertieFont(-1,-1,b);
//     kdDebug(39001) <<"italic : "<<b<<endl;
//     update();
}

void KFormulaPartView::underline(bool /*b*/)
{
//    BasicElement *el=m_pDoc->currentElement();
//     if (el==0)
// 	return;

//     TextElement *te = dynamic_cast<TextElement*>(el);
//     if (te==0)
// 	return;
//     te->changePropertieFont(-1,b);
//     kdDebug(39001) <<"underline : "<<b<<endl;
//     update();
}

void KFormulaPartView::indexList()
{
//     //kdDebug(39001)<<"index\n";
//     QPoint pnt(QCursor::pos());
//     mn_indexList->popup(pnt);
}

void KFormulaPartView::sizeSelected(int /*size*/)
{
//     BasicElement *el=m_pDoc->currentElement();
//     if (el==0)
//         return;
//     el->setNumericFont(size);
//     kdDebug(39001) <<"size selected :"<<size<<endl;
//     update();
}

void KFormulaPartView::fontSelected( const QString& /*font*/)
{

//     BasicElement *el=m_pDoc->currentElement();
//     if (el==0)
// 	return;

//     TextElement *te = dynamic_cast<TextElement*>(el);

//     if (te==0)
// 	return;
// /*    if (font == "(default)")
// 	font="";*/
//     te->changeFontFamily(font);
//     kdDebug(39001) <<font<<endl;
//     update();
}

void KFormulaPartView::modeSelected(char *)
{
    kdDebug(39001) <<"mode\n";
}

void KFormulaPartView::textFont()
{
    kdDebug(39001) <<"Slot textFont\n";
}

void KFormulaPartView::textSplit()
{
    kdDebug(39001) <<"Slot textSplit\n";

//     BasicElement *el=m_pDoc->currentElement();

//     if (el==0)
// 	return;

//     TextElement *te = dynamic_cast<TextElement*>(el);

//     if (te==0)
// 	return;

//     te->split(-1);
//     update();

}

void KFormulaPartView::togglePixmap()
{
    kdDebug(39001) <<"Slot togglePixmap\n";
}

void KFormulaPartView::integralLower()
{
    kdDebug(39001) <<"Slot integralLower\n";
    //m_pDoc->addChild(2);
}

void KFormulaPartView::integralHigher()
{
    kdDebug(39001) <<"Slot integralHigher\n";
    //m_pDoc->addChild(1);
}

void KFormulaPartView::bracketType()
{
    kdDebug(39001) <<"Slot bracketType\n";
}

void KFormulaPartView::matrixSet()
{
//     MatrixSetupWidget *ms=new MatrixSetupWidget();
//     ms->setString(m_pDoc->currentElement()->getContent());
//     QObject::connect(ms,SIGNAL(returnString(QString)),this,SLOT(modifyMatrix(QString)));
//     ms->show();
}

void KFormulaPartView::matrixRemRow()
{
    kdDebug(39001) <<"Slot matrixRemRow\n";
}

void KFormulaPartView::matrixRemCol()
{
    kdDebug(39001) <<"Slot matrixRemCol\n";
}

void KFormulaPartView::matrixInsRow()
{
    kdDebug(39001) <<"Slot matrixInsRow\n";
}

void KFormulaPartView::matrixInsCol()
{
    kdDebug(39001) <<"Slot matrixInsCol\n";
}

void KFormulaPartView::fractionHAlign()
{
    kdDebug(39001) <<"Slot fractionHAlign\n";
}

void KFormulaPartView::toggleMidline()
{
//     kdDebug(39001) <<"Slot toggleMidline\n";
//     QString content=m_pDoc->currentElement()->getContent();
//     if (((KToggleAction *)actionElement_Fraction_MidLine)->isChecked())
//      content[0]='F';
//     else content[0]='V';
//     m_pDoc->currentElement()->setContent(content);
//     update();
}

void KFormulaPartView::symbolType()
{
    kdDebug(39001) <<"Slot symbolType\n";
}

void KFormulaPartView::DecorationType()
{
    kdDebug(39001) <<"Slot decorationType\n";
}

void KFormulaPartView::remove()
{
    kdDebug(39001) <<"Slot remove\n";
}

void KFormulaPartView::elementColor()
{
    kdDebug(39001) <<"Slot elementColor\n";
}

void KFormulaPartView::generalColor()
{
    kdDebug(39001) <<"Slot generalColor\n";
}

void KFormulaPartView::generalFont()
{
    kdDebug(39001) <<"Slot generalFont\n";
}

void KFormulaPartView::setupPrinter(KPrinter&)
{
}

void KFormulaPartView::print(KPrinter& printer)
{
    m_pDoc->getFormula()->print(printer);
}

void KFormulaPartView::cursorChanged(bool visible, bool selecting)
{
    cutAction->setEnabled(visible && selecting);
    copyAction->setEnabled(visible && selecting);

    removeEnclosingAction->setEnabled(!selecting);

    if (visible) {
        int x = formulaWidget->getCursorPoint().x();
        int y = formulaWidget->getCursorPoint().y();
        scrollview->ensureVisible(x, y);
    }
}

#include "kformula_view.moc"

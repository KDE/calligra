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

    (void) KStdAction::selectAll(formulaWidget, SLOT(slotSelectAll()), actionCollection());


    // font stuff
//     KFontAction* actionElement_Text_Font = new KFontAction(i18n( "Font family" ),0,
//                                               actionCollection(),"textfont");
//     connect( actionElement_Text_Font, SIGNAL( activated( const QString& ) ), this, SLOT( fontSelected( const QString& ) ) );

//     KFontSizeAction* actionElement_Text_Size = new KFontSizeAction(i18n( "Size" ),0,
//                                                   actionCollection(),"textsize");

//     connect( actionElement_Text_Size, SIGNAL( fontSizeChanged( int ) ), this, SLOT( sizeSelected( int ) ) );

//     KToggleAction* actionElement_Text_Bold = new KToggleAction(i18n( "Bold" ),
//                                                   "bold",
//                                                   0,
//                                                   actionCollection(),"textbold");
//     connect( actionElement_Text_Bold, SIGNAL( toggled( bool ) ), this, SLOT( bold( bool ) ) );

//     KToggleAction* actionElement_Text_Italic = new KToggleAction(i18n( "Italic" ),
//                                                    "italic",
//                                                    0,
//                                                    actionCollection(),"textitalic");
//     connect( actionElement_Text_Italic, SIGNAL( toggled( bool ) ), this, SLOT( italic( bool ) ) );

//     KToggleAction* actionElement_Text_Under = new KToggleAction(i18n( "Underlined" ),
//                                                   "underl",
//                                                   0,
//                                                   actionCollection(),"textunder");
//     connect(actionElement_Text_Under, SIGNAL( toggled( bool ) ), this, SLOT( underline( bool ) ) );


    // notify on cursor change
    connect(formulaWidget, SIGNAL(cursorChanged(bool, bool)),
            this, SLOT(cursorChanged(bool, bool)));
}


KFormulaPartView::~KFormulaPartView()
{
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

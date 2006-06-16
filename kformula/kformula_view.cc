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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

class KPrinter;

#include <QPainter>
#include <q3popupmenu.h>
#include <q3textedit.h>
#include <QTimer>
#include <QScrollArea>
#include <QFocusEvent>
#include <QResizeEvent>

#include <kaction.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kstdaction.h>
#include <ktip.h>
//#include <kglobal.h>

#include <kformulacontainer.h>
#include <kformuladocument.h>

#include "formulastring.h"
#include "fsparser.h"
#include "kfconfig.h"
#include "kformula_doc.h"
#include "kformula_factory.h"
#include "kformula_view.h"
#include "kformula_view_adaptor.h"

#include "kformula_view_adaptor.h"
#include "kformulawidget.h"
#include <kfontsizeaction.h>
#include <ktoggleaction.h>

bool KFormulaPartView::first_window = true;

KFormulaPartView::KFormulaPartView(KFormulaDoc* _doc, QWidget* _parent, const char* _name)
        : KoView( _doc, _parent, _name ), m_pDoc(_doc)
{
    m_dbus = new KformulaViewAdaptor(this);
    QDBus::sessionBus().registerObject( "/" + objectName(), this);

    setInstance(KFormulaFactory::global());
    if ( !_doc->isReadWrite() )
        setXMLFile("kformula_readonly.rc");
    else
        setXMLFile("kformula.rc");

//     m_dcop = 0;
//     dcopObject(); // build it
    new KformulaViewAdaptor(this);

    formulaWidget = new KFormulaWidget( _doc->getFormula(), this, "formulaWidget" );

    m_scrollArea = new QScrollArea( this);
    m_scrollArea->setWidget( formulaWidget );

//    scrollview->viewport()->setFocusProxy( scrollview );
//    scrollview->viewport()->setFocusPolicy( WheelFocus );
//    scrollview->setFocusPolicy( NoFocus );
    formulaWidget->setFocus();

    // Nice parts start in read only mode.
    formulaWidget->setReadOnly(true);

    KFormula::Container* formula = m_pDoc->getFormula();
    KFormula::Document* document = m_pDoc->getDocument();

    // copy&paste
    cutAction   = KStdAction::cut(document->wrapper(), SLOT(cut()), actionCollection());
    copyAction  = KStdAction::copy(document->wrapper(), SLOT(copy()), actionCollection());
    pasteAction = KStdAction::paste(document->wrapper(), SLOT(paste()), actionCollection());
    cutAction->setEnabled(false);
    copyAction->setEnabled(false);

    // tip of the day
    KStdAction::tipOfDay( this, SLOT( slotShowTip() ), actionCollection() );

    // elements
    addBracketAction      = document->wrapper()->getAddBracketAction();
    addFractionAction     = document->wrapper()->getAddFractionAction();
    addRootAction         = document->wrapper()->getAddRootAction();
    addSumAction          = document->wrapper()->getAddSumAction();
    addProductAction      = document->wrapper()->getAddProductAction();
    addIntegralAction     = document->wrapper()->getAddIntegralAction();
    addMatrixAction       = document->wrapper()->getAddMatrixAction();
    addUpperLeftAction    = document->wrapper()->getAddUpperLeftAction();
    addLowerLeftAction    = document->wrapper()->getAddLowerLeftAction();
    addUpperRightAction   = document->wrapper()->getAddUpperRightAction();
    addLowerRightAction   = document->wrapper()->getAddLowerRightAction();
    addGenericUpperAction = document->wrapper()->getAddGenericUpperAction();
    addGenericLowerAction = document->wrapper()->getAddGenericLowerAction();
    removeEnclosingAction = document->wrapper()->getRemoveEnclosingAction();

    (void) KStdAction::selectAll(formulaWidget, SLOT(slotSelectAll()), actionCollection());

    //------------------------ Settings menu
    KStdAction::preferences( this, SLOT(configure()), actionCollection(), "configure" );

    // font stuff
//     KFontAction* actionElement_Text_Font = new KFontAction(i18n( "Font Family" ),0,
//                                               actionCollection(),"textfont");
//     connect( actionElement_Text_Font, SIGNAL( activated( const QString& ) ), this, SLOT( fontSelected( const QString& ) ) );

    KFontSizeAction* actionTextSize = new KFontSizeAction(i18n( "Size" ),
                                                          actionCollection(),"formula_textsize");
    actionTextSize->setFontSize( m_pDoc->getFormula()->fontSize() );

    connect( actionTextSize, SIGNAL( fontSizeChanged( int ) ), this, SLOT( sizeSelected( int ) ) );
    connect( formula, SIGNAL( baseSizeChanged( int ) ), actionTextSize, SLOT( setFontSize( int ) ) );

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


    formulaStringAction = new KAction( i18n( "Edit Formula String..." ),
                                       0,
                                       this, SLOT( formulaString() ),
                                       actionCollection(), "formula_formulastring" );
    // notify on cursor change
    connect(formulaWidget, SIGNAL(cursorChanged(bool, bool)),
            this, SLOT(cursorChanged(bool, bool)));

    connect( formula, SIGNAL( statusMsg( const QString& ) ),
             this, SLOT( slotActionStatusText( const QString& ) ) );

    if ( !_doc->isEmbedded() && first_window ) {
        QTimer::singleShot( 200, this, SLOT(slotShowTipOnStart()) );
        first_window = false;
    }
}


KFormulaPartView::~KFormulaPartView()
{
    //delete m_bus;
}

KformulaViewAdaptor* KFormulaPartView::dbusObject()
{
    return m_dbus;
}


void KFormulaPartView::focusInEvent(QFocusEvent*)
{
    formulaWidget->setFocus();
}

void KFormulaPartView::updateReadWrite(bool readwrite)
{
    formulaWidget->setReadOnly(!readwrite);
    setEnabled(readwrite);
}

void KFormulaPartView::slotShowTipOnStart() {
    KTipDialog::showTip( this );
}

void KFormulaPartView::slotShowTip() {
    KTipDialog::showTip( this, QString::null, true );
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
    m_scrollArea->setGeometry(0, 0, width(), height());
}


void KFormulaPartView::setupPrinter(KPrinter&)
{
}

void KFormulaPartView::print(KPrinter& printer)
{
    m_pDoc->getFormula()->print(printer);
}

const KFormula::View* KFormulaPartView::formulaView() const { return formulaWidget->view(); }
KFormula::View* KFormulaPartView::formulaView() { return formulaWidget->view(); }

void KFormulaPartView::cursorChanged(bool visible, bool selecting)
{
    cutAction->setEnabled(visible && selecting);
    copyAction->setEnabled(visible && selecting);

    removeEnclosingAction->setEnabled(!selecting);

    if (visible) {
        int x = formulaWidget->getCursorPoint().x();
        int y = formulaWidget->getCursorPoint().y();
        m_scrollArea->ensureVisible(x, y);
    }

    KFormula::Document* doc = document()->getDocument();
    doc->wrapper()->getFormatBoldAction()->setEnabled( selecting );
    doc->wrapper()->getFormatItalicAction()->setEnabled( selecting );
    doc->wrapper()->getFontFamilyAction()->setEnabled( selecting );
    if ( !selecting ) {
        doc->wrapper()->getFormatBoldAction()->setChecked( false );
        doc->wrapper()->getFormatItalicAction()->setChecked( false );
        doc->wrapper()->getFontFamilyAction()->setCurrentItem( 0 );
    }
}

void KFormulaPartView::formulaString()
{
#warning diabled FormulaString dialog as KOffice 2 should support MathML tree editing instead
//    FormulaString dia( this );
//    dia.setEditText( document()->getFormula()->formulaString() );
//    dia.exec();
}

void KFormulaPartView::sizeSelected( int size )
{
    document()->getFormula()->setFontSize( size );
    formulaWidget->setFocus();
}

QStringList KFormulaPartView::readFormulaString( QString text )
{
    FormulaStringParser parser( document()->getDocument()->getSymbolTable(), text );
    QDomDocument formula = parser.parse();
    QStringList errorList = parser.errorList();
    //if ( errorList.count() == 0 ) {
        formulaView()->slotSelectAll();
        document()->getFormula()->paste( formula, i18n( "Read Formula String" ) );
        //}
    return errorList;
}

void KFormulaPartView::configure()
{
    KFConfig configDia( this );
    configDia.exec();
}

#include "kformula_view.moc"

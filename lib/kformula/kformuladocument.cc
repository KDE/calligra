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

#include <qlist.h>
#include <qstringlist.h>

#include <kdebug.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kstddirs.h>

#include "contextstyle.h"
#include "kformulacontainer.h"
#include "kformuladocument.h"
#include "symboltable.h"

KFORMULA_NAMESPACE_BEGIN

struct KFormulaDocument::KFormulaDocument_Impl {


    KFormulaDocument_Impl()
            : leftBracketChar('('), rightBracketChar(')'),
              syntaxHighlighting(true), formula(0)
    {
        formulae.setAutoDelete(false);
    }


    ~KFormulaDocument_Impl()
    {
        if (ownHistory) {
            delete history;
        }
    }

    // We know our actions, maybe a client is interessted...

    KAction* addBracketAction;
    KAction* addSBracketAction;
    KAction* addCBracketAction;
    KAction* addAbsAction;
    KAction* addFractionAction;
    KAction* addRootAction;
    KAction* addSumAction;
    KAction* addProductAction;
    KAction* addIntegralAction;
    KAction* addMatrixAction;
    KAction* addOneByTwoMatrixAction;
    KAction* addUpperLeftAction;
    KAction* addLowerLeftAction;
    KAction* addUpperRightAction;
    KAction* addLowerRightAction;
    KAction* addGenericUpperAction;
    KAction* addGenericLowerAction;
    KAction* removeEnclosingAction;
    KAction* makeGreekAction;
    KAction* insertSymbolAction;

    KToggleAction* syntaxHighlightingAction;
    KSelectAction* leftBracket;
    KSelectAction* rightBracket;
    KSelectAction* symbolNamesAction;


    char leftBracketChar;
    char rightBracketChar;
    QString selectedName;

    /**
     * Whether we want coloured formulae.
     */
    bool syntaxHighlighting;

    /**
     * The active formula.
     */
    KFormulaContainer* formula;

    /**
     * Our undo stack. We don't own it. The stack belongs to
     * our parent and might contain not formula related commands
     * as well.
     */
    KCommandHistory* history;

    /**
     * Tells whether we are responsible to remove our history.
     */
    bool ownHistory;

    /**
     * The symbols/names that are "known" to the system.
     */
    SymbolTable table;

    /**
     * The documents context style. This is the place where all
     * the user configurable informations are stored.
     */
    ContextStyle contextStyle;

    /**
     * All formulae that belong to this document.
     */
    QList<KFormulaContainer> formulae;
};


double KFormulaDocument::getXResolution() const { return impl->contextStyle.getXResolution(); }
double KFormulaDocument::getYResolution() const { return impl->contextStyle.getYResolution(); }

void KFormulaDocument::setResolution(double zX, double zY) { impl->contextStyle.setResolution(zX, zY); }

KCommandHistory* KFormulaDocument::getHistory() const { return impl->history; }
const SymbolTable& KFormulaDocument::getSymbolTable() const { return impl->table; }

KAction* KFormulaDocument::getAddBracketAction()       { return impl->addBracketAction; }
KAction* KFormulaDocument::getAddFractionAction()      { return impl->addFractionAction; }
KAction* KFormulaDocument::getAddRootAction()          { return impl->addRootAction; }
KAction* KFormulaDocument::getAddSumAction()           { return impl->addSumAction; }
KAction* KFormulaDocument::getAddProductAction()       { return impl->addProductAction; }
KAction* KFormulaDocument::getAddIntegralAction()      { return impl->addIntegralAction; }
KAction* KFormulaDocument::getAddMatrixAction()        { return impl->addMatrixAction; }
KAction* KFormulaDocument::getAddOneByTwoMatrixAction(){ return impl->addOneByTwoMatrixAction; }
KAction* KFormulaDocument::getAddUpperLeftAction()     { return impl->addUpperLeftAction; }
KAction* KFormulaDocument::getAddLowerLeftAction()     { return impl->addLowerLeftAction; }
KAction* KFormulaDocument::getAddUpperRightAction()    { return impl->addUpperRightAction; }
KAction* KFormulaDocument::getAddLowerRightAction()    { return impl->addLowerRightAction; }
KAction* KFormulaDocument::getAddGenericUpperAction()  { return impl->addGenericUpperAction; }
KAction* KFormulaDocument::getAddGenericLowerAction()  { return impl->addGenericLowerAction; }
KAction* KFormulaDocument::getRemoveEnclosingAction()  { return impl->removeEnclosingAction; }
KAction* KFormulaDocument::getMakeGreekAction()        { return impl->makeGreekAction; }
KAction* KFormulaDocument::getInsertSymbolAction()     { return impl->insertSymbolAction; }
KSelectAction* KFormulaDocument::getLeftBracketAction()  { return impl->leftBracket; }
KSelectAction* KFormulaDocument::getRightBracketAction() { return impl->rightBracket; }
KSelectAction* KFormulaDocument::getSymbolNamesAction()  { return impl->symbolNamesAction; }
KToggleAction* KFormulaDocument::getSyntaxHighlightingAction() { return impl->syntaxHighlightingAction; }

KFormulaContainer* KFormulaDocument::formula() const { return impl->formula; }


KFormulaDocument::KFormulaDocument(KActionCollection* collection,
                                   KCommandHistory* his)
{
    impl = new KFormulaDocument_Impl;

    KGlobal::dirs()->addResourceType("toolbar", KStandardDirs::kde_default("data") + "kformula/pics/");
    createActions(collection);
    impl->syntaxHighlightingAction->setChecked(impl->syntaxHighlighting);

    if (his == 0) {
        impl->history = new KCommandHistory(collection);
        impl->ownHistory = true;
    }
    else {
        impl->history = his;
        impl->ownHistory = false;
    }
}


KFormulaDocument::KFormulaDocument(KCommandHistory* his)
{
    impl = new KFormulaDocument_Impl;
    if (his == 0) {
        impl->history = new KCommandHistory;
        impl->ownHistory = true;
    }
    else {
        impl->history = his;
        impl->ownHistory = false;
    }
}


KFormulaDocument::~KFormulaDocument()
{
    delete impl;
}


ContextStyle& KFormulaDocument::getContextStyle( bool forPrinting )
{
    // Make sure not to change anything depending on `forPrinting' that
    // would require a new calculation of the formula.
    //kdDebug() << "KFormulaDocument::activate: forPrinting=" << forPrinting << endl;
    impl->contextStyle.setSyntaxHighlighting( forPrinting ? false : impl->syntaxHighlighting );
    return impl->contextStyle;
}


KFormulaContainer* KFormulaDocument::createFormula()
{
    KFormulaContainer* f = new KFormulaContainer(this);
    impl->formulae.append(f);
    return f;
}


void KFormulaDocument::activate(KFormulaContainer* f)
{
    impl->formula = f;
}


void KFormulaDocument::formulaDies(KFormulaContainer* f)
{
    if (f == impl->formula) {
        impl->formula = 0;
    }
    impl->formulae.remove(f);
}


void KFormulaDocument::createActions(KActionCollection* collection)
{
    impl->addIntegralAction = new KAction(i18n("Add/change to integral"),
                                    "int",
                                    CTRL + Key_6,
                                    this, SLOT(addIntegral()),
                                    collection, "formula_addintegral");
    impl->addSumAction      = new KAction(i18n("Add/change to sum"),
                                    "sum",
                                    CTRL + Key_7,
                                    this, SLOT(addSum()),
                                    collection, "formula_addsum");
    impl->addProductAction  = new KAction(i18n("Add/change to product"),
                                    "prod",
                                    CTRL + Key_4,
                                    this, SLOT(addProduct()),
                                    collection, "formula_addproduct");
    impl->addRootAction     = new KAction(i18n("Add/change to root"),
                                    "sqrt",
                                    CTRL + Key_2,
                                    this, SLOT(addRoot()),
                                    collection, "formula_addroot");
    impl->addFractionAction = new KAction(i18n("Add/change to fraction"),
                                    "frac",
                                    CTRL + Key_3,
                                    this, SLOT(addFraction()),
                                    collection, "formula_addfrac");
    impl->addBracketAction  = new KAction(i18n("Add/change to bracket"),
                                    "paren",
                                    0,
                                    this, SLOT(addDefaultBracket()),
                                    collection,"formula_addbra");
    impl->addSBracketAction = new KAction(i18n("Add/change to square bracket"),
                                    "brackets",
                                    0,
                                    this, SLOT(addSquareBracket()),
                                    collection,"formula_addsqrbra");
    impl->addCBracketAction = new KAction(i18n("Add/change to curly bracket"),
                                    "brace",
                                    0,
                                    this, SLOT(addCurlyBracket()),
                                    collection,"formula_addcurbra");
    impl->addAbsAction      = new KAction(i18n("Add/change to abs"),
                                    "abs",
                                    0,
                                    this, SLOT(addLineBracket()),
                                    collection,"formula_addabsbra");

    impl->addMatrixAction   = new KAction(i18n("Add matrix"),
                                    "matrix",
                                    CTRL + Key_8,
                                    this, SLOT(addMatrix()),
                                    collection, "formula_addmatrix");

    impl->addOneByTwoMatrixAction   = new KAction(i18n("Add 1x2 matrix"),
                                    "onetwomatrix",
                                    0,
                                    this, SLOT(addOneByTwoMatrix()),
                                    collection, "formula_add_one_by_two_matrix");


    impl->addUpperLeftAction  = new KAction(i18n("Add upper left index"),
                                      "lsup",
                                      0,
                                      this, SLOT(addUpperLeftIndex()),
                                      collection, "formula_addupperleft");
    impl->addLowerLeftAction  = new KAction(i18n("Add lower left index"),
                                      "lsub",
                                      0,
                                      this, SLOT(addLowerLeftIndex()),
                                      collection, "formula_addlowerleft");
    impl->addUpperRightAction = new KAction(i18n("Add upper right index"),
                                      "rsup",
                                      0,
                                      this, SLOT(addUpperRightIndex()),
                                      collection, "formula_addupperright");
    impl->addLowerRightAction = new KAction(i18n("Add lower right index"),
                                      "rsub",
                                      0,
                                      this, SLOT(addLowerRightIndex()),
                                      collection, "formula_addlowerright");

    impl->addGenericUpperAction = new KAction(i18n("Add upper index"),
                                      CTRL + Key_U,
                                      this, SLOT(addGenericUpperIndex()),
                                      collection, "formula_addupperindex");
    impl->addGenericLowerAction = new KAction(i18n("Add lower index"),
                                      CTRL + Key_L,
                                      this, SLOT(addGenericLowerIndex()),
                                      collection, "formula_addlowerindex");

    impl->removeEnclosingAction = new KAction(i18n("Remove enclosing element"),
                                        CTRL + Key_R,
                                        this, SLOT(removeEnclosing()),
                                        collection, "formula_removeenclosing");

    impl->makeGreekAction = new KAction(i18n("Convert to Greek"),
                                  CTRL + Key_G,
                                  this, SLOT(makeGreek()),
                                  collection, "formula_makegreek");

    impl->syntaxHighlightingAction = new KToggleAction(i18n("Syntax highlighting"),
                                                 0,
                                                 this, SLOT(toggleSyntaxHighlighting()),
                                                 collection, "formula_syntaxhighlighting");

    QStringList delimiter;
    delimiter.append(QString("("));
    delimiter.append(QString("["));
    delimiter.append(QString("{"));
    delimiter.append(QString("<"));
    delimiter.append(QString("/"));
    delimiter.append(QString("\\"));
    delimiter.append(QString("|"));
    delimiter.append(QString(" "));
    delimiter.append(QString(")"));
    delimiter.append(QString("]"));
    delimiter.append(QString("}"));
    delimiter.append(QString(">"));
    impl->leftBracket = new KSelectAction(i18n("Left delimiter"),
                                    0, this, SLOT(delimiterLeft()),
                                    collection, "formula_typeleft");
    impl->leftBracket->setItems(delimiter);
    //leftBracket->setCurrentItem(0);

    delimiter.clear();
    delimiter.append(QString(")"));
    delimiter.append(QString("]"));
    delimiter.append(QString("}"));
    delimiter.append(QString(">"));
    delimiter.append(QString("/"));
    delimiter.append(QString("\\"));
    delimiter.append(QString("|"));
    delimiter.append(QString(" "));
    delimiter.append(QString("("));
    delimiter.append(QString("["));
    delimiter.append(QString("{"));
    delimiter.append(QString("<"));
    impl->rightBracket = new KSelectAction(i18n("Right delimiter"),
                                     0, this, SLOT(delimiterRight()),
                                     collection, "formula_typeright");
    impl->rightBracket->setItems(delimiter);
    //rightBracket->setCurrentItem(0);

    impl->insertSymbolAction = new KAction(i18n("Insert symbol"),
                                        CTRL + Key_I,
                                        this, SLOT(insertSymbol()),
                                        collection, "formula_insertsymbol");
    QStringList names = impl->table.allNames();
    impl->symbolNamesAction = new KSelectAction(i18n("Symbol names"),
                                          0, this, SLOT(symbolNames()),
                                          collection, "formula_symbolnames");
    impl->symbolNamesAction->setItems(names);
    impl->selectedName = names[0];
}


void KFormulaDocument::paste()
{
    if (hasFormula()) {
        formula()->paste();
    }
}

void KFormulaDocument::copy()
{
    if (hasFormula()) {
        formula()->copy();
    }
}

void KFormulaDocument::cut()
{
    if (hasFormula()) {
        formula()->cut();
    }
}

void KFormulaDocument::addDefaultBracket()
{
    if (hasFormula()) {
        formula()->addBracket(impl->leftBracketChar, impl->rightBracketChar);
    }
}

void KFormulaDocument::addSquareBracket()
{
    if (hasFormula()) {
        formula()->addSquareBracket();
    }
}

void KFormulaDocument::addCurlyBracket()
{
    if (hasFormula()) {
        formula()->addCurlyBracket();
    }
}

void KFormulaDocument::addLineBracket()
{
    if (hasFormula()) {
        formula()->addLineBracket();
    }
}

void KFormulaDocument::addFraction()
{
    if (hasFormula()) {
        formula()->addFraction();
    }
}

void KFormulaDocument::addRoot()
{
    if (hasFormula()) {
        formula()->addRoot();
    }
}

void KFormulaDocument::addIntegral()
{
    if (hasFormula()) {
        formula()->addIntegral();
    }
}

void KFormulaDocument::addProduct()
{
    if (hasFormula()) {
        formula()->addProduct();
    }
}

void KFormulaDocument::addSum()
{
    if (hasFormula()) {
        formula()->addSum();
    }
}

void KFormulaDocument::addMatrix()
{
    if (hasFormula()) {
        formula()->addMatrix();
    }
}

void KFormulaDocument::addOneByTwoMatrix()
{
    if (hasFormula()) {
	formula()->addOneByTwoMatrix();
    }
}


void KFormulaDocument::addLowerLeftIndex()
{
    if (hasFormula()) {
        formula()->addLowerLeftIndex();
    }
}

void KFormulaDocument::addUpperLeftIndex()
{
    if (hasFormula()) {
        formula()->addUpperLeftIndex();
    }
}

void KFormulaDocument::addLowerRightIndex()
{
    if (hasFormula()) {
        formula()->addLowerRightIndex();
    }
}

void KFormulaDocument::addUpperRightIndex()
{
    if (hasFormula()) {
        formula()->addUpperRightIndex();
    }
}

void KFormulaDocument::addGenericLowerIndex()
{
    if (hasFormula()) {
        formula()->addGenericLowerIndex();
    }
}

void KFormulaDocument::addGenericUpperIndex()
{
    if (hasFormula()) {
        formula()->addGenericUpperIndex();
    }
}

void KFormulaDocument::removeEnclosing()
{
    if (hasFormula()) {
        formula()->replaceElementWithMainChild();
    }
}

void KFormulaDocument::makeGreek()
{
    if (hasFormula()) {
        formula()->makeGreek();
    }
}

void KFormulaDocument::insertSymbol()
{
    if ( hasFormula() && impl->table.contains( impl->selectedName ) ) {
        QChar ch = impl->table.unicode( impl->selectedName );
        if ( ch != QChar::null ) {
            formula()->addText( ch, true );
        }
        else {
            formula()->addText( impl->selectedName );
        }
    }
}

void KFormulaDocument::toggleSyntaxHighlighting()
{
    impl->syntaxHighlighting = impl->syntaxHighlightingAction->isChecked();

    KFormulaContainer* f;
    for (f=impl->formulae.first(); f != 0; f=impl->formulae.next()) {
        // Only to notify all views. We don't expect to get new values.
        f->recalc();
    }
}

void KFormulaDocument::delimiterLeft()
{
    QString left = impl->leftBracket->currentText();
    impl->leftBracketChar = left.at(0).latin1();
}

void KFormulaDocument::delimiterRight()
{
    QString right = impl->rightBracket->currentText();
    impl->rightBracketChar = right.at(0).latin1();
}

void KFormulaDocument::symbolNames()
{
    impl->selectedName = impl->symbolNamesAction->currentText();
}

void KFormulaDocument::undo()
{
    getHistory()->undo();
}

void KFormulaDocument::redo()
{
    getHistory()->redo();
}

bool KFormulaDocument::hasFormula()
{
    return (formula() != 0) && (formula()->getActiveCursor() != 0);
}

KFORMULA_NAMESPACE_END

using namespace KFormula;
#include "kformuladocument.moc"

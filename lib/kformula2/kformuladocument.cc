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

#include <qstringlist.h>

#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kstddirs.h>

#include "kformulacontainer.h"
#include "kformuladocument.h"


KFormulaDocument::KFormulaDocument(KActionCollection* collection,
                                   KCommandHistory* his)
        : leftBracketChar('('), rightBracketChar(')'), formula(0)
{
    KGlobal::dirs()->addResourceType("toolbar", KStandardDirs::kde_default("data") + "kformula/pics/");
    createActions(collection);
    
    if (his == 0) {
        history = new KCommandHistory(collection);
        ownHistory = true;
    }
    else {
        history = his;
        ownHistory = false;
    }
}


KFormulaDocument::KFormulaDocument(KCommandHistory* his)
        : leftBracketChar('('), rightBracketChar(')'), formula(0)
{
    if (his == 0) {
        history = new KCommandHistory;
        ownHistory = true;
    }
    else {
        history = his;
        ownHistory = false;
    }
}


KFormulaDocument::~KFormulaDocument()
{
    if (ownHistory) {
        delete history;
    }
}


KFormulaContainer* KFormulaDocument::createFormula()
{
    return new KFormulaContainer(this);
}


void KFormulaDocument::activate(KFormulaContainer* f)
{
    formula = f;
}


void KFormulaDocument::formulaDies(KFormulaContainer* f)
{
    if (f == formula) {
        formula = 0;
    }
}


void KFormulaDocument::createActions(KActionCollection* collection)
{
    addIntegralAction = new KAction(i18n("Add/change to integral"),
                                    "int",
                                    CTRL + Key_6,
                                    this, SLOT(addIntegral()),
                                    collection, "addintegral");
    addSumAction      = new KAction(i18n("Add/change to sum"),
                                    "sum",
                                    CTRL + Key_7,
                                    this, SLOT(addSum()),
                                    collection, "addsum");
    addProductAction  = new KAction(i18n("Add/change to product"),
                                    "prod",
                                    CTRL + Key_4,
                                    this, SLOT(addProduct()),
                                    collection, "addproduct");
    addRootAction     = new KAction(i18n("Add/change to root"),
                                    "sqrt",
                                    CTRL + Key_2,
                                    this, SLOT(addRoot()),
                                    collection, "addroot");
    addFractionAction = new KAction(i18n("Add/change to fraction"),
                                    "frac",
                                    CTRL + Key_3,
                                    this, SLOT(addFraction()),
                                    collection, "addfrac");
    addBracketAction  = new KAction(i18n("Add/change to bracket"),
                                    "paren",
                                    CTRL + Key_5,
                                    this, SLOT(addDefaultBracket()),
                                    collection,"addbra");

    addMatrixAction   = new KAction(i18n("Add matrix"),
                                    "matrix",
                                    CTRL + Key_8,
                                    this, SLOT(addMatrix()),
                                    collection, "addmatrix");

    addUpperLeftAction  = new KAction(i18n("Add upper left index"),
                                      "lsup",
                                      0,
                                      this, SLOT(addUpperLeftIndex()),
                                      collection, "addupperleft");
    addLowerLeftAction  = new KAction(i18n("Add lower left index"),
                                      "lsub",
                                      0,
                                      this, SLOT(addLowerLeftIndex()),
                                      collection, "addlowerleft");
    addUpperRightAction = new KAction(i18n("Add upper right index"),
                                      "rsup",
                                      0,
                                      this, SLOT(addUpperRightIndex()),
                                      collection, "addupperright");
    addLowerRightAction = new KAction(i18n("Add lower right index"),
                                      "rsub",
                                      0,
                                      this, SLOT(addLowerRightIndex()),
                                      collection, "addlowerright");

    addGenericUpperAction = new KAction(i18n("Add upper index"),
                                      CTRL + Key_U,
                                      this, SLOT(addGenericUpperIndex()),
                                      collection, "addupperindex");
    addGenericLowerAction = new KAction(i18n("Add lower index"),
                                      CTRL + Key_L,
                                      this, SLOT(addGenericLowerIndex()),
                                      collection, "addlowerindex");

    removeEnclosingAction = new KAction(i18n("Remove enclosing element"),
                                        CTRL + Key_R,
                                        this, SLOT(removeEnclosing()),
                                        collection, "removeenclosing");

    QStringList delimiter;
    delimiter.append(QString("("));
    delimiter.append(QString("["));
    delimiter.append(QString("<"));
    delimiter.append(QString("/"));
    delimiter.append(QString("\\"));
    delimiter.append(QString(")"));
    delimiter.append(QString("]"));
    delimiter.append(QString(">"));
    delimiter.append(QString("|"));
    leftBracket = new KSelectAction(i18n("Left delimiter"),
                                    0, this, SLOT(delimiterLeft()),
                                    collection, "typeleft");
    leftBracket->setItems(delimiter);
    //leftBracket->setCurrentItem(0);

    delimiter.clear();
    delimiter.append(QString(")"));
    delimiter.append(QString("]"));
    delimiter.append(QString(">"));
    delimiter.append(QString("/"));
    delimiter.append(QString("\\"));
    delimiter.append(QString("("));
    delimiter.append(QString("["));
    delimiter.append(QString("<"));
    delimiter.append(QString("|"));
    rightBracket = new KSelectAction(i18n("Right delimiter"),
                                     0, this, SLOT(delimiterRight()),
                                     collection, "typeright");
    rightBracket->setItems(delimiter);
    //rightBracket->setCurrentItem(0);
}


void KFormulaDocument::paste()
{
    if (hasFormula()) {
        formula->paste();
    }
}

void KFormulaDocument::copy()
{
    if (hasFormula()) {
        formula->copy();
    }
}

void KFormulaDocument::cut()
{
    if (hasFormula()) {
        formula->cut();
    }
}

void KFormulaDocument::addDefaultBracket()
{
    if (hasFormula()) {
        formula->addBracket(leftBracketChar, rightBracketChar);
    }
}

void KFormulaDocument::addFraction()
{
    if (hasFormula()) {
        formula->addFraction();
    }
}

void KFormulaDocument::addRoot()
{
    if (hasFormula()) {
        formula->addRoot();
    }
}

void KFormulaDocument::addIntegral()
{
    if (hasFormula()) {
        formula->addIntegral();
    }
}

void KFormulaDocument::addProduct()
{
    if (hasFormula()) {
        formula->addProduct();
    }
}

void KFormulaDocument::addSum()
{
    if (hasFormula()) {
        formula->addSum();
    }
}

void KFormulaDocument::addMatrix()
{
    if (hasFormula()) {
        formula->addMatrix();
    }
}

void KFormulaDocument::addLowerLeftIndex()
{
    if (hasFormula()) {
        formula->addLowerLeftIndex();
    }
}

void KFormulaDocument::addUpperLeftIndex()
{
    if (hasFormula()) {
        formula->addUpperLeftIndex();
    }
}

void KFormulaDocument::addLowerRightIndex()
{
    if (hasFormula()) {
        formula->addLowerRightIndex();
    }
}

void KFormulaDocument::addUpperRightIndex()
{
    if (hasFormula()) {
        formula->addUpperRightIndex();
    }
}

void KFormulaDocument::addGenericLowerIndex()
{
    if (hasFormula()) {
        formula->addGenericLowerIndex();
    }
}

void KFormulaDocument::addGenericUpperIndex()
{
    if (hasFormula()) {
        formula->addGenericUpperIndex();
    }
}

void KFormulaDocument::removeEnclosing()
{
    if (hasFormula()) {
        formula->replaceElementWithMainChild();
    }
}

void KFormulaDocument::delimiterLeft()
{
    QString left = leftBracket->currentText();
    leftBracketChar = left.at(0).latin1();
}

void KFormulaDocument::delimiterRight()
{
    QString right = rightBracket->currentText();
    rightBracketChar = right.at(0).latin1();
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
    return (formula != 0) && (formula->getActiveCursor() != 0);
}

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

#ifndef KFORMULADOCUMENT_H
#define KFORMULADOCUMENT_H

#include <qobject.h>
#include <qstring.h>

#include <kaction.h>
#include <kcommand.h>

class ContextStyle;
class KFormulaContainer;
class KFormulaWidget;
class SymbolTable;


/**
 * A document that can contain a lot of formulas.
 */
class KFormulaDocument : public QObject {
    Q_OBJECT

public:

    /**
     * Creates a formula document with actions.
     *
     * @param collection a place to put the document's actions.
     * @param history the undo stack to use. Creates its own if zero.
     */
    KFormulaDocument(KActionCollection* collection, KCommandHistory* history = 0);

    /**
     * Creates a formula document that doesn't use actions.
     *
     * @param history the undo stack to use. Creates its own if zero.
     */
    KFormulaDocument(KCommandHistory* history = 0);

    ~KFormulaDocument();

    /**
     * @returns the documents context style.
     */
    const ContextStyle& getContextStyle(bool forPrinting = false) const;

    double getXResolution() const;
    double getYResolution() const;

    /**
     * Sets the resolution (factor) to be used to draw the formula.
     * Make sure to recalc all formulas after you called this.
     */
    void setResolution(double zX, double zY);

    /**
     * Creates a new formula. The whole idea of the formula document
     * is to contain formulas.
     *
     * The new formula is not owned by the document so you are responsible
     * to delete in properly.
     */
    KFormulaContainer* createFormula();

    /**
     * Sets a new formula.
     */
    void activate(KFormulaContainer* formula);

    /**
     * Tells that a formula is about to vanish. This must not be
     * the active formula from now on.
     */
    void formulaDies(KFormulaContainer* formula);

    /**
     * @returns our undo stack so the formulas can use it.
     */
    KCommandHistory* getHistory() const;

    void undo();
    void redo();

    /**
     * @returns the documents symbol table
     */
    const SymbolTable& getSymbolTable() const;

    KAction* getAddBracketAction();
    KAction* getAddFractionAction();
    KAction* getAddRootAction();
    KAction* getAddSumAction();
    KAction* getAddProductAction();
    KAction* getAddIntegralAction();
    KAction* getAddMatrixAction();
    KAction* getAddOneByTwoMatrixAction();
    KAction* getAddUpperLeftAction();
    KAction* getAddLowerLeftAction();
    KAction* getAddUpperRightAction();
    KAction* getAddLowerRightAction();
    KAction* getAddGenericUpperAction();
    KAction* getAddGenericLowerAction();
    KAction* getRemoveEnclosingAction();
    KAction* getMakeGreekAction();
    KAction* getInsertSymbolAction();
    KSelectAction* getLeftBracketAction();
    KSelectAction* getRightBracketAction();
    KSelectAction* getSymbolNamesAction();
    KToggleAction* getSyntaxHighlightingAction();

public slots:

    void paste();
    void copy();
    void cut();

    void addDefaultBracket();
    void addSquareBracket();
    void addCurlyBracket();
    void addLineBracket();
    void addFraction();
    void addRoot();
    void addIntegral();
    void addProduct();
    void addSum();
    void addMatrix();
    void addOneByTwoMatrix();
    void addLowerLeftIndex();
    void addUpperLeftIndex();
    void addLowerRightIndex();
    void addUpperRightIndex();
    void addGenericLowerIndex();
    void addGenericUpperIndex();
    void removeEnclosing();
    void makeGreek();
    void insertSymbol();

    void toggleSyntaxHighlighting();
    void delimiterLeft();
    void delimiterRight();
    void symbolNames();

private:

    /**
     * Creates the standard formula actions and puts them into
     * the collection.
     */
    void createActions(KActionCollection* collection);

    /**
     * @returns whether we have a formula that can get requests.
     */
    bool hasFormula();

    struct KFormulaDocument_Impl;
    KFormulaDocument_Impl* impl;

    KFormulaContainer* formula() const;
};

#endif // KFORMULADOCUMENT_H

/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
		 2006 Martin Pfeiffer <hubipete@gmx.net>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef KFORMULAPARTVIEW_H
#define KFORMULAPARTVIEW_H

#include <KoView.h>

class KFormulaPartViewAdaptor;
class KFormulaPartDocument;
class KFormulaCanvas;
class KAction;
class QPaintEvent;
class QFocusEvent;
class QResizeEvent;
class QScrollArea;


/**
 * @short The view class of the KFormulaPart
 * @version
 */
class KFormulaPartView : public KoView
{
Q_OBJECT
public:
    /**
     * The constructor
     * @param _doc a pointer to the associated KFormulaDoc
     * @param _parent a pointer to the view's parent
     */
    KFormulaPartView( KFormulaPartDocument* _doc, QWidget* _parent=0 );

    /// A virtual destructor
    virtual ~KFormulaPartView();

    /**
     * Use this function to obtain the DBUSObject
     * @retur na pointer to the DBUSObject
     */
    virtual KFormulaPartViewAdaptor* dbusObject();

    /**
     * Setup the printer to use
     * @param printer a reference to the KPrinter to use
     */
    virtual void setupPrinter( KPrinter &printer );

    /**
     * Prints the view using the @p printer
     * @param printer the printer to use
     */
    virtual void print( KPrinter &printer );

    /**
     * Use this function to obtain the associated @ref KFormulaPartDocument
     * @return a pointer to the KFormulaPartDocument
     */
    KFormulaPartDocument* document() const;

protected:
    virtual void resizeEvent( QResizeEvent* _ev );
    virtual void focusInEvent( QFocusEvent* );
    virtual void updateReadWrite( bool );

protected slots:
    void configure();
    void cursorChanged(bool visible, bool selecting);
//    void formulaString();
    void sizeSelected( int );
    void slotShowTipOnStart();
    void slotShowTip();

private:
    /// Initialisize all actions
    void setupActions();
    
    /// Enable or disable the actions that modify the formula.
    void setEnabled( bool enabled );

    KFormulaPartViewAdaptor* m_dbus;
    KFormulaPartDocument *m_partDocument;
    KFormulaCanvas* m_formulaCanvas;
    QScrollArea* m_scrollArea;

    KAction* m_cutAction;
    KAction* m_copyAction;
    KAction* m_pasteAction;

    KAction* m_addBracketAction;
    KAction* m_addFractionAction;
    KAction* m_addRootAction;
    KAction* m_addSumAction;
    KAction* m_addProductAction;
    KAction* m_addIntegralAction;
    KAction* m_addMatrixAction;
    KAction* m_addUpperLeftAction;
    KAction* m_addLowerLeftAction;
    KAction* m_addUpperRightAction;
    KAction* m_addLowerRightAction;
    KAction* m_addGenericUpperAction;
    KAction* m_addGenericLowerAction;
    KAction* m_removeEnclosingAction;

    KAction* m_formulaStringAction;
};

#endif

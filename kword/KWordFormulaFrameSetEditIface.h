/* This file is part of the KDE project
   Copyright (C) 2002, Laurent MONTEL <lmontel@mandrakesoft.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KWORD_FORMULAFRAMESETEDIT_IFACE_H
#define KWORD_FORMULAFRAMESETEDIT_IFACE_H

#include <KoDocumentIface.h>
#include <dcopref.h>

#include <QString>
#include <QColor>
class KWFormulaFrameSetEdit;

class KWordFormulaFrameSetEditIface : public DCOPObject
{
    K_DCOP
public:
    KWordFormulaFrameSetEditIface( KWFormulaFrameSetEdit *_frame );

k_dcop:
    virtual void copy();
    virtual void cut();
    virtual void paste();
    virtual void selectAll();

    void addThinSpace();
    void addMediumSpace();
    void addThickSpace();
    void addQuadSpace();
    void addBracket( int left, int right );
    void addParenthesis();
    void addSquareBracket();
    void addCurlyBracket();
    void addLineBracket();
    void addFraction();
    void addRoot();
    void addIntegral();
    void addProduct();
    void addSum();
    void addMatrix();
    void addMatrix( uint rows, uint columns );
    void addOneByTwoMatrix();
    void addNameSequence();
    void addNegThinSpace();

    void addLowerLeftIndex();
    void addUpperLeftIndex();
    void addLowerRightIndex();
    void addUpperRightIndex();
    void addGenericLowerIndex();
    void addGenericUpperIndex();
    void removeEnclosing();
    void makeGreek();
    void insertSymbol( const QString &name );

    void appendColumn();
    void insertColumn();
    void removeColumn();
    void appendRow();
    void insertRow();
    void removeRow();

    void moveLeft( int flag );
    void moveRight( int flag );
    void moveUp( int flag );
    void moveDown( int flag );

    void moveHome( int flag );
    void moveEnd( int flag );

    /** @returns whether the cursor is at the first position. */
    bool isHome() const;

    /** @returns whether the cursor is at the last position. */
    bool isEnd() const;

    void eraseSelection( int direction );
    void addText( const QString &str );

private:
    KWFormulaFrameSetEdit *m_framesetedit;

};

#endif // KWORD_FORMULAFRAMESETEDIT_IFACE_H

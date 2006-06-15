/* This file is part of the KDE project
   Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>

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

#ifndef KFORMULA_VIEW_ADAPTOR_H
#define KFORMULA_VIEW_ADAPTOR_H

#include <QtCore/QObject>
#include <dbus/qdbus.h>

#include <QString>
#include <qstringlist.h>

class KFormulaPartView;

class KformulaViewAdaptor: public QDBusAbstractAdaptor
{
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface", "org.kde.koffice.formula.view")
public:

   KformulaViewAdaptor( KFormulaPartView *view_ );
public Q_SLOTS: // METHODS
    Q_SCRIPTABLE void addThinSpace();
    Q_SCRIPTABLE void addMediumSpace();
    Q_SCRIPTABLE void addThickSpace();
    Q_SCRIPTABLE void addQuadSpace();
    Q_SCRIPTABLE void addBracket( int left, int right );
    Q_SCRIPTABLE void addParenthesis();
    Q_SCRIPTABLE void addSquareBracket();
    Q_SCRIPTABLE void addCurlyBracket();
    Q_SCRIPTABLE void addLineBracket();
    Q_SCRIPTABLE void addFraction();
    Q_SCRIPTABLE void addRoot();
    Q_SCRIPTABLE void addIntegral();
    Q_SCRIPTABLE void addProduct();
    Q_SCRIPTABLE void addSum();
    Q_SCRIPTABLE void addMatrix();
    Q_SCRIPTABLE void addMatrix( uint rows, uint columns );
    Q_SCRIPTABLE void addOneByTwoMatrix();
    Q_SCRIPTABLE void addNameSequence();

    Q_SCRIPTABLE void addLowerLeftIndex();
    Q_SCRIPTABLE void addUpperLeftIndex();
    Q_SCRIPTABLE void addLowerRightIndex();
    Q_SCRIPTABLE void addUpperRightIndex();
    Q_SCRIPTABLE void addGenericLowerIndex();
    Q_SCRIPTABLE void addGenericUpperIndex();
    Q_SCRIPTABLE void removeEnclosing();
    Q_SCRIPTABLE void makeGreek();
    Q_SCRIPTABLE void insertSymbol( QString name );

    Q_SCRIPTABLE void appendColumn();
    Q_SCRIPTABLE void insertColumn();
    Q_SCRIPTABLE void removeColumn();
    Q_SCRIPTABLE void appendRow();
    Q_SCRIPTABLE void insertRow();
    Q_SCRIPTABLE void removeRow();

    Q_SCRIPTABLE void moveLeft( int flag );
    Q_SCRIPTABLE void moveRight( int flag );
    Q_SCRIPTABLE void moveUp( int flag );
    Q_SCRIPTABLE void moveDown( int flag );

    Q_SCRIPTABLE void moveHome( int flag );
    Q_SCRIPTABLE void moveEnd( int flag );

    /** @returns whether the cursor is at the first position. */
    Q_SCRIPTABLE bool isHome() const;

    /** @returns whether the cursor is at the last position. */
    Q_SCRIPTABLE bool isEnd() const;

    Q_SCRIPTABLE void eraseSelection( int direction );
    Q_SCRIPTABLE void addText( QString str );

    Q_SCRIPTABLE QStringList readFormulaString( QString text );

private:
    KFormulaPartView *m_view;
};

#endif

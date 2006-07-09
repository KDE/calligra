/* This file is part of the KDE project
   Copyright (C) 2002 Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

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

#ifndef FORMULASTRING_H
#define FORMULASTRING_H

#include <qvariant.h>
#include <qdialog.h>

class QGridLayout;
class QHBoxLayout;
class QLabel;
class QPushButton;
class QTextEdit;
class QVBoxLayout;

class KFormulaPartView;

class FormulaString : public QDialog
{
    Q_OBJECT

public:
    FormulaString( KFormulaPartView* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~FormulaString();

    QTextEdit* textWidget;
    QPushButton* buttonHelp;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QLabel* position;

protected slots:

    virtual void accept ();
    void helpButtonClicked();

    void cursorPositionChanged ( int para, int pos );

private:
    KFormulaPartView* view;
};

#endif // FORMULASTRING_H

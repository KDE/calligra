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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef FORMULASTRING_H
#define FORMULASTRING_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QPushButton;
class QTextEdit;

class FormulaString : public QDialog
{
    Q_OBJECT

public:
    FormulaString( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~FormulaString();

    QTextEdit* textWidget;
    QPushButton* buttonHelp;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
};

#endif // FORMULASTRING_H

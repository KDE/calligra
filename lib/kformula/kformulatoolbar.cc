/* This file is part of the KDE libraries
    Copyright (C) 1999 Ilya Baran (ibaran@mit.edu)

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

#include "kformulatoolbar.h"
#include <kiconloader.h>
#include <kglobal.h>
#include <kstddirs.h>

using namespace Box;

KFormulaToolBar::KFormulaToolBar(QWidget *parent, const char *name, int _item_size)
  : KToolBar(parent, name, _item_size)
{
  // doesn't matter if we do this twice - KStandardDirs ignores doubled additions
  KGlobal::dirs()->addResourceType("toolbar", KStandardDirs::kde_default("data") + "kformula/pics/");
  insertButton(BarIcon("editcut"), CUT_CHAR, true, "Cut");
  insertButton(BarIcon("editcopy"), COPY_CHAR, true, "Copy");
  insertButton(BarIcon("editpaste"), PASTE_CHAR, true, "Paste");

  insertSeparator();

  insertButton(BarIcon("undo"), UNDO_CHAR, true, "Undo");
  insertButton(BarIcon("redo"), REDO_CHAR, true, "Redo");

  insertSeparator();

  insertButton(BarIcon("rsup"), POWER, true, "Power");
  insertButton(BarIcon("rsub"), SUB, true, "Subscript");
  insertButton(BarIcon("paren"), PAREN, true, "Parentheses");
  insertButton(BarIcon("abs"), ABS, true, "Absolute value");
  insertButton(BarIcon("brackets"), BRACKET, true, "Brackets");
  insertButton(BarIcon("frac"), DIVIDE, true, "Fraction");
  insertButton(BarIcon("sqrt"), SQRT, true, "Root");
  insertButton(BarIcon("int"), INTEGRAL, true, "Integral");
  insertButton(BarIcon("matrix"), MATRIX, true, "Matrix");
  insertButton(BarIcon("lsup"), LSUP, true, "Left superscript");
  insertButton(BarIcon("lsub"), LSUB, true, "Left subscript");
}


void KFormulaToolBar::connectToFormula(KFormulaEdit *formula)
{
  QObject::connect(this, SIGNAL(clicked(int)), formula, SLOT(insertChar(int)));
}

#include "kformulatoolbar.moc"

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

#include "symboltable.h"


SymbolTableEnty::SymbolTableEnty(QString n)
        : name(n)
{
}


SymbolTable::SymbolTable()
{
    entries.setAutoDelete(true);

    // constants
    addEntry("e");

    // functions
    addEntry("lg");
    addEntry("log");
    addEntry("ln");

    addEntry("sin");
    addEntry("cos");
    addEntry("tan");
    addEntry("cot");
    addEntry("arcsin");
    addEntry("arccos");
    addEntry("arctan");

    addEntry("sinh");
    addEntry("cosh");
    addEntry("tanh");
    addEntry("coth");

    addEntry("arg");
    addEntry("det");
    addEntry("dim");
    addEntry("exp");
    addEntry("gcd");
    addEntry("lim");

    addEntry("min");
    addEntry("max");

    // symbols
    addEntry("alpha");
    addEntry("beta");
    addEntry("gamma");
    addEntry("delta");
    addEntry("epsilon");
    addEntry("varepsilon");
    addEntry("zeta");
    addEntry("eta");
    addEntry("theta");
    addEntry("vartheta");
    addEntry("iota");
    addEntry("kappa");
    addEntry("lambda");
    addEntry("mu");
    addEntry("nu");
    addEntry("xi");
    addEntry("pi");
    addEntry("varpi");
    addEntry("rho");
    addEntry("varrho");
    addEntry("sigma");
    addEntry("varsigma");
    addEntry("tau");
    addEntry("upsilon");
    addEntry("phi");
    addEntry("varphi");
    addEntry("chi");
    addEntry("psi");
    addEntry("omega");

    addEntry("Gamma");
    addEntry("Delta");
    addEntry("Theta");
    addEntry("Lambda");
    addEntry("Xi");
    addEntry("Pi");
    addEntry("Sigma");
    addEntry("Upsilon");
    addEntry("Phi");
    addEntry("Psi");
    addEntry("Omega");
}


bool SymbolTable::contains(QString name) const
{
    return entries.find(name) != 0;
}

void SymbolTable::addEntry(QString name)
{
    entries.insert(name, new SymbolTableEnty(name));
}

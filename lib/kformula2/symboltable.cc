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


SymbolTableEnty::SymbolTableEnty(QString n, QChar ch)
        : name(n), symbolChar(ch)
{
}


SymbolTable::SymbolTable()
        : greekLetters("abgdezhqiklmnxpvrstufjcywGDQLXPSUFYW")
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
    addEntry("alpha", 'a');
    addEntry("beta", 'b');
    addEntry("gamma", 'g');
    addEntry("delta", 'd');
    addEntry("epsilon");
    addEntry("varepsilon", 'e');
    addEntry("zeta", 'z');
    addEntry("eta", 'h');
    addEntry("theta", 'q');
    addEntry("vartheta");
    addEntry("iota", 'i');
    addEntry("kappa", 'k');
    addEntry("lambda", 'l');
    addEntry("mu", 'm');
    addEntry("nu", 'n');
    addEntry("xi", 'x');
    addEntry("pi", 'p');
    addEntry("varpi", 'v');
    addEntry("rho", 'r');
    addEntry("varrho");
    addEntry("sigma", 's');
    addEntry("varsigma");
    addEntry("tau", 't');
    addEntry("upsilon", 'u');
    addEntry("phi", 'f');
    addEntry("varphi", 'j');
    addEntry("chi", 'c');
    addEntry("psi", 'y');
    addEntry("omega", 'w');

    addEntry("Gamma", 'G');
    addEntry("Delta", 'D');
    addEntry("Theta", 'Q');
    addEntry("Lambda", 'L');
    addEntry("Xi", 'X');
    addEntry("Pi", 'P');
    addEntry("Sigma", 'S');
    addEntry("Upsilon", 'U');
    addEntry("Phi", 'F');
    addEntry("Psi", 'Y');
    addEntry("Omega", 'W');
}


bool SymbolTable::contains(QString name) const
{
    return entries.find(name) != 0;
}

QChar SymbolTable::getSymbolChar(QString name) const
{
    SymbolTableEnty* entry = entries.find(name);
    if (entry != 0) {
        return entry->getSymbolChar();
    }
    return QChar::null;
}

void SymbolTable::addEntry(QString name, QChar ch)
{
    entries.insert(name, new SymbolTableEnty(name, ch));
}

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


SymbolTableEntry::SymbolTableEntry(QString n, QChar ch)
        : name(n), symbolChar(ch)
{
}


SymbolTable::SymbolTable()
        : greekLetters("abgdezhqiklmnxpvrstufjcywGDQLXPSUFYVW")
{
    entries.setAutoDelete(true);

    // constants
    addEntry("\\e");

    // functions
    addEntry("\\lg");
    addEntry("\\log");
    addEntry("\\ln");

    addEntry("\\sin");
    addEntry("\\cos");
    addEntry("\\tan");
    addEntry("\\cot");
    addEntry("\\arcsin");
    addEntry("\\arccos");
    addEntry("\\arctan");

    addEntry("\\sinh");
    addEntry("\\cosh");
    addEntry("\\tanh");
    addEntry("\\coth");

    addEntry("\\arg");
    addEntry("\\det");
    addEntry("\\dim");
    addEntry("\\exp");
    addEntry("\\gcd");
    addEntry("\\lim");

    addEntry("\\min");
    addEntry("\\max");

    // symbols
    addEntry("\\alpha", 'a');
    addEntry("\\beta", 'b');
    addEntry("\\gamma", 'g');
    addEntry("\\delta", 'd');
    addEntry("\\epsilon");
    addEntry("\\varepsilon", 'e');
    addEntry("\\zeta", 'z');
    addEntry("\\eta", 'h');
    addEntry("\\theta", 'q');
    addEntry("\\vartheta");
    addEntry("\\iota", 'i');
    addEntry("\\kappa", 'k');
    addEntry("\\lambda", 'l');
    addEntry("\\mu", 'm');
    addEntry("\\nu", 'n');
    addEntry("\\xi", 'x');
    addEntry("\\pi", 'p');
    addEntry("\\varpi", 'v');
    addEntry("\\rho", 'r');
    addEntry("\\varrho");
    addEntry("\\sigma", 's');
    addEntry("\\varsigma", 'V');
    addEntry("\\varsigma");
    addEntry("\\tau", 't');
    addEntry("\\upsilon", 'u');
    addEntry("\\phi", 'f');
    addEntry("\\varphi", 'j');
    addEntry("\\chi", 'c');
    addEntry("\\psi", 'y');
    addEntry("\\omega", 'w');

    addEntry("\\Gamma", 'G');
    addEntry("\\Delta", 'D');
    addEntry("\\Theta", 'Q');
    addEntry("\\Lambda", 'L');
    addEntry("\\Xi", 'X');
    addEntry("\\Pi", 'P');
    addEntry("\\Sigma", 'S');
    addEntry("\\Upsilon", 'U');
    addEntry("\\Phi", 'F');
    addEntry("\\Psi", 'Y');
    addEntry("\\Omega", 'W');

    addEntry("\\rightarrow", static_cast<char>(174));
    addEntry("\\leftarrow", static_cast<char>(172));
    addEntry("\\uparrow", static_cast<char>(173));
    addEntry("\\downarrow", static_cast<char>(175));
    addEntry("\\leftrightarrow", static_cast<char>(171));

    addEntry("\\Rightarrow", static_cast<char>(222));
    addEntry("\\Leftarrow", static_cast<char>(220));
    addEntry("\\Uparrow", static_cast<char>(221));
    addEntry("\\Downarrow", static_cast<char>(223));
    addEntry("\\Leftrightarrow", static_cast<char>(219));

    // these should be operators
    addEntry("\\leq", static_cast<char>(163));
    addEntry("\\geq", static_cast<char>(179));
    addEntry("\\neq", static_cast<char>(185));
    addEntry("\\approx", static_cast<char>(187));
    addEntry("\\equiv", static_cast<char>(186));
    addEntry("\\cong", '@');
    addEntry("\\perp", '^');
    addEntry("\\subset", static_cast<char>(204));
    addEntry("\\subseteq", static_cast<char>(205));
    addEntry("\\supset", static_cast<char>(201));
    addEntry("\\supseteq", static_cast<char>(202));
    addEntry("\\in", static_cast<char>(206));
    addEntry("\\ni", static_cast<char>(39));
    addEntry("\\oplus", static_cast<char>(197));
    addEntry("\\otimes", static_cast<char>(196));
    addEntry("\\cap", static_cast<char>(199));
    addEntry("\\cup", static_cast<char>(200));
    addEntry("\\div", static_cast<char>(184));
    addEntry("\\times", static_cast<char>(180));
    addEntry("\\propto", static_cast<char>(181));
    addEntry("\\bullet", static_cast<char>(183));
    addEntry("\\circ", static_cast<char>(176));
    addEntry("\\pm", static_cast<char>(177));

    addEntry("\\infty", static_cast<char>(165));
    addEntry("\\wp", static_cast<char>(195));
    addEntry("\\aleph", static_cast<char>(192));
    addEntry("\\Im", static_cast<char>(193));
    addEntry("\\Re", static_cast<char>(194));
    addEntry("\\ldots", static_cast<char>(188));
    addEntry("\\partial", static_cast<char>(182));
    addEntry("\\clubsuit", static_cast<char>(167));
    addEntry("\\diamondsuit", static_cast<char>(168));
    addEntry("\\heartsuit", static_cast<char>(169));
    addEntry("\\spadesuit", static_cast<char>(170));
    addEntry("\\exists", '$');
    addEntry("\\forall", '"');
    addEntry("\\angle", static_cast<char>(208));
    addEntry("\\nabla", static_cast<char>(209));
    addEntry("\\neg", static_cast<char>(216));
    addEntry("\\vee", static_cast<char>(218));
    addEntry("\\wedge", static_cast<char>(217));
    addEntry("\\Diamond", static_cast<char>(224));
}


bool SymbolTable::contains(QString name) const
{
    return entries.find(name) != 0;
}

QChar SymbolTable::getSymbolChar(QString name) const
{
    SymbolTableEntry* entry = entries.find(name);
    if (entry != 0) {
        return entry->getSymbolChar();
    }
    return QChar::null;
}

QString SymbolTable::getSymbolName(QChar symbol) const
{
    QDictIterator<SymbolTableEntry> it(entries);

    while (it.current()) {
        if (it.current()->getSymbolChar() == symbol) {
            return it.currentKey();
        }
        ++it;
    }
    return "";
}

void SymbolTable::addEntry(QString name, QChar ch)
{
    entries.insert(name, new SymbolTableEntry(name, ch));
}

QStringList SymbolTable::getAllNames() const
{
    QStringList list;

    for (QDictIterator<SymbolTableEntry> iter = entries; iter.current() != 0; ++iter) {
        list.append(iter.current()->getName());
    }
    list.sort();
    return list;
}

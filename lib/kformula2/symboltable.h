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

#ifndef __SYMBOLTABLE_H
#define __SYMBOLTABLE_H

#include <qdict.h>
#include <qstring.h>


/**
 * An entry of our symbol table.
 */
class SymbolTableEnty {
public:
    SymbolTableEnty(QString name);

    QString getName() const { return name; }

    bool matches(QString n) { return name == n; }
    
private:

    QString name;
};


/**
 * The symbol table.
 *
 * It contains all names that are know to the system.
 */
class SymbolTable {
public:

    SymbolTable();

    bool contains(QString name) const;
    
private:

    void addEntry(QString name);
    
    QDict<SymbolTableEnty> entries;
};


#endif // __SYMBOLTABLE_H

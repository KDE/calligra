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

KFORMULA_NAMESPACE_BEGIN


// get the generated table
#include "symbolfontmapping.cc"


CharTableEntry::CharTableEntry( char font, unsigned char ch, CharClass cl )
{
    value = ( cl << 24 ) + ( font << 16 ) + ch;
}


SymbolFontCharTable::SymbolFontCharTable()
    : greek("abgdezhqiklmnxpvrstufjcywGDQLXPSUFYVW")
{
    for ( uint i = 0; symbolFontMap[ i ].unicode != 0; i++ ) {
        table()[ symbolFontMap[ i ].unicode ] =
            CharTableEntry( 0, symbolFontMap[ i ].pos, symbolFontMap[ i ].cl );
        compatibility[ symbolFontMap[ i ].pos ] = symbolFontMap[ i ].unicode;
    }
}


QChar SymbolFontCharTable::unicodeFromSymbolFont( QChar pos ) const
{
    if ( compatibility.contains( pos ) ) {
        return compatibility[ pos ];
    }
    return QChar::null;
}


SymbolTable::SymbolTable()
    : fontTable( 1 )
{
    fontTable.setAutoDelete( true );
    fontTable.insert( 0, new QFont( "symbol", 12, QFont::Normal, false, QFont::AnyCharSet ) );

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

    for ( uint i = 0; symbolFontMap[ i ].unicode != 0; i++ ) {
        if ( symbolFontMap[ i ].latexName != 0 ) {
            addEntry( symbolFontMap[ i ].latexName, symbolFontMap[ i ].unicode );
        }
    }
}


bool SymbolTable::contains(QString name) const
{
    return entries.find(name) != 0;
}

QChar SymbolTable::unicode(QString name) const
{
    SymbolTableEntry* entry = entries.find(name);
    if (entry != 0) {
        return entry->unicode();
    }
    return QChar::null;
}


QString SymbolTable::name(QChar symbol) const
{
    QDictIterator<SymbolTableEntry> it(entries);

    while (it.current()) {
        if (it.current()->unicode() == symbol) {
            return it.currentKey();
        }
        ++it;
    }
    return "";
}


QFont SymbolTable::font( QChar symbol ) const
{
    return *fontTable[ symbolFontCharTable.font( symbol ) ];
}


uchar SymbolTable::character( QChar symbol ) const
{
    return symbolFontCharTable.character( symbol );
}


CharClass SymbolTable::charClass( QChar symbol ) const
{
    return symbolFontCharTable.charClass( symbol );
}


QChar SymbolTable::unicodeFromSymbolFont( QChar pos ) const
{
    return symbolFontCharTable.unicodeFromSymbolFont( pos );
}


QString SymbolTable::greekLetters() const
{
    return symbolFontCharTable.greekLetters();
}


void SymbolTable::addEntry(QString name, QChar ch)
{
    entries.insert( name, new SymbolTableEntry( name, ch ) );
}


QStringList SymbolTable::allNames() const
{
    QStringList list;

    for (QDictIterator<SymbolTableEntry> iter = entries; iter.current() != 0; ++iter) {
        list.append(iter.current()->name());
    }
    list.sort();
    return list;
}

KFORMULA_NAMESPACE_END

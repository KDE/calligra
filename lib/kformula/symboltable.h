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

#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <qdict.h>
#include <qfont.h>
#include <qmap.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qvector.h>

#include "formuladefs.h"

KFORMULA_NAMESPACE_BEGIN


/**
 * font/char/class triples.
 */
class CharTableEntry {
public:

    /**
     * Defaults for all arguments are provided so it can be used in a QMap.
     *
     * @param font a value between 0 and 15 that corresponds with a font
     *             from the @ref FontTable .
     * @param ch the char pos inside that font.
     * @param cl the chars class. That is its type.
     */
    CharTableEntry( char font = 0, unsigned char ch = 0, CharClass cl = ORDINARY );

    char font() const { return static_cast<char>( value >> 16 ); }
    uchar character() const { return static_cast<unsigned char>( value ); }
    CharClass charClass() const { return static_cast<CharClass>( value >> 24 ); }

private:

    int value;
};


/**
 * A table of character information.
 * This is meant to be customizable someday.
 */
class CharTable {
public:

    CharTable() {}

    CharTableEntry operator[]( QChar unicode ) const { return unicodeTable[ unicode ]; }

    char font( QChar unicode ) const { return unicodeTable[ unicode ].font(); }
    uchar character( QChar unicode ) const { return unicodeTable[ unicode ].character(); }
    CharClass charClass( QChar unicode ) const { return unicodeTable[ unicode ].charClass(); }

protected:

    QMap<QChar, CharTableEntry>& table() { return unicodeTable; }

private:

    /**
     * The chars from unicode we have a font for.
     */
    QMap<QChar, CharTableEntry> unicodeTable;
};


/**
 * The table that contains chars from the symbol font.
 * This is the one buildin table.
 */
class SymbolFontCharTable : public CharTable {
public:

    SymbolFontCharTable();

    /**
     * @returns a string with all greek letters.
     */
    QString greekLetters() const { return greek; }

    /**
     * @returns the unicode value of the symbol font char.
     */
    QChar unicodeFromSymbolFont( QChar pos ) const;

private:

    /**
     * symbol font char -> unicode mapping.
     */
    QMap<uchar, QChar> compatibility;

    /**
     * All greek letters that are known.
     */
    QString greek;
};


/**
 * An entry of our symbol table.
 */
class SymbolTableEntry {
public:
    SymbolTableEntry(QString n, QChar unicode = 0) : _name(n), _char(unicode) {}

    QString name() const { return _name; }
    bool matches(QString n) { return _name == n; }
    QChar unicode() const { return _char; }

private:

    QString _name;
    QChar _char;
};


/**
 * The symbol table.
 *
 * It contains all names that are know to the system.
 */
class SymbolTable {
public:

    SymbolTable();

    bool contains( QString name ) const;

    /**
     * @returns the char in the symbol font that belongs to
     * the given name.
     */
    QChar unicode( QString name ) const;

    QString name( QChar symbol ) const;

    QFont font( QChar symbol ) const;
    uchar character( QChar symbol ) const;
    CharClass charClass( QChar symbol ) const;

    /**
     * @returns the unicode value of the symbol font char.
     */
    QChar unicodeFromSymbolFont( QChar pos ) const;

    /**
     * @returns a string with all greek letters.
     */
    QString greekLetters() const;

    /**
     * @returns all known names as strings.
     */
    QStringList allNames() const;

private:

    void addEntry(QString name, QChar ch = QChar::null);

    /**
     * Name -> unicode mapping.
     * Note that there are names that don't have a unicode value
     * and therefore no char.
     */
    QDict<SymbolTableEntry> entries;

    /**
     * Symbol fonts in use.
     * There must not be more than 256 fonts.
     */
    QVector<QFont> fontTable;

    /**
     * The one CharTable that is supported right now.
     */
    SymbolFontCharTable symbolFontCharTable;
};

KFORMULA_NAMESPACE_END

#endif // SYMBOLTABLE_H

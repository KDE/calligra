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

#include <qfont.h>
#include <qmap.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qvaluevector.h>

#include "kformuladefs.h"

class KConfig;

KFORMULA_NAMESPACE_BEGIN

class ContextStyle;

/**
 * What we know about a unicode char. The char value itself
 * is a key inside the symbol table. Here we have the name,
 * the char class and which font to use.
 */
class CharTableEntry {
public:

    /**
     * Defaults for all arguments are provided so it can be used in a QMap.
     */
    CharTableEntry( QString name = "", CharClass cl = ORDINARY );

    char font() const { return static_cast<char>( value >> 16 ); }
    uchar character() const { return static_cast<unsigned char>( value ); }
    CharClass charClass() const { return static_cast<CharClass>( value >> 24 ); }
    QString texName() const { return name; }

    void setFontChar( char f, unsigned char c );

private:

    int value;
    QString name;
};


/**
 * We expect to always have the symbol font.
 */
class SymbolFontHelper {
public:

    SymbolFontHelper();

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
 * The symbol table.
 *
 * It contains all names that are know to the system.
 */
class SymbolTable {
public:

    SymbolTable();

    /**
     * Reads the unicode / font tables.
     */
    void init( ContextStyle* context );

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

    /**
     * Fills the map with (font name, file name) pairs one for
     * each font available.
     */
    void findAvailableFonts( QMap<QString, QString>* fontMap ) const;

    typedef QMap<QChar, CharTableEntry> UnicodeTable;
    typedef QMap<QString, QChar> EntryTable;
    typedef QValueVector<QFont> FontTable;

private:

    void defaultInitUnicode();
    void defaultInitFont();

    /**
     * The chars from unicode.
     */
    UnicodeTable unicodeTable;

    /**
     * Name -> unicode mapping.
     */
    EntryTable entries;

    /**
     * Symbol fonts in use.
     * There must not be more than 256 fonts.
     */
    FontTable fontTable;

    /**
     * Basic symbol font support.
     */
    SymbolFontHelper symbolFontHelper;
};

KFORMULA_NAMESPACE_END

#endif // SYMBOLTABLE_H

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

#include <qfile.h>
#include <qregexp.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qtextstream.h>

#include <kconfig.h>
#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>
#include <kstandarddirs.h>

#include "symboltable.h"
#include "contextstyle.h"


KFORMULA_NAMESPACE_BEGIN

// class ConfigReader {
// public:
//     ConfigReader() {}
//     virtual ~ConfigReader() {}

//     bool read( QFile& file );

// protected:

//     virtual bool parseLine( QString line ) =0;

//     int parseInt( QString s, bool* b=0 );
//     QString parseAssignment( QString s, QString name );
// };

// bool ConfigReader::read( QFile& file )
// {
//     QTextStream stream( &file );
//     QString line;
//     while ( !( line = stream.readLine() ).isNull() ) {
//         //kdDebug( DEBUGID ) << "ConfigReader::read: " << line << endl;
//         int p = line.find( '#' );
//         if ( p > -1 ) {
//             line = line.left( p );
//         }
//         line = line.stripWhiteSpace();
//         if ( line.length() > 0 ) {
//             if ( !parseLine( line ) )
//                 return false;
//         }
//     }
//     return true;
// }

// int ConfigReader::parseInt( QString s, bool* b )
// {
//     s = s.stripWhiteSpace();
//     if ( s.length() == 0 ) {
//         if ( b ) *b = false;
//         return 0;
//     }
//     if ( s.length() == 1 ) {
//         return s.toInt( b );
//     }
//     if ( ( s[0]=='0' )&&( s[1]=='x' ) ) {
//         return s.right( s.length()-2 ).toInt( b, 16 );
//     }
//     if ( s[0]=='0' ) {
//         return s.right( s.length()-1 ).toInt( b, 8 );
//     }
//     return s.toInt( b );
// }

// QString ConfigReader::parseAssignment( QString s, QString name )
// {
//     int i = s.find( '=' );
//     if ( i > -1 ) {
//         if ( s.left( i-1 ).stripWhiteSpace() == name ) {
//             return s.right( s.length()-i-1 ).stripWhiteSpace();
//         }
//     }
//     return QString::null;
// }


// class UnicodeReader : public ConfigReader {
// public:
//     UnicodeReader( QMap<QChar, CharTableEntry>* t, QMap<QString, QChar>* e )
//         : ConfigReader(), table( t ), entries( e ) {}

// protected:

//     virtual bool parseLine( QString line );

// private:
//     QMap<QChar, CharTableEntry>* table;
//     QMap<QString, QChar>* entries;
// };

// bool UnicodeReader::parseLine( QString line )
// {
//     QStringList fields = QStringList::split( ',', line );
//     int id = -1;
//     CharClass cc = ORDINARY;
//     QString name;
//     switch ( QMIN( fields.size(), 3 ) ) {
//     case 3:
//         name = fields[ 2 ].stripWhiteSpace();
//     case 2: {
//         QString tmp = fields[ 1 ].stripWhiteSpace();
//         if ( tmp.upper() == "BINOP" ) {
//             cc = BINOP;
//         }
//         else if ( tmp.upper() == "RELATION" ) {
//             cc = RELATION;
//         }
//     }
//     case 1:
//         id = parseInt( fields[ 0 ] );
//     }
//     if ( id != -1 ) {
//         ( *table )[id] = CharTableEntry( name, cc );
//         if ( name.length() > 0 ) {
//             //( *entries )[ name ] = id;
//             QString i18nName = i18n( name.latin1() );
//             //if ( name != i18nName ) {
//                 ( *entries )[ i18nName ] = id;
//                 //}
//         }
//     }
//     return true;
// }


// class FontReader : public ConfigReader {
// public:
//     FontReader( SymbolTable::UnicodeTable* t, SymbolTable::FontTable* f )
//         : ConfigReader(), table( t ), fontTable( f ) {}

//     bool read( QFile& file, QString fontName );

// protected:

//     virtual bool parseLine( QString line );

// private:
//     SymbolTable::UnicodeTable* table;
//     SymbolTable::FontTable* fontTable;
//     uint index;
// };


// bool FontReader::read( QFile& file, QString fontName )
// {
//     index = fontTable->size();
//     fontTable->push_back( QFont( fontName ) );
//     return ConfigReader::read( file );
// }

// bool FontReader::parseLine( QString line )
// {
//     QStringList fields = QStringList::split( ',', line );
//     if ( fields.size() == 2 ) {
//         bool posOk = false;
//         int pos = parseInt( fields[ 0 ], &posOk );
//         bool idOk = false;
//         int id = parseInt( fields[ 1 ], &idOk );
//         if ( posOk && idOk )
//             ( *table )[id].setFontChar( static_cast<char>( index ), static_cast<uchar>( pos ) );
//     }
//     return true;
// }


// get the generated table
#include "symbolfontmapping.cc"


SymbolFontHelper::SymbolFontHelper()
    : greek("abgdezhqiklmnxpvrstufjcywGDQLXPSUFYVW")
{
    for ( uint i = 0; symbolMap[ i ].unicode != 0; i++ ) {
        compatibility[ symbolMap[ i ].pos ] = symbolMap[ i ].unicode;
    }
}


QChar SymbolFontHelper::unicodeFromSymbolFont( QChar pos ) const
{
    if ( compatibility.contains( pos ) ) {
        return compatibility[ pos.latin1() ];
    }
    return QChar::null;
}


SymbolTable::SymbolTable()
{
}


bool fontAvailable( QString fontName )
{
    QFont f( fontName );
    QStringList fields = QStringList::split( '-', f.rawName() );
    //if ( ( ( fields.size() == 13 ) && ( fields[1].upper() == fontName.upper() ) ) ||
    //     ( ( fields.size() == 3 ) && ( fields[0].upper() == fontName.upper() ) ) ) {
    if ( ( fields.size() > 1 ) &&
         ( ( fields[1].upper() == fontName.upper() ) ||
           ( fields[0].upper() == fontName.upper() ) ) ) {
        return true;
    }
    else {
        kdWarning( DEBUGID ) << "Font '" << fontName << "' not found but '" << f.rawName() << "'." << endl;
        return false;
    }
}


void SymbolTable::init( ContextStyle* /*context*/ )
{
    normalChars.clear();
    boldChars.clear();
    italicChars.clear();
    boldItalicChars.clear();
    entries.clear();
    fontTable.clear();

    NameTable tempNames;
    for ( int i=0; nameTable[i].unicode != 0; ++i ) {
        tempNames[QChar( nameTable[i].unicode )] = nameTable[i].name;
        //kdDebug( DEBUGID ) << k_funcinfo << " " << nameTable[i].name << endl;
    }

    if ( fontAvailable( "symbol" ) ) {
        initFont( symbolMap, "symbol", tempNames, normalChar );
    }

    if ( fontAvailable( "esstixeight" ) ) {
        initFont( esstixeightMap, "esstixeight", tempNames, normalChar );
    }
    if ( fontAvailable( "esstixeleven" ) ) {
        initFont( esstixelevenMap, "esstixeleven", tempNames, boldItalicChar );
    }
    if ( fontAvailable( "esstixfifteen" ) ) {
        initFont( esstixfifteenMap, "esstixfifteen", tempNames, normalChar );
    }
    if ( fontAvailable( "esstixfive" ) ) {
        initFont( esstixfiveMap, "esstixfive", tempNames, normalChar );
    }
    if ( fontAvailable( "esstixfour" ) ) {
        initFont( esstixfourMap, "esstixfour", tempNames, normalChar );
    }
    if ( fontAvailable( "esstixfourteen" ) ) {
        initFont( esstixfourteenMap, "esstixfourteen", tempNames, normalChar );
    }
    if ( fontAvailable( "esstixnine" ) ) {
        initFont( esstixnineMap, "esstixnine", tempNames, italicChar );
    }
    if ( fontAvailable( "esstixone" ) ) {
        initFont( esstixoneMap, "esstixone", tempNames, normalChar );
    }
    if ( m_esstixDelimiter = fontAvailable( "esstixseven" ) ) {
        initFont( esstixsevenMap, "esstixseven", tempNames, normalChar );
    }
    if ( fontAvailable( "esstixseventeen" ) ) {
        initFont( esstixseventeenMap, "esstixseventeen", tempNames, normalChar );
    }
    if ( fontAvailable( "esstixsix" ) ) {
        initFont( esstixsixMap, "esstixsix", tempNames, normalChar );
    }
    if ( fontAvailable( "esstixsixteen" ) ) {
        initFont( esstixsixteenMap, "esstixsixteen", tempNames, normalChar );
    }
    if ( fontAvailable( "esstixten" ) ) {
        initFont( esstixtenMap, "esstixten", tempNames, normalChar );
    }
    if ( fontAvailable( "esstixthirteen" ) ) {
        initFont( esstixthirteenMap, "esstixthirteen", tempNames, normalChar );
    }
    if ( fontAvailable( "esstixthree" ) ) {
        initFont( esstixthreeMap, "esstixthree", tempNames, normalChar );
    }
    if ( fontAvailable( "esstixtwelve" ) ) {
        initFont( esstixtwelveMap, "esstixtwelve", tempNames, boldChar );
    }
    if ( fontAvailable( "esstixtwo" ) ) {
        initFont( esstixtwoMap, "esstixtwo", tempNames, normalChar );
    }
}

void SymbolTable::initFont( const InternFontTable* table,
                            const char* fontname,
                            const NameTable& tempNames,
                            CharStyle style )
{
    uint fontnr = fontTable.size();
    fontTable.push_back( QFont( fontname ) );
    for ( uint i = 0; table[ i ].unicode != 0; ++i ) {
        QChar uc = table[ i ].unicode;
        unicodeTable( style )[ uc ] = CharTableEntry( table[ i ].cl,
                                                      static_cast<char>( fontnr ),
                                                      table[ i ].pos );

        if ( tempNames.contains( uc ) ) {
            entries[ tempNames[uc] ] = uc;
            names[uc] = tempNames[uc];
        }
    }
}


bool SymbolTable::contains(QString name) const
{
    return entries.find( name ) != entries.end();
}

QChar SymbolTable::unicode(QString name) const
{
    return entries[ name ];
}


QString SymbolTable::name( QChar symbol ) const
{
    return names[symbol];
}


QFont SymbolTable::font( QChar symbol, CharStyle style ) const
{
    const UnicodeTable& table = unicodeTable( style );
    if ( table.contains( symbol ) ) {
        char f = table[symbol].font();
        return fontTable[f];
    }
    if ( ( style != normalChar ) && ( style != anyChar ) ) {
        if ( normalChars.contains( symbol ) ) {
            char f = normalChars[symbol].font();
            return fontTable[f];
        }
    }
    return QFont();
}


uchar SymbolTable::character( QChar symbol, CharStyle style ) const
{
    const UnicodeTable& table = unicodeTable( style );
    if ( table.contains( symbol ) ) {
        return table[symbol].character();
    }
    if ( ( style != normalChar ) && ( style != anyChar ) ) {
        if ( normalChars.contains( symbol ) ) {
            return normalChars[symbol].character();
        }
    }
    return 0;
}


CharClass SymbolTable::charClass( QChar symbol, CharStyle style ) const
{
    const UnicodeTable& table = unicodeTable( style );
    if ( table.contains( symbol ) ) {
        return table[symbol].charClass();
    }
    if ( ( style != normalChar ) && ( style != anyChar ) ) {
        if ( normalChars.contains( symbol ) ) {
            return normalChars[symbol].charClass();
        }
    }
    return ORDINARY;
}


QChar SymbolTable::unicodeFromSymbolFont( QChar pos ) const
{
    return symbolFontHelper.unicodeFromSymbolFont( pos );
}


QString SymbolTable::greekLetters() const
{
    return symbolFontHelper.greekLetters();
}


QStringList SymbolTable::allNames() const
{
    QStringList list;

    for ( EntryTable::const_iterator iter = entries.begin();
          iter != entries.end();
          ++iter ) {
        if ( QChar( character( iter.data() ) ) != QChar::null ) {
            list.append( iter.key() );
        }
    }
    list.sort();
    return list;
}


bool SymbolTable::inTable( QChar ch, CharStyle style ) const
{
    if ( style == anyChar ) {
        return normalChars.contains( ch ) ||
            boldChars.contains( ch ) ||
            italicChars.contains( ch ) ||
            boldItalicChars.contains( ch );
    }
    return unicodeTable( style ).contains( ch );
}


SymbolTable::UnicodeTable& SymbolTable::unicodeTable( CharStyle style )
{
    switch ( style ) {
    case boldChar: return boldChars;
    case italicChar: return italicChars;
    case boldItalicChar: return boldItalicChars;
    default: break;
    }
    return normalChars;
}

const SymbolTable::UnicodeTable& SymbolTable::unicodeTable( CharStyle style ) const
{
    switch ( style ) {
    case boldChar: return boldChars;
    case italicChar: return italicChars;
    case boldItalicChar: return boldItalicChars;
    default: break;
    }
    return normalChars;
}


KFORMULA_NAMESPACE_END

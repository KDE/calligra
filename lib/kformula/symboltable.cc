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
#include <kstandarddirs.h>

#include "symboltable.h"
#include "contextstyle.h"


KFORMULA_NAMESPACE_BEGIN

class ConfigReader {
public:
    ConfigReader() {}
    virtual ~ConfigReader() {}

    bool read( QFile& file );

protected:

    virtual bool parseLine( QString line ) =0;

    int parseInt( QString s, bool* b=0 );
    QString parseAssignment( QString s, QString name );
};

bool ConfigReader::read( QFile& file )
{
    QTextStream stream( &file );
    QString line;
    while ( !( line = stream.readLine() ).isNull() ) {
        //kdDebug( DEBUGID ) << "ConfigReader::read: " << line << endl;
        int p = line.find( '#' );
        if ( p > -1 ) {
            line = line.left( p );
        }
        line = line.stripWhiteSpace();
        if ( line.length() > 0 ) {
            if ( !parseLine( line ) )
                return false;
        }
    }
    return true;
}

int ConfigReader::parseInt( QString s, bool* b )
{
    s = s.stripWhiteSpace();
    if ( s.length() == 0 ) {
        if ( b ) *b = false;
        return 0;
    }
    if ( s.length() == 1 ) {
        return s.toInt( b );
    }
    if ( ( s[0]=='0' )&&( s[1]=='x' ) ) {
        return s.right( s.length()-2 ).toInt( b, 16 );
    }
    if ( s[0]=='0' ) {
        return s.right( s.length()-1 ).toInt( b, 8 );
    }
    return s.toInt( b );
}

QString ConfigReader::parseAssignment( QString s, QString name )
{
    int i = s.find( '=' );
    if ( i > -1 ) {
        if ( s.left( i-1 ).stripWhiteSpace() == name ) {
            return s.right( s.length()-i-1 ).stripWhiteSpace();
        }
    }
    return QString::null;
}


class UnicodeReader : public ConfigReader {
public:
    UnicodeReader( QMap<QChar, CharTableEntry>* t, QMap<QString, QChar>* e )
        : ConfigReader(), table( t ), entries( e ) {}

protected:

    virtual bool parseLine( QString line );

private:
    QMap<QChar, CharTableEntry>* table;
    QMap<QString, QChar>* entries;
};

bool UnicodeReader::parseLine( QString line )
{
    QStringList fields = QStringList::split( ',', line );
    int id = -1;
    CharClass cc = ORDINARY;
    QString name;
    switch ( QMIN( fields.size(), 3 ) ) {
    case 3:
        name = fields[ 2 ].stripWhiteSpace();
    case 2: {
        QString tmp = fields[ 1 ].stripWhiteSpace();
        if ( tmp.upper() == "BINOP" ) {
            cc = BINOP;
        }
        else if ( tmp.upper() == "RELATION" ) {
            cc = RELATION;
        }
    }
    case 1:
        id = parseInt( fields[ 0 ] );
    }
    if ( id != -1 ) {
        ( *table )[id] = CharTableEntry( name, cc );
        if ( name.length() > 0 ) {
            ( *entries )[ name ] = id;
        }
    }
    return true;
}


class FontReader : public ConfigReader {
public:
    FontReader( SymbolTable::UnicodeTable* t, SymbolTable::FontTable* f )
        : ConfigReader(), table( t ), fontTable( f ) {}

    bool read( QFile& file, QString fontName );

protected:

    virtual bool parseLine( QString line );

private:
    SymbolTable::UnicodeTable* table;
    SymbolTable::FontTable* fontTable;
    uint index;
};


bool FontReader::read( QFile& file, QString fontName )
{
    index = fontTable->size();
    fontTable->push_back( QFont( fontName ) );
    return ConfigReader::read( file );
}

bool FontReader::parseLine( QString line )
{
    QStringList fields = QStringList::split( ',', line );
    if ( fields.size() == 2 ) {
        bool posOk = false;
        int pos = parseInt( fields[ 0 ], &posOk );
        bool idOk = false;
        int id = parseInt( fields[ 1 ], &idOk );
        if ( posOk && idOk )
            ( *table )[id].setFontChar( static_cast<char>( index ), static_cast<uchar>( pos ) );
    }
    return true;
}


// get the generated table
#include "symbolfontmapping.cc"


CharTableEntry::CharTableEntry( QString n, CharClass cl )
    : name( n )
{
    value = cl << 24;
}

void CharTableEntry::setFontChar( char f, unsigned char c )
{
    value = ( charClass() << 24 ) + ( f << 16 ) + c;
}


SymbolFontHelper::SymbolFontHelper()
    : greek("abgdezhqiklmnxpvrstufjcywGDQLXPSUFYVW")
{
    for ( uint i = 0; symbolFontMap[ i ].unicode != 0; i++ ) {
        compatibility[ symbolFontMap[ i ].pos ] = symbolFontMap[ i ].unicode;
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
        kdDebug( DEBUGID ) << "Font '" << fontName << "' not found but '" << f.rawName() << "'." << endl;
        return false;
    }
}

void SymbolTable::findAvailableFonts( QMap<QString, QString>* fontMap ) const
{
    QStringList fontFiles = KGlobal::dirs()->findAllResources( "data", "kformula/*.font" );
    for ( QStringList::Iterator it = fontFiles.begin(); it != fontFiles.end(); ++it ) {
        QString fileName = *it;
        int sepPos = fileName.findRev( '/' );
        if ( sepPos > -1 ) {
            fileName = fileName.right( fileName.length() - sepPos - 1 );
        }
        QString fontName = fileName.left( fileName.length() - 5 ).replace( QRegExp( "%20" ), " " );
        //kdDebug() << "SymbolTable::init fontName=" << fontName << endl;
        if ( fontAvailable( fontName ) ) {
            ( *fontMap )[fontName] = *it;
        }
    }
}

void SymbolTable::init( ContextStyle* context )
{
    unicodeTable.clear();
    entries.clear();
    fontTable.clear();

    QString filename = KGlobal::dirs()->findResource( "data", "kformula/unicode.tbl" );
    if ( QFile::exists( filename ) ) {
        QFile file( filename );
        if ( file.open( IO_ReadOnly ) ) {
            UnicodeReader reader( &unicodeTable, &entries );
            reader.read( file );
        }
        else {
            kdWarning( DEBUGID ) << "Error opening file '" << filename.latin1() << "'. Using defaults." << endl;
            defaultInitUnicode();
        }
    }
    else {
        kdWarning( DEBUGID ) << "'unicode.tbl' not found. Using defaults." << endl;
        defaultInitUnicode();
    }

    // We expect to have a working symbol font.
    defaultInitFont();

    QMap<QString, QString> availableFonts;
    findAvailableFonts( &availableFonts );

    if ( availableFonts.size() > 0 ) {
        bool anySuccess = false;
        uint len = context->requestedFonts().size();
        for ( uint i = len; i > 0; --i ) {
            QString fontName = context->requestedFonts()[i-1];
            if ( availableFonts.find( fontName ) != availableFonts.end() ) {
                QString fileName = availableFonts[ fontName ];
                //kdDebug() << "SymbolTable::init requested Font=" << *it << " " << fileName << endl;
                QFile file( fileName );
                if ( file.open( IO_ReadOnly ) ) {
                    FontReader reader( &unicodeTable, &fontTable );
                    if ( reader.read( file, fontName ) ) {
                        anySuccess = true;
                    }
                    else {
                        kdWarning( DEBUGID ) << "Reading font file '" << fileName.latin1() << "' failed." << endl;
                    }
                }
                else {
                    kdWarning( DEBUGID ) << "Error opening file '" << fileName.latin1() << "'." << endl;
                }
            }
            else {
                kdWarning( DEBUGID ) << "Font '" << fontName.latin1() << "' not available." << endl;
            }
        }
        if ( !anySuccess ) {
            kdWarning( DEBUGID ) << "No font file read. Using defaults." << endl;
        }
    }
    else {
        kdWarning( DEBUGID ) << "No available fonts found. Using defaults." << endl;
    }
}

void SymbolTable::defaultInitUnicode()
{
    for ( uint i = 0; symbolFontMap[ i ].unicode != 0; i++ ) {
        QString name = symbolFontMap[ i ].latexName;
        unicodeTable[ symbolFontMap[ i ].unicode ] = CharTableEntry( name, symbolFontMap[ i ].cl );
        if ( symbolFontMap[ i ].latexName != 0 ) {
            entries[ name ] = symbolFontMap[ i ].unicode;
        }
    }
}

void SymbolTable::defaultInitFont()
{
    uint index = fontTable.size();
    fontTable.push_back( QFont( "symbol" ) );
    for ( uint i = 0; symbolFontMap[ i ].unicode != 0; i++ ) {
        unicodeTable[ symbolFontMap[ i ].unicode ].setFontChar( index, symbolFontMap[ i ].pos );
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


QString SymbolTable::name(QChar symbol) const
{
    return unicodeTable[symbol].texName();
}


QFont SymbolTable::font( QChar symbol ) const
{
    char f = unicodeTable[symbol].font();
    //kdDebug( DEBUGID ) << "SymbolTable::font " << fontTable.size() << " " << fontTable[f].rawName() << endl;
    return fontTable[f];
}


uchar SymbolTable::character( QChar symbol ) const
{
    return unicodeTable[symbol].character();
}


CharClass SymbolTable::charClass( QChar symbol ) const
{
    return unicodeTable[symbol].charClass();
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

KFORMULA_NAMESPACE_END

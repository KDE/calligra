/* Swinder - Portable library for spreadsheet
   Copyright (C) 2003-2005 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2006,2009 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA
 */
#include "globalssubstreamhandler.h"

#include <map>
#include <vector>

#include "excel.h"
#include "sheet.h"
#include "workbook.h"

namespace Swinder {

class GlobalsSubStreamHandler::Private
{
public:
    Workbook* workbook;

    // version of workbook
    unsigned version;

    // mapping from BOF pos to actual Sheet
    std::map<unsigned,Sheet*> bofMap;

    // for EXTERNBOOK and EXTERNSHEET
    std::vector<UString> externBookTable;
    std::vector<UString> externSheetTable;

    // for NAME and EXTERNNAME
    std::vector<UString> nameTable;

    // password protection flag
    // TODO: password hash for record decryption
    bool passwordProtected;

    // table of font
    std::vector<FontRecord> fontTable;

    // table of format
    std::map<unsigned,UString> formatsTable;

    // shared-string table
    std::vector<UString> stringTable;

    // color table (from Palette record)
    std::vector<Color> colorTable;

    // table of Xformat
    std::vector<XFRecord> xfTable;
};

GlobalsSubStreamHandler::GlobalsSubStreamHandler( Workbook* workbook, unsigned version )
    : d(new Private)
{
    d->workbook = workbook;
    d->version = version;
    d->passwordProtected = false;

    // initialize palette
    static const char *const default_palette[64-8] = { // default palette for all but the first 8 colors
        "#000000", "#ffffff", "#ff0000", "#00ff00", "#0000ff", "#ffff00", "#ff00ff",
        "#00ffff", "#800000", "#008000", "#000080", "#808000", "#800080", "#008080",
        "#c0c0c0", "#808080", "#9999ff", "#993366", "#ffffcc", "#ccffff", "#660066",
        "#ff8080", "#0066cc", "#ccccff", "#000080", "#ff00ff", "#ffff00", "#00ffff",
        "#800080", "#800000", "#008080", "#0000ff", "#00ccff", "#ccffff", "#ccffcc",
        "#ffff99", "#99ccff", "#ff99cc", "#cc99ff", "#ffcc99", "#3366ff", "#33cccc",
        "#99cc00", "#ffcc00", "#ff9900", "#ff6600", "#666699", "#969696", "#003366",
        "#339966", "#003300", "#333300", "#993300", "#993366", "#333399", "#333333",
    };
    for (int i = 0; i < 64-8; i++) {
        d->colorTable.push_back(Color(default_palette[i]));
    }
}

GlobalsSubStreamHandler::~GlobalsSubStreamHandler()
{
    delete d;
}

bool GlobalsSubStreamHandler::passwordProtected() const
{
    return d->passwordProtected;
}

unsigned GlobalsSubStreamHandler::version() const
{
    return d->version;
}

Sheet* GlobalsSubStreamHandler::sheetFromPosition( unsigned position ) const
{
    std::map<unsigned, Sheet*>::iterator iter = d->bofMap.find(position);
    if (iter != d->bofMap.end())
        return iter->second;
    else
        return 0;
}

UString GlobalsSubStreamHandler::stringFromSST( unsigned index ) const
{
    if (index < d->stringTable.size())
        return d->stringTable[index];
    else
        return UString();
}

unsigned GlobalsSubStreamHandler::fontCount() const
{
    return d->fontTable.size();
}

FontRecord GlobalsSubStreamHandler::font( unsigned index ) const
{
    if (index < d->fontTable.size())
        return d->fontTable[index];
    else
        return FontRecord();
}

Color GlobalsSubStreamHandler::color( unsigned index ) const
{
    if (index < d->colorTable.size())
        return d->colorTable[index];
    else
        return Color();
}

unsigned GlobalsSubStreamHandler::xformatCount() const
{
    return d->xfTable.size();
}

XFRecord GlobalsSubStreamHandler::xformat( unsigned index ) const
{
    if (index < d->xfTable.size())
        return d->xfTable[index];
    else
        return XFRecord();
}

UString GlobalsSubStreamHandler::format( unsigned index ) const
{
    if (index < d->formatsTable.size())
        return d->formatsTable[index];
    else
        return UString();
}

const std::vector<UString>& GlobalsSubStreamHandler::externSheets() const
{
    return d->externSheetTable;
}

UString GlobalsSubStreamHandler::nameFromIndex( unsigned index ) const
{
    if (index < d->nameTable.size())
        return d->nameTable[index];
    else
        return UString();
}

void GlobalsSubStreamHandler::handleRecord( Record* record )
{
    if (!record) return;

    unsigned type = record->rtti();
    if (type == BOFRecord::id)
        handleBOF( static_cast<BOFRecord*>( record ) );
    else if (type == BoundSheetRecord::id)
        handleBoundSheet( static_cast<BoundSheetRecord*>( record ) );
    else if (type == ExternBookRecord::id)
        handleExternBook( static_cast<ExternBookRecord*>( record ) );
    else if (type == ExternNameRecord::id)
        handleExternName( static_cast<ExternNameRecord*>( record ) );
    else if (type == ExternSheetRecord::id)
        handleExternSheet( static_cast<ExternSheetRecord*>( record ) );
    else if (type == FilepassRecord::id)
        handleFilepass( static_cast<FilepassRecord*>( record ) );
    else if (type == FormatRecord::id)
        handleFormat( static_cast<FormatRecord*>( record ) );
    else if (type == FontRecord::id)
        handleFont( static_cast<FontRecord*>( record ) );
    else if (type == NameRecord::id)
        handleName( static_cast<NameRecord*>( record ) );
    else if (type == PaletteRecord::id)
        handlePalette( static_cast<PaletteRecord*>( record ) );
    else if (type == SSTRecord::id)
        handleSST( static_cast<SSTRecord*>( record ) );
    else if (type == XFRecord::id)
        handleXF( static_cast<XFRecord*>( record ) );
}

void GlobalsSubStreamHandler::handleBOF( BOFRecord* record )
{
    if (!record) return;

    if (record->type() == BOFRecord::Workbook) {
        d->version = record->version();
    }
}

void GlobalsSubStreamHandler::handleBoundSheet( BoundSheetRecord* record )
{
    if (!record) return;

    // only care for Worksheet, forget everything else
    if (record->type() == BoundSheetRecord::Worksheet) {
        // create a new sheet
        Sheet* sheet = new Sheet( d->workbook );
        sheet->setName( record->sheetName() );
        sheet->setVisible( record->visible() );

        d->workbook->appendSheet( sheet );

        // update bof position map
        unsigned bofPos = record->bofPosition();
        d->bofMap[ bofPos ] = sheet;
    }
}

void GlobalsSubStreamHandler::handleDateMode( DateModeRecord* record )
{
    if (!record) return;

    // FIXME FIXME what to do ??
    if (record->base1904())
        std::cerr << "WARNING: Workbook uses unsupported 1904 Date System " << std::endl;
}

void GlobalsSubStreamHandler::handleExternBook( ExternBookRecord* record )
{
    if (!record) return;

    d->externBookTable.push_back(record->bookName());
}

void GlobalsSubStreamHandler::handleExternName( ExternNameRecord* record )
{
    if (!record) return;

    d->nameTable.push_back(record->externName());
}

void GlobalsSubStreamHandler::handleExternSheet( ExternSheetRecord* record )
{
    if (!record) return;

    d->externSheetTable.resize(record->refCount());

    for (unsigned i = 0; i < record->refCount(); i++) {
        unsigned bookRef = record->bookRef(i);

        UString result;
        if (bookRef >= d->externBookTable.size()) {
            result = UString("Error");
        } else {
            UString book = d->externBookTable[bookRef];
            if (book == "\004") {
                unsigned sheetRef = record->firstSheetRef(i);
                if (sheetRef > d->workbook->sheetCount()) {
                    result = UString("Error");
                } else {
                    result = d->workbook->sheet(sheetRef)->name();
                }
            } else {
                result = UString("Unknown");
            }
        }

        if(result.find(UString(" ")) != -1 || result.find(UString("'")) != -1) {
            // escape string
            UString outp("'");
            for (int idx = 0; idx < result.length(); idx++) {
                if(result[idx] == '\'')
                    outp.append(UString("''"));
                else
                    outp.append(UString(result[idx]));
            }
            result = outp + UString("'");
        }

        d->externSheetTable[i] = result;
    }
}

void GlobalsSubStreamHandler::handleFilepass( FilepassRecord* record )
{
    if (!record ) return;

    d->passwordProtected = true;
}

void GlobalsSubStreamHandler::handleFont( FontRecord* record )
{
    if (!record) return;

    d->fontTable.push_back( *record );

    // font #4 is never used, so add a dummy one
    if (d->fontTable.size() == 4)
        d->fontTable.push_back( FontRecord() );
}

void GlobalsSubStreamHandler::handleFormat( FormatRecord* record )
{
    if (!record) return;

    d->formatsTable[record->index()] = record->formatString();
}

void GlobalsSubStreamHandler::handleName( NameRecord* record )
{
    if (!record) return;

    d->nameTable.push_back( record->definedName() );
}

void GlobalsSubStreamHandler::handlePalette( PaletteRecord* record )
{
    if (!record) return;

    d->colorTable.clear();
    for (unsigned i = 0; i < record->count(); i++)
        d->colorTable.push_back(record->color( i ));
}

void GlobalsSubStreamHandler::handleSST( SSTRecord* record )
{
    if (!record) return;

    d->stringTable.clear();
    for (unsigned i = 0; i < record->count();i++) {
        UString str = record->stringAt(i);
        d->stringTable.push_back(str);
    }
}

void GlobalsSubStreamHandler::handleXF( XFRecord* record )
{
    if (!record) return;

    d->xfTable.push_back(*record);
}

} // namespace Swinder

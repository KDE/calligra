/*
   This file is part of the KDE project
   Copyright (C) 2001 Ewald Snel <ewald@rambo.its.tudelft.nl>
   Copyright (C) 2001 Tomasz Grobelny <grotk@poczta.onet.pl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/

#ifndef __RTFIMPORT_H__
#define __RTFIMPORT_H__

#include <koFilter.h>
#include <qasciidict.h>
#include <qcolor.h>
#include <qcstring.h>
#include <qfile.h>
#include <qfont.h>
#include <qmap.h>
#include <qstring.h>
#include <qvaluelist.h>
#include <qvaluestack.h>
#include <qtextcodec.h>
#include "rtfimport_dom.h"
#include "rtfimport_tokenizer.h"
#include <qptrlist.h>


class RTFImport;


// RTF property (control word table entry)
struct RTFProperty
{
    const char *onlyValidIn;
    const char *name;
    void (RTFImport::*cwproc)( struct RTFProperty * = 0L );
    int offset;		// offset of formatting property
    int value;		// default value
};

// RTF field description
struct RTFField
{
    const char *id;
    int		type;
    int		subtype;
    const char *value;
};

// RTF destination
struct RTFDestination
{
    const char *group;
    const char *name;
    void (RTFImport::*destproc)( RTFProperty * = 0L );
    void *target;
};

// Paragraph border
struct RTFBorder
{
    enum BorderStyle { Solid, Dashes, Dots, DashDot, DashDotDot, None = 16 };

    BorderStyle style;
    int color;
    int width;
    int space;
};

// Tabulator
struct RTFTab
{
    enum TabType { Left, Centered, FlushRight, Decimal };
    enum LeaderType { None, Dots, Hyphens, Underline, ThickLine };

    TabType type;
    LeaderType leader;
    int position;
};

// Font table entry
struct RTFFont
{
    QString name;
    QFont::StyleHint styleHint;
    int fixedPitch;
    int number;
};

// RTF embedded picture
struct RTFPicture
{
    enum PictureType { BMP, WMF, MacPict, EMF, PNG, JPEG };

    QByteArray bits;
    PictureType type;
    int width, height;
    int cropLeft, cropTop, cropRight, cropBottom;
    int desiredWidth, desiredHeight;
    int scalex, scaley;
    int nibble;
    bool scaled;
};

// Paragraph-formatting properties
struct RTFLayout
{
    enum Alignment { Left, Right, Justified, Centered };

    QValueStack<RTFTab> tablist;
    RTFTab tab;
    RTFBorder borders[4];
    RTFBorder *border;
    Alignment alignment;
    int style;
    int firstIndent, leftIndent, rightIndent;
    int spaceBefore, spaceAfter, spaceBetween;
    bool inTable;
    bool keep, keepNext;
    bool pageBB;
};

// Character-formatting properties
struct RTFFormat
{
    enum VertAlign { Normal = 0, SubScript, SuperScript };

    VertAlign vertAlign;
    int font, fontSize, baseline;
    int color, bgcolor;
    int uc;
    bool underline, underlined, bold, italic, strike, striked, hidden, caps;
    bool underlineDash, underlineThick, underlineDot, underlineDashDot, underlineDashDotDot;
};

// Style sheet entry
struct RTFStyle
{
    QCString name;
    RTFFormat format;
    RTFLayout layout;
    int next;
};

// Section-formatting properties
struct RTFSectionLayout
{
    int headerMargin;
    int footerMargin;
    bool titlePage;
};

// Table cell definition
struct RTFTableCell
{
    RTFBorder borders[4];
    int bgcolor;
    int x;
};

// Table-formatting properties
struct RTFTableRow
{
    QValueList<RTFTableCell> cells;
    QValueList<QByteArray> frameSets;
    RTFLayout::Alignment alignment;
    int height;
    int left;
};

// KWord format
struct KWFormat
{
    RTFFormat fmt;
    QByteArray xmldata;
    uint id, pos, len;
};

// RTF rich text state (text and tables)
struct RTFTextState
{
    DomNode node;	// paragraphs
    DomNode cell;	// table cell(s)
    DomNode text;	// plain text (for paragraph or table cell)
    QValueList<KWFormat> formats;
    QValueList<QByteArray> frameSets;
    QValueList<RTFTableRow> rows;
    uint table, length;
};

// RTF group state (formatting properties)
struct RTFGroupState
{
    RTFTableRow tableRow;
    RTFTableCell tableCell;
    RTFFormat format;
    RTFLayout layout;
    RTFSectionLayout section;
    bool brace0;	// '}' will close the current destination
};


class RTFImport : public KoFilter
{
    Q_OBJECT

public:
    RTFImport( KoFilter *parent, const char *name, const QStringList& );

    virtual KoFilter::ConversionStatus convert( const QCString& from, const QCString& to );

    void setCodepage( RTFProperty * );
    void setToggleProperty( RTFProperty * );
    void setFlagProperty( RTFProperty *property );
    void setNumericProperty( RTFProperty *property );
    void setEnumProperty( RTFProperty *property );
    void setBorderProperty( RTFProperty *property );
    void setBorderStyle( RTFProperty *property );
    void setUpProperty( RTFProperty * );
    void setPlainFormatting( RTFProperty * = 0L );
    void setParagraphDefaults( RTFProperty * = 0L );
    void setSectionDefaults( RTFProperty * = 0L );
    void setTableRowDefaults( RTFProperty * = 0L );
    void insertParagraph( RTFProperty * = 0L );
    void insertPageBreak( RTFProperty * );
    void insertTableCell( RTFProperty * );
    void insertTableRow( RTFProperty * = 0L );
    void insertCellDef( RTFProperty * );
    void insertTabDef( RTFProperty * );
    void insertUTF8( int ch );
    void insertSymbol( RTFProperty *property );
    void insertHexSymbol( RTFProperty * );
    void insertUnicodeSymbol( RTFProperty * );
    void parseField( RTFProperty* );
    void parseFldinst( RTFProperty* );
    void parseFldrslt( RTFProperty* );
    void parseFontTable( RTFProperty * );
    void parseFootNote( RTFProperty * );
    void parseStyleSheet( RTFProperty * );
    void parseColorTable( RTFProperty * );
    void parsePicture( RTFProperty * );
    void parseRichText( RTFProperty * );
    void parsePlainText( RTFProperty * );
    void skipGroup( RTFProperty * );
    void changeDestination( RTFProperty *property );

    void resetState();
    void addAnchor( const char *instance );
    void addFormat( DomNode &node, KWFormat &format, RTFFormat *baseFormat );
    void addLayout( DomNode &node, QCString &name, RTFLayout &layout, bool frameBreak );
    void addParagraph( DomNode &node, bool frameBreak );
    void addVariable(DomNode& spec, int type, QCString key, RTFFormat* fmt=0);
    void finishTable();
    void writeOutPart( const char *name, QByteArray &array );


    RTFTokenizer token;
    DomNode frameSets;
    DomNode pictures;
    DomNode author, company, title, doccomm;
    RTFTextState bodyText;
    QPtrList<RTFTextState> footnotes; //list of footnotes
    int fnnum; //number of last footnote
    RTFTextState firstPageHeader, oddPagesHeader, evenPagesHeader;
    RTFTextState firstPageFooter, oddPagesFooter, evenPagesFooter;
    QMap<int,QCString> fontTable;
    QValueList<RTFStyle> styleSheet;
    QValueList<QColor> colorTable;
    QValueStack<RTFGroupState> stateStack;
    QValueStack<RTFDestination> destinationStack;
    RTFGroupState state;
    RTFDestination destination;
    RTFTextState *textState;
    RTFFont font;
    RTFStyle style;
    RTFPicture picture;
    RTFTableCell emptyCell;
    KWFormat kwFormat;
    QAsciiDict<RTFProperty> properties;
    uint table, pictureNumber;

    // Color table and document-formatting properties
    int red, green, blue;
    int paperWidth, paperHeight;
    int leftMargin, topMargin, rightMargin, bottomMargin;
    int defaultTab, defaultFont;
    bool landscape, facingPages;

    //codepage for the document
    QCString codepage;

    // Field support
    QCString fldinst, fldrslt;
    RTFFormat fldfmt;
    int flddst; // support for recursive fields
};

#endif

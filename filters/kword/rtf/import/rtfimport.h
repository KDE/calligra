/*
   This file is part of the KDE project
   Copyright (C) 2001 Ewald Snel <ewald@rambo.its.tudelft.nl>
   Copyright (C) 2001 Tomasz Grobelny <grotk@poczta.onet.pl>
   Copyright (C) 2003, 2004 Nicolas GOUTTE <goutte@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/

// ### FIXME: copyright holders/date

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


class DomNode;
class RTFImport;


/// RTF property (control word table entry)
struct RTFProperty
{
    const char *onlyValidIn;
    const char *name;
    void (RTFImport::*cwproc)( struct RTFProperty * );
    /// offset of formatting property
    int offset;
    /// default value
    int value;
};

/// RTF field description
struct RTFField
{
    const char *id;
    int		type;
    int		subtype;
    const char *value;
};

/// RTF destination
struct RTFDestination
{
    const char *group;
    const char *name;
    void (RTFImport::*destproc)( RTFProperty * );
    void *target;
};

/// Paragraph border
struct RTFBorder
{
    enum BorderStyle { Solid, Dashes, Dots, DashDot, DashDotDot, None = 16 };

    BorderStyle style;
    int color;
    int width;
    int space;
};

/// Tabulator
struct RTFTab
{
    enum TabType { Left, Centered, FlushRight, Decimal };
    enum LeaderType { None, Dots, Hyphens, Underline, ThickLine };

    TabType type;
    LeaderType leader;
    int position;
};

/// Font table entry
struct RTFFont
{
    QString name;
    QFont::StyleHint styleHint;
    int fixedPitch;
    int number;
};

/// RTF embedded picture
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
    /// \\blipuid
    QString identifier;
};

/// Paragraph-formatting properties
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
    int spaceBefore, spaceAfter;
    int spaceBetween; ///< Line spacing
    bool spaceBetweenMultiple; ///< Is the linespacing defined as multiple of line height (true) or by a height in 1/20 pt (false)?
    bool inTable;
    bool keep, keepNext;
    bool pageBB, pageBA;
};

/// Character-formatting properties
struct RTFFormat
{
    enum VertAlign { Normal = 0, SubScript, SuperScript };
    enum Underline { UnderlineNone=0, UnderlineSimple, UnderlineThick, UnderlineDouble, UnderlineWordByWord,
        UnderlineWave, UnderlineDash, UnderlineDot, UnderlineDashDot, UnderlineDashDotDot };

    VertAlign vertAlign;
    Underline underline;
    int font, fontSize, baseline;
    int color, bgcolor, underlinecolor;
    int uc;
    bool bold, italic, strike, striked;
    bool hidden, caps, smallCaps;
};

/// Comparison operator \since 1.4
inline bool operator == ( const RTFFormat& f1, const RTFFormat& f2 )
{
    return f1.vertAlign == f2.vertAlign && f1.underline == f2.underline
        && f1.font == f2.font && f1.fontSize == f2.fontSize && f1.baseline == f2.baseline
        && f1.color == f2.color && f1.bgcolor == f2.color && f1.underlinecolor == f2.underlinecolor
        && f1.uc == f2.uc
        && f1.bold == f2.bold && f1.italic == f2.italic && f1.strike == f2.strike && f1.striked == f2.striked
        && f1.hidden == f2.hidden && f1.caps == f2.caps && f1.smallCaps == f2.smallCaps
        ;
}
/// Comparison operator \since 1.4
inline bool operator != ( const RTFFormat& f1, const RTFFormat& f2 )
{
    return ! ( f1 == f2 );
}

/// Style sheet entry
struct RTFStyle
{
    QString name;
    RTFFormat format;
    RTFLayout layout;
    int next;
};

/// Section-formatting properties
struct RTFSectionLayout
{
    int headerMargin;
    int footerMargin;
    bool titlePage;
};

/// Table cell definition
struct RTFTableCell
{
    RTFBorder borders[4];
    int bgcolor;
    int x;
};

/// Table-formatting properties
struct RTFTableRow
{
    QValueList<RTFTableCell> cells;
    QStringList frameSets;
    RTFLayout::Alignment alignment;
    int height;
    int left;
};

/// KWord format
struct KWFormat
{
    RTFFormat fmt;
    QString xmldata;
    uint id, pos, len;
};

/// RTF rich text state (text and tables)
struct RTFTextState
{
    /// paragraphs
    DomNode node;
    /// table cell(s)
    DomNode cell;
    /// plain text (for paragraph or table cell)
    DomNode text;
    QValueList<KWFormat> formats;
    QStringList frameSets;
    QValueList<RTFTableRow> rows;
    uint table, length;
};

/// RTF group state (formatting properties)
struct RTFGroupState
{
    RTFTableRow tableRow;
    RTFTableCell tableCell;
    RTFFormat format;
    RTFLayout layout;
    RTFSectionLayout section;
    /// '}' will close the current destination
    bool brace0;
    /// Should the group be ignored?
    bool ignoreGroup;
};


class RTFImport : public KoFilter
{
    Q_OBJECT

public:
    RTFImport( KoFilter *parent, const char *name, const QStringList& );

    /**
     * Convert document from RTF to KWord format.
     * @param from the mimetype for RTF
     * @param to the mimetype for KWord
     * @return true if the document was successfully converted
     */
    virtual KoFilter::ConversionStatus convert( const QCString& from, const QCString& to );

    void ignoreKeyword( RTFProperty * );
    void setCodepage( RTFProperty * );
    void setMacCodepage( RTFProperty * );
    void setAnsiCodepage( RTFProperty * );
    void setPcaCodepage( RTFProperty * );
    void setPcCodepage( RTFProperty * );
    void setToggleProperty( RTFProperty * );
    void setFlagProperty( RTFProperty *property );
    void setNumericProperty( RTFProperty *property );
    void setEnumProperty( RTFProperty *property );
    void setUnderlineProperty( RTFProperty *property );
    /**
     * Sets the value of a border property specified by token.
     * @param property the property to set
     */
    void setBorderProperty( RTFProperty *property );
    /**
     * Sets the value of a border color specified by token.
     */
    void setBorderColor( RTFProperty * );
    /**
     * Sets the value of a border property specified by token.
     * @param property the property to set
     */
    void setBorderStyle( RTFProperty *property );
    void setUpProperty( RTFProperty * );
    void setPlainFormatting( RTFProperty * = 0L );
    void setParagraphDefaults( RTFProperty * = 0L );
    void setSectionDefaults( RTFProperty * = 0L );
    void setTableRowDefaults( RTFProperty * = 0L );
    /**
     * Select which border is the current one.
     * @param property the property to set
     */
    void selectLayoutBorder( RTFProperty * property );
    /**
     * Select which border is the current one, in case of a cell
     * @param property the property to set
     */
    void selectLayoutBorderFromCell( RTFProperty * property );
    void insertParagraph( RTFProperty * = 0L );
    void insertPageBreak( RTFProperty * );
    void insertTableCell( RTFProperty * );
    void insertTableRow( RTFProperty * = 0L );
    void insertCellDef( RTFProperty * );
    void insertTabDef( RTFProperty * );
    /**
     * Inserts a single (Unicode) character in UTF8 format.
     * @param ch the character to write to the current destination
     */
    void insertUTF8( int ch );
    /// Insert special character (as plain text).
    void insertSymbol( RTFProperty *property );
    /// Insert special character (hexadecimal escape value).
    void insertHexSymbol( RTFProperty * );
    /// Insert unicode character (keyword \\u).
    void insertUnicodeSymbol( RTFProperty * );
    void insertDateTime( RTFProperty *property );
    void insertPageNumber( RTFProperty * );
    void parseBlipUid( RTFProperty* );
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
    void parseGroup( RTFProperty * );
    void skipGroup( RTFProperty * );
    void changeDestination( RTFProperty *property );

    void resetState();
    void addAnchor( const char *instance );
    void addFormat( DomNode &node, const KWFormat& format, const RTFFormat* baseFormat );
    void addLayout( DomNode &node, const QString &name, const RTFLayout &layout, bool frameBreak );
    void addParagraph( DomNode &node, bool frameBreak );
    void addVariable(const DomNode& spec, int type, const QString& key, const RTFFormat* fmt=0);
    void addImportedPicture( const QString& rawFileName );
    void addDateTime( const QString& format, const bool isDate, RTFFormat& fmt );
    void finishTable();
    void writeOutPart( const char *name, const DomNode &node );


    RTFTokenizer token;
    DomNode frameSets;
    DomNode pictures;
    DomNode author, company, title, doccomm;
    RTFTextState bodyText;
    QPtrList<RTFTextState> footnotes; ///< list of footnotes
    int fnnum; ///< number of last footnote
    RTFTextState firstPageHeader, oddPagesHeader, evenPagesHeader;
    RTFTextState firstPageFooter, oddPagesFooter, evenPagesFooter;
    QMap<int,QString> fontTable;
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
    QAsciiDict<RTFProperty> destinationProperties;
    uint table;
    uint pictureNumber; ///< Picture number; increase *before* use!

    // Color table and document-formatting properties
    int red, green, blue;
    int paperWidth, paperHeight;
    int leftMargin, topMargin, rightMargin, bottomMargin;
    int defaultTab, defaultFont;
    bool landscape, facingPages;

    // Field support
    QCString fldinst, fldrslt;
    RTFFormat fldfmt;
    int flddst; ///< support for recursive fields
    QString inFileName; ///< File name of the source file.
protected:
    QTextCodec* textCodec; ///< currently used QTextCodec by the RTF file
    QTextCodec* utf8TextCodec; ///< QTextCodec for UTF-8 (used in \u)
    QMap<QString,int> debugUnknownKeywords;
    bool m_batch; ///< Should the filter system be in batch mode (i.e. non-interactive)
};

#endif

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

#include <KoFilter.h>
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

struct RTFTextState;

/// RTF destination
struct RTFDestination
{
    const char *group;
    const char *name;
    void (RTFImport::*destproc)( RTFProperty * );
    RTFTextState* target;
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

    /**
     * Skip the keyword, as we do not need to do anything with it
     * (either because it is supported anyway or because we cannot support it.)
     */
    void ignoreKeyword( RTFProperty * );
    /**
     * Set document codepage.
     * @note Mac's code pages > 10000 are not supported
     */
    void setCodepage( RTFProperty * );
    /**
     * Set document codepage to Mac (also known as MacRoman or as Apple Roman)
     */
    void setMacCodepage( RTFProperty * );
    /**
     * Set document codepage to CP1252
     * @note Old RTF files have a \ansi keyword but no \ansicpg keyword
     */
    void setAnsiCodepage( RTFProperty * );
    /**
     * Set document codepage to IBM 850
     */
    void setPcaCodepage( RTFProperty * );
    /**
     * Set document codepage to IBM 435.
     * @note As Qt does not support IBM 435, this is currently approximated as IBM 850
     */
    void setPcCodepage( RTFProperty * );
    /**
     * Sets the value of a boolean RTF property specified by token.
     * @param property the property to set
     * @deprecated not portable, as it needs an out-of-specification use of offsetof
     */
    void setToggleProperty( RTFProperty * );
    /**
     * Sets a boolean RTF property specified by token.
     * @param property the property to set
     * @deprecated not portable, as it needs an out-of-specification use of offsetof
     */
    void setFlagProperty( RTFProperty *property );
    /**
     * Sets the charset.
     * @param property the property to set
     * @deprecated not portable, as it needs an out-of-specification use of offsetof
     */
    void setCharset( RTFProperty *property );
    /**
     * Sets the value of a numeric RTF property specified by token.
     * @param property the property to set
     * @deprecated not portable, as it assumes that an enum is a char
     */
    void setNumericProperty( RTFProperty *property );
    /**
     * Sets an enumeration (flag) RTF property specified by token.
     * @param property the property to set
     * @deprecated not portable, as it assumes that an enum is a char
     */
    void setEnumProperty( RTFProperty *property );
    /**
     * Set font style hint
     * @since 1.4
     */
    void setFontStyleHint( RTFProperty* property );
    /**
     * Set the picture type
     * (BMP, PNG...)
     * @since 1.4
     */
    void setPictureType( RTFProperty* property );
    /**
     * Sets the enumaration value for \\ul-type keywords
     * \\ul switches on simple underline
     * \\ul0 switches off all underlines
     * @since 1.4 (renamed)
     */
    void setSimpleUnderlineProperty( RTFProperty* );
    /**
     * Set underline properties
     * @param property the property to set
     * @since 1.4 (changed behaviour)
     */
    void setUnderlineProperty( RTFProperty* property );
    /**
     * Sets the value of a border property specified by token.
     * @param property the property to set
     */
    void setBorderProperty( RTFProperty *property );
    /**
     * Sets the value of a border color specified by token.
     * @deprecated not portable, as it needs an out-of-specification use of offsetof
     */
    void setBorderColor( RTFProperty * );
    /**
     * Sets the value of a border property specified by token.
     * @param property the property to set
     */
    void setBorderStyle( RTFProperty *property );
    /**
     * Sets the value of the font baseline (superscript).
     */
    void setUpProperty( RTFProperty * );
    /**
     * Reset character-formatting properties.
     */
    void setPlainFormatting( RTFProperty * = 0L );
    /**
     * Reset paragraph-formatting properties
     */
    void setParagraphDefaults( RTFProperty * = 0L );
    /**
     * Reset section-formatting properties.
     */
    void setSectionDefaults( RTFProperty * = 0L );
    /**
     * Reset table-formatting properties.
     */
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
    /**
     * Finish table row and calculate cell borders.
     */
    void insertTableRow( RTFProperty * = 0L );
    /**
     * Inserts a table cell definition.
     */
    void insertCellDef( RTFProperty * );
    /**
     * Inserts a tabulator definition.
     */
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
    /**
     * Insert a date or time field
     */
    void insertDateTime( RTFProperty *property );
    /**
     * Insert a page number field
     */
    void insertPageNumber( RTFProperty * );
    /**
     * Parse the picture identifier
     */
    void parseBlipUid( RTFProperty* );
    /**
     * Parse recursive fields.
     * @note The {\fldrslt ...} group will be used for
     * unsupported and embedded fields.
     */
    void parseField( RTFProperty* );
    void parseFldinst( RTFProperty* );
    void parseFldrslt( RTFProperty* );
    /**
     * Font table destination callback
     */
    void parseFontTable( RTFProperty * );
    /**
     * This function parses footnotes
     * \todo Endnotes
     */
    void parseFootNote( RTFProperty * );
    /**
     * Style sheet destination callback.
     */
    void parseStyleSheet( RTFProperty * );
    /**
     * Color table destination callback.
     */
    void parseColorTable( RTFProperty * );
    /**
     * Picture destination callback.
     */
    void parsePicture( RTFProperty * );
    /**
     * Rich text destination callback.
     */
    void parseRichText( RTFProperty * );
    /**
     * Plain text destination callback.
     */
    void parsePlainText( RTFProperty * );
    /**
     * Do nothing special for this group
     */
    void parseGroup( RTFProperty * );
    /**
     * Discard all tokens until the current group is closed.
     */
    void skipGroup( RTFProperty * );
    /**
     * Change the destination.
     */
    void changeDestination( RTFProperty *property );

    /**
     * Reset formatting properties to their default settings.
     */
    void resetState();
    /**
     * Add anchor to current destination (see KWord DTD).
     * @param instance the frameset number in the document
     */
    void addAnchor( const char *instance );
    /**
     * Add format information to document node.
     * @param node the document node (destination)
     * @param format the format information
     * @param baseFormat the format information is based on this format
     */
    void addFormat( DomNode &node, const KWFormat& format, const RTFFormat* baseFormat );
    /**
     * Add layout information to document node.
     * @param node the document node (destination)
     * @param name the name of the current style
     * @param layout the paragraph layout information
     * @param frameBreak paragraph is always the last in a frame if true
     */
    void addLayout( DomNode &node, const QString &name, const RTFLayout &layout, bool frameBreak );
    /**
     * Add paragraph information to document node.
     * @param node the document node (destination)
     * @param frameBreak paragraph is always the last in a frame if true
     */
    void addParagraph( DomNode &node, bool frameBreak );
    void addVariable(const DomNode& spec, int type, const QString& key, const RTFFormat* fmt=0);
    void addImportedPicture( const QString& rawFileName );
    /**
     *  Add a date/time field and split it for KWord
     * @param format format of the date/time
     * @param isDate is it a date field? (For the default format, if needed)
     */
    void addDateTime( const QString& format, const bool isDate, RTFFormat& fmt );
    /**
     * Finish table and recalculate cell borders.
     */
    void finishTable();
    /**
     * Write out part (file inside the store).
     * @param name the internal name of the part
     * @param node the data to write
     */
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
    /**
     * Dummy text state for destinations without own RTFTextState
     * @note this is mainly to avoid dangling or NULL pointers
     */
    RTFTextState m_dummyTextState;
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

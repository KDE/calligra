/*
   This file is part of the KDE project
   Copyright (C) 2001, 2002, 2004 Nicolas GOUTTE <goutte@kde.org>
   Copyright (c) 2001 IABG mbH. All rights reserved.
                      Contact: Wolf-Michael Bolle <Bolle@IABG.de>

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
 * Boston, MA 02110-1301, USA.
*/

/*
   The classes in this file are based on a class in the old files:
    /home/kde/koffice/filters/kword/ascii/asciiexport.cc
    /home/kde/koffice/filters/kword/docbookexpert/docbookexport.cc

   The old file was copyrighted by
    Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
    Copyright (c) 2000 ID-PRO Deutschland GmbH. All rights reserved.
                       Contact: Wolf-Michael Bolle <Wolf-Michael.Bolle@GMX.de>
*/

/*
   Part of the code is based on code licensed under the terms of the
   GNU Library General Public License version 2:
   Copyright 2001 Michael Johnson <mikej@xnet.com>
*/

#ifndef EXPORTFILTERSSTRUCTURES_H
#define EXPORTFILTERSSTRUCTURES_H

#include <qvaluelist.h>
#include <qstring.h>
#include <qmap.h>
#include <qcolor.h>
#include <qdom.h>
#include <qdatetime.h>

#include <KoPictureKey.h>
#include <koffice_export.h>
/**
 * TextFormatting is a container for data retrieved from the \<FORMAT\> tag
 * and its subtags to be used in the \<PARAGRAPH\> tag.
 */
class TextFormatting
{
    public:
        TextFormatting(): italic (false), underline (false), underlineWord(false),
            strikeout (false), strikeoutWord( false ),
            weight (50), fontSize (0), verticalAlignment (0), missing (true) {}

        TextFormatting(const bool newMissing) : italic (false),
            underline (false), underlineWord(false), strikeout (false), strikeoutWord( false ),
            weight (50), fontSize (0), verticalAlignment (0),
            missing (newMissing) {}

        TextFormatting ( QString f,
                         bool    i,
                         bool    u,
                         bool    s,
                         int     w,
                         int     sz,
                         QColor  fg,
                         QColor  bg,
                         int     v) :
            fontName (f), italic (i), underline (u), underlineWord(false),
            strikeout(s), strikeoutWord( false ),
            weight (w), fontSize (sz), fgColor (fg),
            bgColor (bg), verticalAlignment (v), missing (false) {}

        QString fontName;

        bool    italic;

        bool    underline;       ///< Any underline? (If true, we do not know if simple or double or what else)

        QString    underlineValue; ///< "value" of the underline: single, double, bold-single...
        QString    underlineStyle; ///< underline style: solid, dash, dot dash, ....
        bool       underlineWord;  ///< true if word-by-word (i.e spaces are not underlined)
        QColor     underlineColor; ///< color of the line

        bool    strikeout;             ///< true if strikeout
        QString strikeoutType;         ///< type of strikeout: single, bold, double
        QString strikeoutLineStyle;    ///< type of the strikeout line: solid, dash, dot, ....
	bool 	strikeoutWord;	///< \todo: not finished ###

        int     weight;
        int     fontSize;
        QColor  fgColor;
        QColor  bgColor;
        int     verticalAlignment;

        QString fontAttribute;
        /**
         * Language code
         * @note the test language "xx" in converted to "en_US"
         */
        QString language;

        bool    missing;   ///< true if this TextFormatting object does not correspond to a real \<FORMAT\> element
};


class Picture
{
    public:
        Picture () {}

        KoPictureKey key;      ///< Picture key: where the picture came from
        QString koStoreName;   ///< Picture store name: filename within KWord archive
};

/**
 * For \<FRAME\> Tag.
 * Look at kword.dtd for a complete description
 * of individuals class members
 */
class FrameData
{
public:
    double right;
    double left;
    double top;
    double bottom;
    double minHeight;
    int runaround;
    QString runaroundSide;
    double runaroundGap;
    int autoCreateNewFrame;
    int newFrameBehavior;
    int copy;
    int sheetSide;

    double lWidth, rWidth, tWidth, bWidth;

    QColor lColor;
    QColor rColor;
    QColor tColor;
    QColor bColor;

    int lStyle, rStyle, tStyle, bStyle;

    QColor bkColor;
    int    bkStyle;
    double bleftpt, brightpt, btoppt, bbottompt;
};


class ParaData;

class KOFFICEFILTER_EXPORT TableCell
{
   public:
      TableCell (): col( 0 ), row( 0 ), m_cols( 0 ), m_rows( 0 ), paraList( 0 ) {}

      /// \since 1.4 (changes of parameters)
      TableCell ( int c, int r, int _cols, int _rows, QValueList<ParaData> *p, FrameData &frameData  )
         : col (c), row (r), m_cols( _cols ), m_rows( _rows ), paraList (p), frame (frameData) {}

      ~TableCell ();

      int                   col;
      int                   row;
      int m_cols; ///< \since 1.4
      int m_rows; ///< \since 1.4
      QValueList<ParaData> *paraList;
      FrameData   frame;
};


class Table
{
   public:
      Table () : cols (0) {}

      /// \since 1.4 (change of parameters)
      void addCell ( int c, int r, int _cols, int _rows, QValueList<ParaData> &p, FrameData &frameData );

      int                   cols;
      QValueList<TableCell> cellList;
};


/// This is basically \<FRAMESET\> tag
class FrameAnchor
{
   public:
      FrameAnchor ():
       type(-1) {}

      FrameAnchor ( const KoPictureKey& n  ) : key (n), type (-1) {}

      KoPictureKey key; ///< Picture key
      int     type;
      FrameData   frame;

      Picture picture;
      Table   table;
};

/**
 * Data of variables
 */
class KOFFICEFILTER_EXPORT VariableData
{
public:
    VariableData (): m_type(-1), footnotePara(0) {}
    VariableData ( const QString& text ) : m_text(text), m_type(-1) {}
    VariableData ( const VariableData& other ) :
      m_key(other.m_key), m_text(other.m_text),
      m_type(other.m_type), propertyMap(other.propertyMap),
      footnotePara(other.footnotePara) {}
public:
    /**
     * Set parameters of a LINK element
     */
    void setLink(const QString& linkName, const QString& hrefName);
    QString getLinkName(void) const; ///< Name of link (attribute "linkName" of \<LINK\>)
    QString getHrefName(void) const; ///< Reference of link (attribute "hrefName" of \<LINK\>)
    /**
     * Set parameters of a PGNUM element
     */
    void setPgNum(const QString& subtype, const QString& value);
    bool isPageNumber(void) const;
    bool isPageCount(void) const;
    /**
     * Set parameters of a FIELD element
     */
    void setField(const QString& subtype, const QString& value);
    QString getFieldName(void) const;
    QString getFieldValue(void) const;
    /**
     * Set parameters of a \<FOOTNOTE\> element
     * @since 1.4 (change of parameters)
     */
    void setFootnote( const QString& notetype, const QString& automatic, const QString& value, QValueList<ParaData>* para );
    /**
     * Is the footnote an automatic one?
     * @return true for an automatical foornote, false for a manual footnote
     */
    bool getFootnoteAuto(void) const;
    /// Get the value (counter) of the footnote
    QString getFootnoteValue(void) const;
    /**
     * Get type of footnote
     * @return true for footnote, false for endnote
     * @since 1.4
     */
    bool getFootnoteType( void ) const;
    /// Paragrapgh of the footnote
    QValueList<ParaData>* getFootnotePara(void) const;

    /**
     * Set generic variable data
     * @since 1.3.1, 1.4
     */
    void setGenericData( const QString& key, const QString& data );
    /**
     * Get generic variable data
     * @since 1.3.1, 1.4
     */
    QString getGenericData( const QString& key ) const;

    QString m_key;
    QString m_text;
    int m_type;

protected:
    QMap<QString,QString> propertyMap;
    QValueList<ParaData>* footnotePara;
};

class FormatData
{
public:
    FormatData ()
        : id (-1), pos (-1), len (-1), text(true) {}

    FormatData ( const int p, const int l, const bool missing  )
        : id (1), pos (p), len (l), text(missing) {}

    FormatData ( const int i, const int p, const int l  )
        : id (i), pos (p), len (l), text(false) {}

    FormatData ( const int p, const int l, const FrameAnchor& t )
        : id (6), pos (p), len (l), frameAnchor (t) {}

    FormatData ( const FormatData& other )
        : id(other.id), pos(other.pos), len(other.len),
          text(other.text), frameAnchor(other.frameAnchor), variable(other.variable) {}
public:
    int id;
    int pos;    ///< Start position of text to which this format applies
    int len;

    TextFormatting  text;
    FrameAnchor     frameAnchor;
    VariableData    variable;
};


class ValueListFormatData : public QValueList<FormatData>
{
public:
    ValueListFormatData (void) { }
    virtual ~ValueListFormatData (void) { }
};


/// Counter structure, for LayoutData
class CounterData
{
public:
    CounterData()
        : numbering (NUM_NONE), style (STYLE_NONE), depth(0), start(0), customCharacter(0)
        {}

    enum Numbering
    {
        NUM_LIST    = 0,   ///< Numbered as a list item
        NUM_CHAPTER = 1,   ///< Numbered as a heading
        NUM_NONE    = 2    ///< No counter
    };

    enum Style
    {
        STYLE_NONE         = 0,
        STYLE_NUM          = 1,
        STYLE_ALPHAB_L     = 2,
        STYLE_ALPHAB_U     = 3,
        STYLE_ROM_NUM_L    = 4,
        STYLE_ROM_NUM_U    = 5,
        STYLE_CUSTOMBULLET = 6,
        STYLE_CUSTOM       = 7,
        STYLE_CIRCLEBULLET = 8,
        STYLE_SQUAREBULLET = 9,
        STYLE_DISCBULLET   = 10,
        STYLE_BOXBULLET    = 11
    };

    Numbering numbering;
    Style style;
    /*unsigned*/ int depth;
    int start;
    QString lefttext;
    QString righttext;

    int /*QChar*/ customCharacter;
    QString customFont;
    //QString custom;

    QString text;   ///< text at the time of saving (no formatting info though)
};


/// Border
class BorderData
{
public:
    BorderData() : style(0), width(0.0) {}
public:
    QColor color;
    int style;
    double width;
};

/**
  * Tabulator (see also \<TABULATOR\> element in KWord's DTD)
  */
class KOFFICEFILTER_EXPORT TabulatorData
{
public:

    enum
    {
        TF_NONE       = 0,
        TF_DOT        = 1,
        TF_LINE       = 2,
        TF_DASH       = 3,
        TF_DASHDOT    = 4,
        TF_DASHDOTDOT = 5
    };

    TabulatorData() : m_type(0), m_ptpos(0.0), m_filling(0), m_width(0.0) {}
public:
    bool operator == (const TabulatorData& other) const;
public:
    int m_type;
    double m_ptpos;
    int m_filling;
    double m_width;
};

/**
  * List of tabulators
  */
class TabulatorList : public QValueList<TabulatorData>
{
public:
    TabulatorList(void) {}
    virtual ~TabulatorList(void) {}
};

/// Paragraph layout
class LayoutData
{
public:
    LayoutData():indentFirst(0.0), indentLeft(-1.0), indentRight(-1.0), marginTop(-1.0), marginBottom(-1.0),
        lineSpacingType(10), lineSpacing(0.0), pageBreakBefore(false), pageBreakAfter(false),
        keepLinesTogether(false),
        shadowDistance(0.0), shadowDirection(0)
        { }


    enum { LS_CUSTOM = 0, LS_SINGLE = 10, LS_ONEANDHALF = 15, LS_DOUBLE = 20,
      LS_ATLEAST = 30, LS_MULTIPLE = 40, LS_FIXED = 50 };

    QString     styleName;
    QString     styleFollowing;
    QString     alignment;      ///< left, right, center, justify
    CounterData counter;
    FormatData  formatData;
    double      indentFirst;    ///< indentation correction of first line (can be negative!)
    double      indentLeft;     ///< left indentation (a negative value means invalid)
    double      indentRight;    ///< right indentation (a negative value means invalid)
    double      marginTop;      ///< space before the paragraph  (a negative value means invalid)
    double      marginBottom;   ///< space after the paragraph (a negative value means invalid)

    int         lineSpacingType;///< 0=custom, 10=one line, 15=one and half lines, 20= 2 lines \todo: document the other values
    double      lineSpacing;    ///< Space between lines in pt, if lineSpacingType==0

    bool        pageBreakBefore;
    bool        pageBreakAfter;
    bool        keepLinesTogether;

    double      shadowDistance; ///< distance of \<SHADOW\>
    int         shadowDirection;///< direction of \<SHADOW\>
    QColor      shadowColor;    ///< red, green, blue of \<SHADOW\>
    BorderData  leftBorder;
    BorderData  rightBorder;
    BorderData  topBorder;
    BorderData  bottomBorder;
    TabulatorList tabulatorList; ///< List of tabulators
};


struct ParaData
{
    QString                    text;
    ValueListFormatData        formattingList;
    LayoutData                 layout;
};

struct HeaderFooterData
{
    enum HeaderFooterPage
    {
      PAGE_FIRST,
      PAGE_ODD,
      PAGE_EVEN,
      PAGE_ALL
    };

    HeaderFooterPage page;
    QValueList<ParaData> para;
};

typedef HeaderFooterData HeaderData;
typedef HeaderFooterData FooterData;

/// data for \<FRAMESET\> which holds footnotes
struct FootnoteData
{
    QString frameName;
    QValueList<ParaData> para;
};

/// see \<VARIABLESETTINGS\> in the KWord DTD
class VariableSettingsData
{
public:
    VariableSettingsData () : startingPageNumber (1),
                              displaylink (true),
                              underlinelink (true),
                              displaycomment (true),
                              displayfieldcode (false)
    {
    }

    int startingPageNumber;
    bool displaylink;
    bool underlinelink;
    bool displaycomment;
    bool displayfieldcode;
    QDateTime creationTime;
    QDateTime modificationTime;
    QDateTime printTime;
};

class KWEFDocumentInfo
{
public:
    KWEFDocumentInfo(void) { } // Initiate all QString
public:
    // <ABOUT>
    QString title;
    QString abstract;
    QString keywords;
    QString subject;
    // <AUTHOR>
    QString fullName;
    QString jobTitle;
    QString company;
    QString email;
    QString telephone;
    QString fax;
    QString country;
    QString postalCode;
    QString city;
    QString street;
    QString initial;
    QString position;
    QString telephonework;
};

/// \since 1.4
class Bookmark
{
public:
    Bookmark();
    /// Bookmark name
    QString m_name;
    /// Name of the frameset where the bookmark is
    QString m_frameset;
    /// Number of the paragrapgh where the booksmark starts
    int m_startparag;
    /// Number of the paragrapgh where the booksmark starts
    int m_endparag;
    /// Position where the bookmark starts in the paragraph
    int m_cursorIndexStart;
    /// Position where the bookmark ends in the paragraph
    int m_cursorIndexEnd;
};

// Helper functions

void CreateMissingFormatData ( QString &paraText, ValueListFormatData &paraFormatDataList );

#endif /* EXPORTFILTERSSTRUCTURES_H */

// $Header$

/*
   This file is part of the KDE project
   Copyright (C) 2001, 2002 Nicolas GOUTTE <nicog@snafu.de>
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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
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


// FormatData is a container for data retreived from the FORMAT tag
// and its subtags to be used in the PARAGRAPH tag.

class TextFormatting
{
    public:
        TextFormatting(): italic (false), underline (false),
            underlineIsDouble (false), strikeout (false),
            weight (50), fontSize (0), verticalAlignment (0), missing (true) {}

        TextFormatting(const bool newMissing) : italic (false),
            underline (false), underlineIsDouble (false), strikeout (false),
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
            fontName (f), italic (i), underline (u), underlineIsDouble (false),
            strikeout (s), weight (w), fontSize (sz), fgColor (fg),
            bgColor (bg), verticalAlignment (v), missing (false) {}

        QString fontName;

        bool    italic;

        // As doubled underlined was introduced late in KWord, we must use a compatibe way
        // First with "underline", we check if an underline is wanted or not.
        // Then with "underIsDouble", we check the type of the underline (simple or double.)
        bool    underline;       // underline? (If true, we do not know if simple or double)
        bool    underlineIsDouble; // simple or double underline? (Undefined if "underline" is false)

        bool    strikeout;

        int     weight;
        int     fontSize;
        QColor  fgColor;
        QColor  bgColor;
        int     verticalAlignment;

        bool    missing;   // true if the FormatData does not correspond to a real <FORMAT> element
};


class ParaData;

class TableCell
{
   public:
      TableCell () {}

      TableCell ( int                   c,
                  int                   r,
                  QValueList<ParaData> *p  ) : col (c), row (r), paraList (p) {}

      ~TableCell ();

      int                   col;
      int                   row;
      QValueList<ParaData> *paraList;
};


class Table
{
   public:
      Table () : cols (0) {}

      void addCell ( int                   c,
                     int                   r,
                     QValueList<ParaData> &p  );

      int                   cols;
      QValueList<TableCell> cellList;
};


class Picture
{
    public:
        Picture () {}

        QString key;           // IMAGE/PIXMAPS KEY filename - where the picture came from
        QString koStoreName;   // PIXMAPS KEY name           - filename within kwd archive
};


class FrameAnchor
{
   public:
      FrameAnchor () {}

      FrameAnchor ( const QString& n  ) : name (n), type (-1) {}

      QString name;
      int     type;
      double  top;      // Frame top
      double  bottom;   // frame bottom
      double  left;     // frame left
      double  right;    // frame right

      Picture picture;
      Table   table;
};


class VariableData
{
public:
    VariableData () {}
    VariableData ( const QString& text ) : m_text(text) {}
public:
    /**
     * Set parameters of a <LINK> element
     */
    void setLink(const QString& linkName, const QString& hrefName);
    QString getLinkName(void) const; // Name of link (attribute "linkName" of <LINK>)
    QString getHrefName(void) const; // Reference of link (attribute "hrefName" of <LINK>)
    /**
     * Set paramaeters of a <PGNUM> element
     */
    void setPgNum(const QString& subtype, const QString& value);
    bool isPageNumber(void) const;
    bool isPageCount(void) const;
public:
    QString m_key;
    QString m_text;
    int m_type;
protected:
    QMap<QString,QString> propertyMap;
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
public:
    int id;
    int pos;    // Start of text to which this format applies
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


// Counter structure, for LayoutData
class CounterData
{
public:
    CounterData()
        : numbering (NUM_NONE), style (STYLE_NONE), depth(0), start(0), customCharacter(0)
        {}

    enum Numbering
    {
        NUM_LIST    = 0,   // Numbered as a list item
        NUM_CHAPTER = 1,   // Numbered as a heading
        NUM_NONE    = 2    // No counter
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
        STYLE_DISCBULLET   = 10
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
};


// Border
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
  * Tabulator (see also <TABULATOR> element in KWord's DTD)
  */
class TabulatorData
{
public:
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

// Paragraph layout
class LayoutData
{
public:
    LayoutData():indentFirst(0.0), indentLeft(-1.0), indentRight(-1.0), marginTop(-1.0), marginBottom(-1.0),
        lineSpacingType(10), lineSpacing(0.0), pageBreakBefore(false), pageBreakAfter(false),
        shadowDistance(0.0), shadowDirection(0)
        { }

    QString     styleName;
    QString     styleFollowing;
    QString     alignment;      // left, right, center, justify
    CounterData counter;
    FormatData  formatData;
    double      indentFirst;    // indentation correction of first line (can be negative!)
    double      indentLeft;     // left indentation (a negative value means invalid)
    double      indentRight;    // right indentation (a negative value means invalid)
    double      marginTop;      // space before the paragraph  (a negative value means invalid)
    double      marginBottom;   // space after the paragraph (a negative value means invalid)

    int         lineSpacingType;// 0=custom, 10=one line, 15=one and half lines, 20= 2 lines
    double      lineSpacing;    // Space between lines in pt, if lineSpacingType==0

    bool        pageBreakBefore;
    bool        pageBreakAfter;
    double      shadowDistance; // distance of <SHADOW>
    int         shadowDirection;// direction of <SHADOW>
    QColor      shadowColor;    // red, green, blue of <SHADOW>
    BorderData  leftBorder;
    BorderData  rightBorder;
    BorderData  topBorder;
    BorderData  bottomBorder;
    TabulatorList tabulatorList; // List of tabulators
};


struct ParaData
{
    QString                    text;
    ValueListFormatData        formattingList;
    LayoutData                 layout;
};


class KWEFDocumentInfo
{
public:
    KWEFDocumentInfo(void) { } // Initiate all QString
public:
    // <ABOUT>
    QString title;
    QString abstract;
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
};


// Helper functions

void CreateMissingFormatData ( QString &paraText, ValueListFormatData &paraFormatDataList );

#endif /* EXPORTFILTERSSTRUCTURES_H */

// $Header$

/*
   This file is part of the KDE project
   Copyright (C) 2001 Nicolas GOUTTE <nicog@snafu.de>

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

#ifndef EXPORTFILTERSSTRUCTURES_H
#define EXPORTFILTERSSTRUCTURES_H

#include <qvaluelist.h>
#include <qstring.h>
#include <qcolor.h>
#include <qdom.h>

// FormatData is a container for data retreived from the FORMAT tag
// and its subtags to be used in the PARAGRAPH tag.

class FormatData
/*
   This class is based on a class in the old file:
    /home/kde/koffice/filters/kword/ascii/asciiexport.cc

   The old file was copyrighted by
    Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
    Copyright (c) 2000 ID-PRO Deutschland GmbH. All rights reserved.
                       Contact: Wolf-Michael Bolle <Bolle@ID-PRO.de>
*/
{
    public:
        FormatData ()
        { init(); }
        FormatData ( int p,
                     int l  ) : pos (p), len (l), realLen (l)
        { init(); }

        QString fontName;

        int  pos; // Start of text to which this format applies
        int  len; // Len of text to which this format applies
        int  realLen; //Real length of text (in case "len" is not the truth!)
        // TODO: get rid of realLen!

        int  weight;
        int  fontSize;
        QColor colour;
        QColor textbackgroundColour;
        int  verticalAlignment;

        bool italic;
        bool underline;
        bool strikeout;

        bool missing; // true if the FormatData does not correspond to a real <FORMAT> element
    private:
        void init()
        {
            //Initialize member variables
            // (initialize all variables, even those to 0!)
            weight=0;
            fontSize=-1;
            colour=QColor();
            verticalAlignment=0;
            italic=false;
            underline=false;
            strikeout=false;
            fontName=QString::null;
            missing=false;
        }
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
        NUM_LIST = 0,       // Numbered as a list item.
        NUM_CHAPTER = 1,    // Numbered as a heading.
        NUM_NONE = 2        // No counter.
    };
    enum Style
    {
        STYLE_NONE = 0,
        STYLE_NUM = 1, STYLE_ALPHAB_L = 2, STYLE_ALPHAB_U = 3,
        STYLE_ROM_NUM_L = 4, STYLE_ROM_NUM_U = 5, STYLE_CUSTOMBULLET = 6,
        STYLE_CUSTOM = 7, STYLE_CIRCLEBULLET = 8, STYLE_SQUAREBULLET = 9,
        STYLE_DISCBULLET = 10
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

// Paragraph layout
class LayoutData
{
public:
  LayoutData():indentFirst(0.0), indentLeft(0.0), indentRight(0.0), marginBottom(0.0), marginTop(0.0), lineSpacing(0.0)
      { }

    QString styleName;
    QString alignment;
    CounterData counter;
    FormatData formatData;
    double indentFirst, indentLeft, indentRight, marginBottom, marginTop,lineSpacing;
    bool pageBreakBefore, pageBreakAfter;
    QString abiprops; // AbiWord properties (TODO: get rid of it)
    QString tabulator;
};

// Helper functions

void CreateMissingFormatData(QString &paraText, ValueListFormatData &paraFormatDataList);

#endif /* EXPORTFILTERSSTRUCTURES_H */

/* This file is part of the KDE project
   Copyright (C) 2001 Ariya Hidayat <ariyahidayat@yahoo.de>

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


#ifndef __WP_PARSER_H
#define __WP_PARSER_H

#include <q3ptrlist.h>
#include <q3memarray.h>
#include <QString>

namespace WP
{

/**
 * This class is an abstract encapsulation of a token to be use in the parser.
 * @see Parser
 */

  class Token
  {

  public:

    enum Type
    {
      Unknown, Function, Text, Lang,
      SoftSpace, HardSpace, HardHyphen, ExtChar,
      SoftReturn, HardReturn, DormantHardReturn,
      AttrOn, AttrOff, UnderlineMode,
      MarkTocStart, MarkTocEnd,
      LeftMargin, RightMargin, TopMargin, BottomMargin,
      Linespace, Justification, ParagraphIndent, LeftMarginAdjust,
        RightMarginAdjust,
      TabSet, TabLeft, TabHardFlushRight,
      FontColor, FontSize, FontFace, HighlightOn, HighlightOff,
      TableOn, TableColumn, TableEnd, TableCell, TableRow, TableOff
    };

    enum Attr
    {
      None, ExtraLarge, VeryLarge, Large, Small, Fine,
      Bold, Italic, Underline, DoubleUnderline, Subscript, Superscript,
      StrikedOut, Redline, Shadow, Outline, SmallCaps, Blink
    };

    enum Align
    { Left, Right, Center, Full, All };

    enum TabType
    { LeftTab, CenterTab, RightTab, DecimalTab, VerticalTab };

    class Tab
    {
    public:
      TabType type;
      int pos;
        Tab (TabType t, int p)
      {
        type = t;
        pos = p;
      }
    };

    Token () { m_type = Unknown; }

    Token (Type type) { m_type = type; }

    Token (const QString & text) { m_type = Text; m_text = text;  }

    Token (Type type, Attr attr) { m_type = type;  m_attr = attr; }

    Token (Type type, int value) { m_type = type;  m_value = value; }

    Token (Type type, Align align){ m_type = type;  m_align = align;  }

    Token (Type type, int charset, int charcode)
    {
      m_type = type;
      m_charset = charset;
      m_charcode = charcode;
    }

    Token (Type type, int red, int green, int blue)
    {
      m_type = type;
      m_red = red;
      m_green = green;
      m_blue = blue;
    }

    Token (Type type, QString fontface){ m_type = FontFace; m_fontface = fontface;  }

    Token (const Q3PtrList < Tab > &tabs)
    {
      m_type = TabSet;
      m_tabs = tabs;
    }

    Type type (){ return m_type; }

    int value (){ return m_value; }

    QString text (){ return m_text; }

    Attr attr (){ return m_attr; }

    Align align (){ return m_align; }

    int red (){ return m_red; }
    int green (){ return m_green; }
    int blue (){ return m_blue; }

    QString fontface (){ return m_fontface; }
    int charset (){ return m_charset; }
    int charcode (){ return m_charcode; }
    Q3PtrList < Tab > tabs (){  return m_tabs; }


  private:

    Type m_type;
    int m_value;
    QString m_text;
    Attr m_attr;
    Align m_align;
    int m_red, m_green, m_blue;
    QString m_fontface;
    int m_charset, m_charcode;
    Q3PtrList < Tab > m_tabs;



  };

  class Packet
  {
    public:
      unsigned type, size, pos;
      Q3MemArray<quint8> data;
  };

/**
 * This class implements a functional parser for WordPerfect documents.
 * @author Ariya Hidayat
 * @see Token
 */

  class Parser
  {

  public:
    Parser ();

    /**
     * Parses given filename.
     * @returns TRUE if parsing was successful, otherwise FALSE.
     *
     */
    bool parse (const QString & filename);

    // high byte is major version, 0=WP 5.x, 2=WP 6/7/8
    int version;

    QString docTitle, docAuthor, docAbstract;

    Q3PtrList<Token> tokens;
    Q3PtrList<Packet> packets;

    /**
     * Maps WordPerfect extended character to its Unicode equivalent. Supported character
     * sets at the moment are Multinational (charset 1), Phonetic Symbol (charset 2),
     * Typographic Symbol (charset 4), Iconic Symbol (charset 5),
     * Math/Scientific (charset 6 and 7), Greek (charset 8), Hebrew (harset 9), and
     * Cyrillic (charset 10)
     */
    static unsigned int ExtCharToUnicode (int charset, int charcode);

  protected:

  private:

    void handleTab ( Q3MemArray <quint8> data );

    void parsePacketWP5( const QString & filename );
    void parseDocWP5( const QString & filename, int start );

    void parsePacketWP6( const QString & filename );
    void parseDocWP6( const QString & filename, int start );

  };

}


#endif

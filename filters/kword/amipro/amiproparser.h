/* This file is part of the KDE project
   Copyright (C) 2002 Ariya Hidayat <ariyahidayat@yahoo.de>

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

#ifndef __AMIPROPARSER_H
#define __AMIPROPARSER_H

class QString;
class QStringList;
#include <q3valuelist.h>
#include <qcolor.h>

class AmiPro
{
  public:
    static const float LS_Single, LS_OneAndHalf, LS_Double;
};

class AmiProStyle;

class AmiProFormat
{
  public:
    int pos, len;
    bool bold, italic, underline;
    bool word_underline, double_underline;
    bool subscript, superscript, strikethrough;
    QString fontFamily;
    float fontSize;
    QColor fontColor;
    enum { Left, Right, Center, Justified } align;
    AmiProFormat();
    AmiProFormat( const AmiProFormat& );
    AmiProFormat& operator=( const AmiProFormat& );
    void assign( const AmiProFormat& );
    void applyStyle( const AmiProStyle& );
};

typedef Q3ValueList<AmiProFormat> AmiProFormatList;


class AmiProLayout
{
  public:
    QString name;
    QString fontFamily;
    float fontSize;
    QColor fontColor;
    bool bold, italic, underline;
    bool word_underline, double_underline;
    bool subscript, superscript, strikethrough;
    Qt::AlignmentFlags align;
    float linespace;
    float spaceBefore, spaceAfter;
    AmiProLayout();
    AmiProLayout( const AmiProLayout& );
    AmiProLayout& operator=( const AmiProLayout& );
    void assign( const AmiProLayout& );
    void applyStyle( const AmiProStyle& );
};

class AmiProStyle
{
  public:
    QString name;
    QString fontFamily;
    float fontSize;
    QColor fontColor;
    bool bold, italic, underline;
    bool word_underline, double_underline;
    bool subscript, superscript, strikethrough;
    Qt::AlignmentFlags align;
    float linespace;
    float spaceBefore, spaceAfter;
    AmiProStyle();
    AmiProStyle( const AmiProStyle& );
    AmiProStyle& operator=( const AmiProStyle& );
    void assign( const AmiProStyle& );
};

typedef Q3ValueList<AmiProStyle> AmiProStyleList;

class AmiProListener
{
  public: 
    AmiProListener();
    virtual ~AmiProListener();
    virtual bool doOpenDocument();
    virtual bool doCloseDocument();   
    virtual bool doDefineStyle( const AmiProStyle& style );
    virtual bool doParagraph( const QString& text, AmiProFormatList formatList, 
      AmiProLayout& layout );
};

class AmiProParser
{

  public:
    AmiProParser();
    virtual ~AmiProParser();

    enum { OK, UnknownError, FileError, InvalidFormat } Error;

    bool process( const QString& filename );
    int result(){ return m_result; }

    void setListener( AmiProListener * );

  private:

    int m_result;
    QString m_text;
    AmiProFormat m_currentFormat;
    AmiProFormatList m_formatList;
    AmiProLayout m_layout;
    AmiProStyleList m_styleList;


    AmiProListener *m_listener;

    QString m_currentSection;

    bool setResult( int );
    bool parseParagraph( const QStringList& lines );
    bool parseStyle( const QStringList& line );
    AmiProStyle findStyle( const QString& name );

    bool handleTag( const QString& tag );
    bool processOpenDocument();
    bool processCloseDocument();
    bool processParagraph( const QString& text, AmiProFormatList formatList,
      AmiProLayout& layout );

};

#endif

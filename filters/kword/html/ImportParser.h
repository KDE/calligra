// $Header$

/* This file is part of the KDE project
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

#ifndef IMPORTPARSER_H
#define IMPORTPARSER_H

#include <qmap.h>
#include "ImportTags.h"

class HtmlAttributes : public QMap<QString,QString>
{
public:
    HtmlAttributes(void) { }
    ~HtmlAttributes(void) { }
};

class HtmlParser
{
public:
    HtmlParser(QTextStream& streamIn)
        : m_streamIn(streamIn), m_unchok(false), m_line(1), m_column(0) { }
    virtual ~HtmlParser(void) { }
public:
    bool setEncoding(const QString& strEncoding);
    bool parse(void);
public:
    virtual bool doEmptyElement(const QString& tagName, const HtmlAttributes& attributes);
    virtual bool doStartElement(const QString& tagName, const HtmlAttributes& attributes)=0;
    virtual bool doEndElement(const QString& tagName)=0;
    virtual bool doCharacters(const QString& strChars)=0;
protected:
    inline bool IsWhiteSpace(const QChar& ch)
    {
        return ((ch==" ") || (ch==QChar(9)) || (ch==QChar(10)) || (ch==QChar(13)));
    }
    inline bool atEnd(void) { return m_streamIn.atEnd(); }
    inline ulong getLine(void) { return m_line; }
    inline ulong getColumn(void) { return m_column; }
    QChar getCharacter(void);
    virtual void WriteOut(const QChar& ch)=0;
    virtual void WriteOut(const QString& str)=0;
    void unGetCharacter(const QChar& ch);
    bool parseTag(bool tagClosing);
    void treatMetaTag(const QString& tagName, HtmlAttributes& attributes);
private:
    MapTag m_mapTag;
    QTextStream& m_streamIn;
    QChar m_unch; // unget char
    bool m_unchok; // Is unget active?
    ulong m_line;	// No. of line in text
    ulong m_column; // No. of column in text
};

class HtmlBaseListener : protected HtmlParser
{
public:
    HtmlBaseListener(QTextStream& streamIn) : HtmlParser(streamIn) { }
    virtual ~HtmlBaseListener(void) { }
public: // Undoing the class protection
    inline bool parse(void) { return HtmlParser::parse(); }
    inline bool setEncoding(const QString& strEncoding) { return HtmlParser::setEncoding(strEncoding); }
};

class CharsetParser : protected HtmlParser
{
public:
    CharsetParser(QTextStream& streamIn) : HtmlParser(streamIn) { }
    virtual ~CharsetParser(void) { }
public: // virtual
    virtual bool doStartElement(const QString& tagName, const HtmlAttributes& attributes);
    virtual bool doEndElement(const QString& tagName);
    virtual bool doCharacters(const QString& strChars);
public:
    QString findCharset(void);
protected:
    bool treatMetaTag(const QString& tagName, const HtmlAttributes& attributes);
    virtual void WriteOut(const QChar& ch) { };
    virtual void WriteOut(const QString& str) { };
private:
    QString m_strCharset;
};

#endif // IMPORTPARSER_H

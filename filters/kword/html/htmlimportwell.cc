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

#include <qstring.h>
#include <qtextstream.h>

#include <kdebug.h>

#include "ImportParser.h"
#include "htmlimportwell.h"

class WellListener : public HtmlBaseListener
{
public:
    WellListener(QTextStream& streamIn,QTextStream& streamOut)
        : HtmlBaseListener(streamIn), m_streamOut(streamOut) { }
    virtual ~WellListener(void) { }
public:
    virtual bool doEmptyElement(const QString& tagName, const HtmlAttributes& attributes);
    virtual bool doStartElement(const QString& tagName, const HtmlAttributes& attributes);
    virtual bool doEndElement(const QString& tagName);
    virtual bool doCharacters(const QString& strChars);
protected:
    virtual void WriteOut(const QChar& ch) { m_streamOut << ch; }
    virtual void WriteOut(const QString& str) { m_streamOut << str; }
private:
    QTextStream& m_streamOut;
};

bool WellListener :: doEmptyElement(const QString& tagName, const HtmlAttributes& attributes)
{
    m_streamOut << '<' << tagName;

    HtmlAttributes::ConstIterator it;
    for (it=attributes.begin() ; it!=attributes.end() ; it++)
    {
        // FIXME: problem if it.data(9 contains a quote!
        m_streamOut << " " << it.key() << '=' << '"' << it.data() << '"';
    }

    m_streamOut << " />";

    return true;
}

bool WellListener :: doStartElement(const QString& tagName, const HtmlAttributes& attributes)
{
    m_streamOut << '<' << tagName;

    HtmlAttributes::ConstIterator it;

    for (it=attributes.begin() ; it!=attributes.end() ; it++)
    {
        // FIXME: problem if it.data() contains a quote!
        m_streamOut << " " << it.key() << '=' << '"' << it.data() << '"';
    }

    m_streamOut << '>';

    return true;
}

bool WellListener :: doEndElement(const QString& tagName)
{
    m_streamOut << "</" << tagName << '>';

    return true;
}

bool WellListener :: doCharacters(const QString& strChars)
{
    m_streamOut << strChars;
    return true;
}

bool WellFilter(QTextStream& streamIn, QTextStream& streamOut)
{
    QString strEncoding; // TODO: encoding must be read from file

    WellListener* listener=new WellListener(streamIn,streamOut);

    if (!listener)
    {
        return false;
    }

    listener->setEncoding(strEncoding);

    const bool res=listener->parse();

    delete listener;

    return res;
}

/*
    Copyright (C) 2000, S.R.Haque <shaheedhaque@hotmail.com>.
    This file is part of the KDE project

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

DESCRIPTION

    This file implements a simplified abstraction of Microsoft Word documents.
*/

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <kdebug.h>
#include <msword.h>
#include <properties.h>
#include <qarray.h>

class Document: private MsWord
{
protected:

    // Construction. Invoke with the OLE streams that comprise the Word document.

    Document(
        const U8 *mainStream,
        const U8 *table0Stream,
        const U8 *table1Stream,
        const U8 *dataStream);
    virtual ~Document();

    // Metadata.

    QString getTitle(void) const;
    QString getSubject(void) const;
    QString getAuthor(void) const;
    QString getLastRevisedBy(void) const;

    // Call the parse() function to process the document. The callbacks return
    // the text along with any relevant attributes.

    void parse();

    typedef struct
    {
        U32 start;
        U32 end;
        Properties *values;
    } PropertyRun;

    typedef struct
    {
        PAP baseStyle;
        QArray<PropertyRun> runs;
    } Attributes;

    virtual void gotError(
        const QString &text) = 0;
    virtual void gotParagraph(
        const QString &text, 
        Attributes &style) = 0;
    virtual void gotHeadingParagraph(
        const QString &text, 
        Attributes &style) = 0;
    virtual void gotListParagraph(
        const QString &text, 
        Attributes &style) = 0;
    virtual void gotTableBegin(
        unsigned tableNumber) = 0;
    virtual void gotTableEnd(
        unsigned tableNumber) = 0;
    virtual void gotTableRow(
        unsigned tableNumber,
        unsigned rowNumber,
        const QString texts[],
        const PAP styles[],
        TAP &row) = 0;

private:

    // Error handling and reporting support.

    static const int s_area = 30513;

    unsigned m_tableNumber;
    unsigned m_tableRowNumber;

    // Character property handling.

    void createAttributes(
        const PAP &style,
        const CHPXarray &chpxs,
        Attributes &style);

    // Override the base class functions.

    void gotParagraph(
        const QString &text,
        const PAP &pap,
        const CHPXarray &chpxs);
    void gotHeadingParagraph(
        const QString &text,
        const PAP &pap,
        const CHPXarray &chpxs);
    void gotListParagraph(
        const QString &text,
        const PAP &pap,
        const CHPXarray &chpxs);
    void gotTableBegin();
    void gotTableEnd();
    void gotTableRow(
        const QString texts[],
        const PAP styles[],
        TAP &row);
};
#endif

/* This file is part of the KDE project
   Copyright (C) 1999 Werner Trobin <trobin@kde.org>

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

#ifndef WINWORDDOC_H
#define WINWORDDOC_H

#include <qstring.h>
#include <qdom.h>

#include <properties.h>
#include <myfile.h>
#include <msword.h>

#include <kdebug.h>

class WinWordDoc: private MsWord
{
public:
    WinWordDoc(QDomDocument &part, const myFile &mainStream,
               const myFile &table0Stream, const myFile &table1Stream,
               const myFile &dataStream);
    ~WinWordDoc();

    const bool isOk() const { return m_success; }

    const bool convert();
    const QDomDocument * const part();

private:
    WinWordDoc(const WinWordDoc &);
    const WinWordDoc &operator=(const WinWordDoc &);

    // The conversion is done in multiple passes. Which pass is this?

    enum
    {
        INIT,
        TEXT_PASS,
        TABLE_PASS,
        DONE
    } m_phase;
    bool m_success;

    // Convert from Word text into XML-friendly text.

    void encode(QString &text);

    QString colourType(
        unsigned colour,
        const char *red,
        const char *green,
        const char *blue,
        unsigned defaultColour = 1) const;

    // Convert from Word number format codes to our own number format.

    char numberingType(unsigned nfc) const;

    void gotError(const QString &text);
    void gotParagraph(const QString &text, PAP &style);
    void gotHeadingParagraph(const QString &text, PAP &style);
    void gotListParagraph(const QString &text, PAP &style);
    void gotTableBegin();
    void gotTableEnd();
    void gotTableRow(const QString texts[], const PAP styles[], TAP &row);

    QDomDocument m_part;
    unsigned m_tableManager;
    unsigned m_tableRow;
    unsigned m_tableRows;

    // Since there is no way to fill m_part incrementally with XML content,
    // we will fill m_body instead.

    QString m_body;

    // Page sizes, margins etc. all in points.

    static const unsigned s_height = 841; // Height.
    static const unsigned s_width = 595;  // Width.
    static const unsigned s_hMargin = 28; // Horizontal margin.
    static const unsigned s_vMargin = 42; // Vertical margin.
};
#endif // WINWORDDOC_H


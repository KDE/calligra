/* This file is part of the KDE project
   Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>
   Copyright (c) 2003 Lukas Tinkl <lukas@kde.org>
   Copyright (c) 2003 David Faure <faure@kde.org>

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

#ifndef OOUTILS_H
#define OOUTILS_H

#include <qstring.h>
#include <KoFilterChain.h>
class QColor;
class QDomElement;
class KoStyleStack;
class QDomDocument;
class KZip;

class ooNS {
public:
    static const char* const office;
    static const char* const style;
    static const char* const text;
    static const char* const table;
    static const char* const draw;
    static const char* const presentation;
    static const char* const fo;
    static const char* const xlink;
    static const char* const number;
    static const char* const svg;
    static const char* const dc;
    static const char* const meta;
    static const char* const config;
};

namespace OoUtils
{
    QString expandWhitespace(const QDomElement& tag);

    bool parseBorder(const QString & tag, double * width, int * style, QColor * color);

    ///////// Paragraph properties /////////

    // Convert fo:margin-left, fo:margin-right and fo:text-indent to <INDENTS>
    void importIndents( QDomElement& parentElement, const KoStyleStack& styleStack );

    // Convert fo:line-height, style:line-height-at-least and style:line-spacing to <LINESPACING>
    void importLineSpacing( QDomElement& parentElement, const KoStyleStack& styleStack );

    // Convert fo:margin-top and fo:margin-bottom to <OFFSETS>
    void importTopBottomMargin( QDomElement& parentElement, const KoStyleStack& styleStack );

    // Convert style:tab-stops to <TABULATORS>
    void importTabulators( QDomElement& parentElement, const KoStyleStack& styleStack );

    // Convert fo:border* to <*BORDER>
    void importBorders( QDomElement& parentElement, const KoStyleStack& styleStack );

    /////////// Text properties ///////////

    // From style:text-underline to kword/kpresenter's underline/underlinestyleline
    void importUnderline( const QString& text_underline, QString& underline, QString& styleline );

    // From style:text-position (for subscript/superscript)
    // to kword/kpresenter's value/relativetextsize
    void importTextPosition( const QString& text_position, QString& value, QString& relativetextsize );

    void createDocumentInfo(QDomDocument &_meta, QDomDocument & docinfo);
    KoFilter::ConversionStatus loadAndParse(const QString& filename, QDomDocument& doc, KZip * m_zip);
    KoFilter::ConversionStatus loadAndParse(const QString& filename, QDomDocument& doc, KoStore *m_store );

    /// Load an OASIS thumbnail
    KoFilter::ConversionStatus loadThumbnail( QImage& thumbnail, KZip * m_zip );

    // Internal
    KoFilter::ConversionStatus loadAndParse(QIODevice* io, QDomDocument& doc, const QString & fileName);
}

#endif /* OOUTILS_H */

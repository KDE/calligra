/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002 Laurent Montel <lmontel@mandrakesoft.com>
   SPDX-FileCopyrightText: 2003 Lukas Tinkl <lukas@kde.org>
   SPDX-FileCopyrightText: 2003 David Faure <faure@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef OOUTILS_H
#define OOUTILS_H

#include <KoFilter.h>
#include <KoXmlReader.h>
#include <QString>
class QColor;
class QDomElement;
class KoStyleStack;
class QDomDocument;
class KZip;
class KoStore;
class ooNS
{
public:
    static const char office[];
    static const char style[];
    static const char text[];
    static const char table[];
    static const char draw[];
    static const char presentation[];
    static const char fo[];
    static const char xlink[];
    static const char number[];
    static const char svg[];
    static const char dc[];
    static const char meta[];
    static const char config[];
};

namespace OoUtils
{
QString expandWhitespace(const KoXmlElement &tag);

bool parseBorder(const QString &tag, double *width, int *style, QColor *color);

///////// Paragraph properties /////////

// Convert fo:margin-left, fo:margin-right and fo:text-indent to <INDENTS>
void importIndents(QDomElement &parentElement, const KoStyleStack &styleStack);

// Convert fo:line-height, style:line-height-at-least and style:line-spacing to <LINESPACING>
void importLineSpacing(QDomElement &parentElement, const KoStyleStack &styleStack);

// Convert fo:margin-top and fo:margin-bottom to <OFFSETS>
void importTopBottomMargin(QDomElement &parentElement, const KoStyleStack &styleStack);

// Convert style:tab-stops to <TABULATORS>
void importTabulators(QDomElement &parentElement, const KoStyleStack &styleStack);

// Convert fo:border* to <*BORDER>
void importBorders(QDomElement &parentElement, const KoStyleStack &styleStack);

/////////// Text properties ///////////

// From style:text-underline to Words/Stage's underline/underlinestyleline
void importUnderline(const QString &text_underline, QString &underline, QString &styleline);

// From style:text-position (for subscript/superscript)
// to Words/Stage's value/relativetextsize
void importTextPosition(const QString &text_position, QString &value, QString &relativetextsize);

void createDocumentInfo(KoXmlDocument &_meta, QDomDocument &docinfo);
KoFilter::ConversionStatus loadAndParse(const QString &filename, KoXmlDocument &doc, KZip *zip);
KoFilter::ConversionStatus loadAndParse(const QString &filename, KoXmlDocument &doc, KoStore *store);

/// Load an OASIS thumbnail
KoFilter::ConversionStatus loadThumbnail(QImage &thumbnail, KZip *zip);

// Internal
KoFilter::ConversionStatus loadAndParse(QIODevice *io, KoXmlDocument &doc, const QString &fileName);
}

#endif /* OOUTILS_H */

/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
   SPDX-FileCopyrightText: 2002, 2003 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2003 Nicolas GOUTTE <goutte@kde.org>
   SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoPageFormat.h"
#include <QLocale>

#include <KLazyLocalizedString>
#include <KLocalizedString>
#include <OdfDebug.h>

// paper formats ( mm )
#define PG_A3_WIDTH 297.0
#define PG_A3_HEIGHT 420.0
#define PG_A4_WIDTH 210.0
#define PG_A4_HEIGHT 297.0
#define PG_A5_WIDTH 148.0
#define PG_A5_HEIGHT 210.0
#define PG_B5_WIDTH 182.0
#define PG_B5_HEIGHT 257.0
#define PG_US_LETTER_WIDTH 216.0
#define PG_US_LETTER_HEIGHT 279.0
#define PG_US_LEGAL_WIDTH 216.0
#define PG_US_LEGAL_HEIGHT 356.0
#define PG_US_EXECUTIVE_WIDTH 191.0
#define PG_US_EXECUTIVE_HEIGHT 254.0

struct PageFormatInfo {
    KoPageFormat::Format format;
    QPageSize pageSize;
    const char *shortName; // Short name
    KLazyLocalizedString descriptiveName; // Full name
    qreal width; // in mm
    qreal height; // in mm
};

// NOTES:
// - the width and height of non-ISO formats are rounded
// http://en.wikipedia.org/wiki/Paper_size can help
// - the comments "should be..." indicates the exact values if the inch sizes would be multiplied by 25.4 mm/inch

const PageFormatInfo pageFormatInfo[] = {
    {KoPageFormat::IsoA3Size, QPageSize::A3, "A3", kli18nc("Page size", "ISO A3"), 297.0, 420.0},
    {KoPageFormat::IsoA4Size, QPageSize::A4, "A4", kli18nc("Page size", "ISO A4"), 210.0, 297.0},
    {KoPageFormat::IsoA5Size, QPageSize::A5, "A5", kli18nc("Page size", "ISO A5"), 148.0, 210.0},
    {KoPageFormat::UsLetterSize, QPageSize::Letter, "Letter", kli18nc("Page size", "US Letter"), 215.9, 279.4},
    {KoPageFormat::UsLegalSize, QPageSize::Legal, "Legal", kli18nc("Page size", "US Legal"), 215.9, 355.6},
    {KoPageFormat::ScreenSize, QPageSize::A4, "Screen", kli18nc("Page size", "Screen"), PG_A4_HEIGHT, PG_A4_WIDTH}, // Custom, so fall back to A4
    {KoPageFormat::CustomSize, QPageSize::A4, "Custom", kli18nc("Page size", "Custom"), PG_A4_WIDTH, PG_A4_HEIGHT}, // Custom, so fall back to A4
    {KoPageFormat::IsoB5Size, QPageSize::B5, "B5", kli18nc("Page size", "ISO B5"), 182.0, 257.0},
    {KoPageFormat::UsExecutiveSize, QPageSize::Executive, "Executive", kli18nc("Page size", "US Executive"), 191.0, 254.0}, // should be 190.5 mm x 254.0 mm
    {KoPageFormat::IsoA0Size, QPageSize::A0, "A0", kli18nc("Page size", "ISO A0"), 841.0, 1189.0},
    {KoPageFormat::IsoA1Size, QPageSize::A1, "A1", kli18nc("Page size", "ISO A1"), 594.0, 841.0},
    {KoPageFormat::IsoA2Size, QPageSize::A2, "A2", kli18nc("Page size", "ISO A2"), 420.0, 594.0},
    {KoPageFormat::IsoA6Size, QPageSize::A6, "A6", kli18nc("Page size", "ISO A6"), 105.0, 148.0},
    {KoPageFormat::IsoA7Size, QPageSize::A7, "A7", kli18nc("Page size", "ISO A7"), 74.0, 105.0},
    {KoPageFormat::IsoA8Size, QPageSize::A8, "A8", kli18nc("Page size", "ISO A8"), 52.0, 74.0},
    {KoPageFormat::IsoA9Size, QPageSize::A9, "A9", kli18nc("Page size", "ISO A9"), 37.0, 52.0},
    {KoPageFormat::IsoB0Size, QPageSize::B0, "B0", kli18nc("Page size", "ISO B0"), 1030.0, 1456.0},
    {KoPageFormat::IsoB1Size, QPageSize::B1, "B1", kli18nc("Page size", "ISO B1"), 728.0, 1030.0},
    {KoPageFormat::IsoB10Size, QPageSize::B10, "B10", kli18nc("Page size", "ISO B10"), 32.0, 45.0},
    {KoPageFormat::IsoB2Size, QPageSize::B2, "B2", kli18nc("Page size", "ISO B2"), 515.0, 728.0},
    {KoPageFormat::IsoB3Size, QPageSize::B3, "B3", kli18nc("Page size", "ISO B3"), 364.0, 515.0},
    {KoPageFormat::IsoB4Size, QPageSize::B4, "B4", kli18nc("Page size", "ISO B4"), 257.0, 364.0},
    {KoPageFormat::IsoB6Size, QPageSize::B6, "B6", kli18nc("Page size", "ISO B6"), 128.0, 182.0},
    {KoPageFormat::IsoC5Size, QPageSize::C5E, "C5", kli18nc("Page size", "ISO C5"), 163.0, 229.0}, // Some sources tells: 162 mm x 228 mm
    {KoPageFormat::UsComm10Size, QPageSize::Comm10E, "Comm10", kli18nc("Page size", "US Common 10"), 105.0, 241.0}, // should be 104.775 mm x 241.3 mm
    {KoPageFormat::IsoDLSize, QPageSize::DLE, "DL", kli18nc("Page size", "ISO DL"), 110.0, 220.0},
    {KoPageFormat::UsFolioSize, QPageSize::Folio, "Folio", kli18nc("Page size", "US Folio"), 210.0, 330.0}, // should be 209.54 mm x 330.2 mm
    {KoPageFormat::UsLedgerSize, QPageSize::Ledger, "Ledger", kli18nc("Page size", "US Ledger"), 432.0, 279.0}, // should be 431.8 mm x 297.4 mm
    {KoPageFormat::UsTabloidSize, QPageSize::Tabloid, "Tabloid", kli18nc("Page size", "US Tabloid"), 279.0, 432.0}, // should be 297.4 mm x 431.8 mm
    {(KoPageFormat::Format)-1, QPageSize::LastPageSize, "", {}, -1, -1}};

QPageSize KoPageFormat::printerPageSize(KoPageFormat::Format format)
{
    if (format == ScreenSize) {
        warnOdf << "You use the page layout SCREEN. Printing in ISO A4 Landscape.";
        return QPageSize::A4;
    }
    if (format == CustomSize) {
        warnOdf << "The used page layout (Custom) is not supported by KQPrinter. Printing in A4.";
        return QPageSize::A4;
    }
    return pageFormatInfo[format].pageSize;
}

qreal KoPageFormat::width(Format format, Orientation orientation)
{
    if (orientation == Landscape)
        return height(format, Portrait);
    return pageFormatInfo[format].width;
}

qreal KoPageFormat::height(Format format, Orientation orientation)
{
    if (orientation == Landscape)
        return width(format, Portrait);
    return pageFormatInfo[format].height;
}

KoPageFormat::Format KoPageFormat::guessFormat(qreal width, qreal height)
{
    for (int i = 0; pageFormatInfo[i].format != -1; i++) {
        // We have some tolerance. 1pt is a third of a mm, this is
        // barely noticeable for a page size.
        if (qAbs(width - pageFormatInfo[i].width) < 1.0 && qAbs(height - pageFormatInfo[i].height) < 1.0)
            return pageFormatInfo[i].format;
    }
    return CustomSize;
}

QString KoPageFormat::formatString(Format format)
{
    return QString::fromLatin1(pageFormatInfo[format].shortName);
}

KoPageFormat::Format KoPageFormat::formatFromString(const QString &string)
{
    for (int i = 0; pageFormatInfo[i].format != -1; i++) {
        if (string == QString::fromLatin1(pageFormatInfo[i].shortName))
            return pageFormatInfo[i].format;
    }
    // We do not know the format name, so we have a custom format
    return CustomSize;
}

KoPageFormat::Format KoPageFormat::defaultFormat()
{
    QPageSize pageSize;
    if (QLocale().measurementSystem() == QLocale::ImperialSystem) {
        pageSize = QPageSize::Letter;
    } else {
        pageSize = QPageSize::A4;
    }
    for (int i = 0; pageFormatInfo[i].format != -1; i++) {
        if (pageFormatInfo[i].pageSize == pageSize)
            return static_cast<Format>(i);
    }
    return IsoA4Size;
}

QString KoPageFormat::name(Format format)
{
    return pageFormatInfo[format].descriptiveName.toString();
}

QStringList KoPageFormat::localizedPageFormatNames()
{
    QStringList lst;
    for (int i = 0; pageFormatInfo[i].format != -1; i++) {
        lst << pageFormatInfo[i].descriptiveName.toString();
    }
    return lst;
}

QStringList KoPageFormat::pageFormatNames()
{
    QStringList lst;
    for (int i = 0; pageFormatInfo[i].format != -1; i++) {
        lst << pageFormatInfo[i].shortName;
    }
    return lst;
}

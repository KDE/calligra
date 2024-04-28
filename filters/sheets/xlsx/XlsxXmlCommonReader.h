/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * SPDX-FileCopyrightText: 2009 Nokia Corporation and /or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */

#ifndef XLSXXMLCOMMONREADER_H
#define XLSXXMLCOMMONREADER_H

#include <KoGenStyle.h>
#include <MsooXmlReader.h>
#include <MsooXmlTheme.h>

#include <QColor>

class KoCharacterStyle;

//! A class reading MSOOXML XLSX markup
//! This is a base class implementing reading elements common to some XLSX content types.
class XlsxXmlCommonReader : public MSOOXML::MsooXmlReader
{
public:
    explicit XlsxXmlCommonReader(KoOdfWriters *writers);

    ~XlsxXmlCommonReader() override;

protected:
    QColor tintedColor(const QColor &color, qreal tint);

    KoFilter::ConversionStatus read_t();
    KoFilter::ConversionStatus read_r();
    KoFilter::ConversionStatus read_rPr();
    KoFilter::ConversionStatus read_vertAlign();
    KoFilter::ConversionStatus read_sz();
    KoFilter::ConversionStatus read_rFont();

    QColor m_currentColor;
    KoFilter::ConversionStatus read_color();

    KoFilter::ConversionStatus read_u();
    KoFilter::ConversionStatus read_strike();
    KoFilter::ConversionStatus read_b();
    KoFilter::ConversionStatus read_outline();
    KoFilter::ConversionStatus read_scheme();
    KoFilter::ConversionStatus read_i();

    KoCharacterStyle *m_currentTextStyleProperties;

    //! Used for creating style in w:pPr (style:style/@style:name attr)
    KoGenStyle m_currentTextStyle;

    MSOOXML::DrawingMLTheme *m_themes;
    QVector<QString> m_colorIndices;

private:
    void init();

    class Private;
    Private *const d;
};

#endif // XLSXXMLSHAREDSTRINGSREADER_H

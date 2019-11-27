/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef XLSXXMLCOMMONREADER_H
#define XLSXXMLCOMMONREADER_H

#include <MsooXmlTheme.h>
#include <MsooXmlReader.h>
#include <KoGenStyle.h>

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
    QColor tintedColor(const QColor& color, qreal tint);

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

    MSOOXML::DrawingMLTheme* m_themes;
    QVector<QString> m_colorIndices;

private:
    void init();

    class Private;
    Private* const d;
};

#endif //XLSXXMLSHAREDSTRINGSREADER_H

/*
 * This file is part of Office 2007 Filters for KOffice
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef DOCXXMLNUMBERINGREADER_H
#define DOCXXMLNUMBERINGREADER_H

#include <KoGenStyle.h>
#include <KoListLevelProperties.h>
#include <MsooXmlReader.h>
#include <QMap>

//! A context structure for DocxXmlNumberingReader
class DocxXmlNumberingReaderContext : public MSOOXML::MsooXmlReaderContext
{
public:
};

//! A class reading MSOOXML numbering markup - numbering.xml part.
class DocxXmlNumberingReader : public MSOOXML::MsooXmlReader
{
public:
    explicit DocxXmlNumberingReader(KoOdfWriters *writers);
    virtual ~DocxXmlNumberingReader();
    virtual KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext* context = 0);

protected:

    KoFilter::ConversionStatus read_numbering();
    KoFilter::ConversionStatus read_abstractNum();
    KoFilter::ConversionStatus read_lvl();
    KoFilter::ConversionStatus read_start();
    KoFilter::ConversionStatus read_numFmt();
    KoFilter::ConversionStatus read_lvlText();
    KoFilter::ConversionStatus read_num();
    KoFilter::ConversionStatus read_abstractNumId();
    KoFilter::ConversionStatus read_lvlJc();

    // Note we read pPr here for now because we are only interested in subset of pPr features
    // which can be used with lists.
    KoFilter::ConversionStatus read_pPr_numbering();
    KoFilter::ConversionStatus read_ind_numbering();
    KoFilter::ConversionStatus read_rPr_numbering();
    KoFilter::ConversionStatus read_rFonts_numbering();

    bool m_bulletStyle;

    KoGenStyle m_currentListStyle;
    KoListLevelProperties* m_currentListStyleProperties;

    QMap<QString, KoGenStyle> m_abstractListStyles;

    QString m_bulletCharacter;
    QString m_bulletFont;

private:
    void init();
    class Private;
    Private* const d;
};

#endif //DOCXXMLNUMBERINGREADER_H

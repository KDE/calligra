/*
 * This file is part of Office 2007 Filters for Calligra
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

#include <QMap>

#include "DocxXmlDocumentReader.h"

//! A class reading MSOOXML numbering markup - numbering.xml part.
class DocxXmlNumberingReader : public DocxXmlDocumentReader
{
public:
    explicit DocxXmlNumberingReader(KoOdfWriters *writers);
    ~DocxXmlNumberingReader() override;
    KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext* context = 0) override;

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
    KoFilter::ConversionStatus read_suff();
    KoFilter::ConversionStatus read_lvlPicBulletId();
    KoFilter::ConversionStatus read_numPicBullet();
    KoFilter::ConversionStatus read_lvlOverride();

    // NOTE: We read pPr here for now because we are only interested in subset
    // of pPr features which can be used with lists.
    KoFilter::ConversionStatus read_pPr_numbering();
    KoFilter::ConversionStatus read_ind_numbering();
/*     KoFilter::ConversionStatus read_rPr_numbering(); */
/*     KoFilter::ConversionStatus read_rFonts_numbering(); */
/*     KoFilter::ConversionStatus read_color_numbering(); */

    bool m_bulletStyle;

    QMap<QString, QList<MSOOXML::Utils::ParagraphBulletProperties> > m_abstractListStyles;
    QMap<QString, QString> m_picBulletPaths;

    QString m_bulletCharacter;
    QString m_currentAbstractId;

private:
    void init();
    class Private;
    Private* const d;
};

#endif //DOCXXMLNUMBERINGREADER_H

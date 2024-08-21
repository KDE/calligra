/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef MSOOXMLDRAWINGTABLESTYLEREADER_H
#define MSOOXMLDRAWINGTABLESTYLEREADER_H

#include "komsooxml_export.h"

#include "MsooXmlCommonReader.h"
#include "MsooXmlDrawingTableStyle.h"

#include <QMap>

/**
 *   The following classes deal with the creation of the table styles part,
 *   specifically, we deal with the elements that start at the a:tblStyleLst §20.1.4.2.27,
 *   you can find its part definition at Table Styles Part §14.2.9
 */
namespace MSOOXML
{

class DrawingMLTheme;
class MsooXmlImport;

class KOMSOOXML_EXPORT MsooXmlDrawingTableStyleContext : public MSOOXML::MsooXmlReaderContext
{
public:
    MsooXmlDrawingTableStyleContext(MSOOXML::MsooXmlImport *_import,
                                    const QString &_path,
                                    const QString &_file,
                                    MSOOXML::DrawingMLTheme *_themes,
                                    QMap<QString, MSOOXML::DrawingTableStyle *> *_styleList,
                                    QMap<QString, QString> _colorMap);
    ~MsooXmlDrawingTableStyleContext() override;

    QMap<QString, MSOOXML::DrawingTableStyle *> *styleList;

    // Those members are used by some methods included
    MsooXmlImport *import;
    QString path;
    QString file;
    MSOOXML::DrawingMLTheme *themes;
    QMap<QString, QString> colorMap;
};

class KOMSOOXML_EXPORT MsooXmlDrawingTableStyleReader : public MsooXmlCommonReader
{
public:
    explicit MsooXmlDrawingTableStyleReader(KoOdfWriters *writers);
    ~MsooXmlDrawingTableStyleReader() override;

    KoFilter::ConversionStatus read(MsooXmlReaderContext *context = nullptr) override;

protected:
    KoFilter::ConversionStatus read_tblStyleLst();
    KoFilter::ConversionStatus read_tblStyle();
    KoFilter::ConversionStatus read_tblBg();
    KoFilter::ConversionStatus read_band1H();
    KoFilter::ConversionStatus read_band1V();
    KoFilter::ConversionStatus read_band2H();
    KoFilter::ConversionStatus read_band2V();
    KoFilter::ConversionStatus read_firstCol();
    KoFilter::ConversionStatus read_firstRow();
    KoFilter::ConversionStatus read_lastCol();
    KoFilter::ConversionStatus read_lastRow();
    KoFilter::ConversionStatus read_neCell();
    KoFilter::ConversionStatus read_nwCell();
    KoFilter::ConversionStatus read_seCell();
    KoFilter::ConversionStatus read_swCell();
    KoFilter::ConversionStatus read_wholeTbl();
    KoFilter::ConversionStatus read_tcStyle();
    KoFilter::ConversionStatus read_tcTxStyle();
    KoFilter::ConversionStatus read_bottom();
    KoFilter::ConversionStatus read_left();
    KoFilter::ConversionStatus read_right();
    KoFilter::ConversionStatus read_top();
    KoFilter::ConversionStatus read_insideV();
    KoFilter::ConversionStatus read_insideH();
    KoFilter::ConversionStatus read_tl2br();
    KoFilter::ConversionStatus read_tr2bl();
    KoFilter::ConversionStatus read_tcBdr();
    KoFilter::ConversionStatus read_fill();

#include "MsooXmlDrawingMLShared.h"

private:
    MsooXmlDrawingTableStyleContext *m_context;

    DrawingTableStyle *m_currentStyle;
    TableStyleProperties *m_currentTableStyleProperties;
};

}

#endif // MSOOXMLDRAWINGTABLESTYLEREADER_H

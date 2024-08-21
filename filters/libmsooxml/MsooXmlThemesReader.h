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

#ifndef MSOOXMLTHEMESREADER_H
#define MSOOXMLTHEMESREADER_H

#include <QColor>
#include <QHash>
#include <QMap>

#include <KoGenStyles.h>

#include "MsooXmlCommonReader.h"
#include "MsooXmlImport.h"
#include "MsooXmlTheme.h"

namespace MSOOXML
{

//! Context for MsooXmlThemesReader::read()
class KOMSOOXML_EXPORT MsooXmlThemesReaderContext : public MsooXmlReaderContext
{
public:
    MsooXmlThemesReaderContext(DrawingMLTheme &t,
                               MSOOXML::MsooXmlRelationships *rel,
                               MSOOXML::MsooXmlImport *imp,
                               const QString &pathName,
                               const QString &fileName);
    DrawingMLTheme *const theme;
    MSOOXML::MsooXmlRelationships *relationships;
    MSOOXML::MsooXmlImport *import;
    QString path;
    QString file;
};

//! A class reading MSOOXML themes markup - theme/theme1.xml.
/*! @todo generalize for other MSOOXML subformats.
 */
class KOMSOOXML_EXPORT MsooXmlThemesReader : public MSOOXML::MsooXmlCommonReader
{
public:
    //! Creates MsooXmlThemesReader object.
    //! On successful reading, @a theme will be written with theme definition.
    explicit MsooXmlThemesReader(KoOdfWriters *writers);

    ~MsooXmlThemesReader() override;

    //! Reads/parses the file. The output goes mainly to KoGenStyles* KoOdfWriters::mainStyles
    KoFilter::ConversionStatus read(MsooXmlReaderContext *context = nullptr) override;

protected:
    KoFilter::ConversionStatus readInternal();
    KoFilter::ConversionStatus read_theme();
    KoFilter::ConversionStatus read_themeElements();
    //! @todo no CASE
    KoFilter::ConversionStatus read_objectDefaults();
    KoFilter::ConversionStatus read_custClrLst();
    KoFilter::ConversionStatus read_extraClrSchemeLst();
    KoFilter::ConversionStatus read_extraClrScheme();

    KoFilter::ConversionStatus read_clrScheme();
    KoFilter::ConversionStatus read_color(); //!< helper
    DrawingMLColorSchemeItemBase *m_currentColor_local; //!< used by *Clr()

    KoFilter::ConversionStatus read_fmtScheme();
    KoFilter::ConversionStatus read_fontScheme();
    KoFilter::ConversionStatus read_clrMap();

    KoFilter::ConversionStatus fillStyleReadHelper(int &index);
    KoFilter::ConversionStatus read_bgFillStyleLst();
    KoFilter::ConversionStatus read_fillStyleLst();
    KoFilter::ConversionStatus read_majorFont();
    KoFilter::ConversionStatus read_minorFont();
    KoFilter::ConversionStatus read_lnStyleLst();

    //! Used for skipping a subtree - just reads and shows each element.
    //! called by BIND_READ_SKIP() macro.
    KoFilter::ConversionStatus read_SKIP();

#include "MsooXmlDrawingMLShared.h"

    KoFilter::ConversionStatus read_srgbClr_local();
    KoFilter::ConversionStatus read_sysClr_local();

private:
    void init();

    MsooXmlThemesReaderContext *m_context;

    typedef KoFilter::ConversionStatus (MsooXmlThemesReader::*ReadMethod)();
    QHash<QString, ReadMethod> m_readMethods;
    QHash<QString, QString> m_colorSchemeIndices;
    bool m_clrScheme_initialized;
    bool m_color_initialized;

    MSOOXML::MsooXmlRelationships *m_relationships;
    MSOOXML::MsooXmlImport *m_import;
    QString m_path;
    QString m_file;
};

} // namespace MSOOXML

#endif // MSOOXMLTHEMESREADER_H

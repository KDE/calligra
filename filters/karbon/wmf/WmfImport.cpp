/*  This file is part of the KDE project
    SPDX-FileCopyrightText: 2000 S.R.Haque <shaheedhaque@hotmail.com>.
    SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>

    SPDX-License-Identifier: LGPL-2.0-or-later

DESCRIPTION
*/

#include "WmfImport.h"
#include "WmfImportParser.h"

#include <KoFilterChain.h>
#include <KoXmlWriter.h>

#include <KPluginFactory>

#include <QFile>

K_PLUGIN_FACTORY_WITH_JSON(WMFImportFactory, "calligra_filter_wmf2svg.json", registerPlugin<WMFImport>();)

WMFImport::WMFImport(QObject *parent, const QVariantList &)
    : KoFilter(parent)
{
}

WMFImport::~WMFImport() = default;

KoFilter::ConversionStatus WMFImport::convert(const QByteArray &from, const QByteArray &to)
{
    if (to != "image/svg+xml" || from != "image/x-wmf")
        return KoFilter::NotImplemented;

    QFile svgFile(m_chain->outputFile());
    if (!svgFile.open(QIODevice::WriteOnly)) {
        return KoFilter::CreationError;
    }

    KoXmlWriter svgWriter(&svgFile);

    WMFImportParser wmfParser(svgWriter);
    if (!wmfParser.load(QString(m_chain->inputFile()))) {
        return KoFilter::WrongFormat;
    }

    // Do the conversion!
    if (!wmfParser.play()) {
        return KoFilter::WrongFormat;
    }

    svgFile.close();

    return KoFilter::OK;
}

#include <WmfImport.moc>

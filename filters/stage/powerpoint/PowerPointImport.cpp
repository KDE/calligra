/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005 Yolla Indria <yolla.indria@gmail.com>
   SPDX-FileCopyrightText: 2010 KO GmbH <jos.van.den.oever@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include <KoFilterChain.h>
#include <KoOdf.h>
#include <PowerPointImport.h>

#include "PptToOdp.h"

#include <KPluginFactory>

K_PLUGIN_FACTORY_WITH_JSON(PowerPointImportFactory, "calligra_filter_ppt2odp.json", registerPlugin<PowerPointImport>();)

KoFilter::ConversionStatus PowerPointImport::convert(const QByteArray &from, const QByteArray &to)
{
    if (from != "application/vnd.ms-powerpoint")
        return KoFilter::NotImplemented;

    if (to != KoOdf::mimeType(KoOdf::Presentation))
        return KoFilter::NotImplemented;

    PptToOdp ppttoodp([this](int progress) {
        setProgress(progress);
    });
    return ppttoodp.convert(m_chain->inputFile(), m_chain->outputFile(), KoStore::Zip);
}

void PowerPointImport::setProgress(const int percent)
{
    Q_EMIT sigProgress(percent);
}
#include "PowerPointImport.moc"

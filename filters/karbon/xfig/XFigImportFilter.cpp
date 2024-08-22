/* This file is part of the Calligra project, made within the KDE community.

   SPDX-FileCopyrightText: 2012 Friedrich W. H. Kossebau <kossebau@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "XFigImportFilter.h"

// filter
#include "XFigDocument.h"
#include "XFigOdgWriter.h"
#include "XFigParser.h"
// Calligra core
#include <KoFilterChain.h>
#include <KoOdf.h>
#include <KoStore.h>
// KF5
#include <KPluginFactory>
// Qt
#include <QFile>

K_PLUGIN_FACTORY_WITH_JSON(XFigImportFactory, "calligra_filter_xfig2odg.json", registerPlugin<XFigImportFilter>();)

XFigImportFilter::XFigImportFilter(QObject *parent, const QVariantList &)
    : KoFilter(parent)
{
}

XFigImportFilter::~XFigImportFilter() = default;

KoFilter::ConversionStatus XFigImportFilter::convert(const QByteArray &from, const QByteArray &to)
{
    if ((from != "image/x-xfig") || (to != "application/vnd.oasis.opendocument.graphics")) {
        return KoFilter::NotImplemented;
    }

    // prepare input
    QFile inputFile(m_chain->inputFile());
    if (!inputFile.open(QIODevice::ReadOnly)) {
        return KoFilter::FileNotFound;
    }

    // prepare output
    KoStore *outputStore = KoStore::createStore(m_chain->outputFile(), KoStore::Write, KoOdf::mimeType(KoOdf::Graphics), KoStore::Zip);
    if (!outputStore) {
        return KoFilter::StorageCreationError;
    }

    XFigOdgWriter odgWriter(outputStore);

    // translate!
    XFigDocument *document = XFigParser::parse(&inputFile);
    if (!document) {
        return KoFilter::CreationError;
    }

    const bool isWritten = odgWriter.write(document);

    delete document;

    return isWritten ? KoFilter::OK : KoFilter::CreationError;
}

#include "XFigImportFilter.moc"

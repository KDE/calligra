/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Ariya Hidayat <ariya@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <WPGImport.h>

#include <KoFilterChain.h>

#include <KPluginFactory>

#include <QDebug>
#include <QFile>
#include <QLoggingCategory>
#include <QString>

#include <librevenge-stream/librevenge-stream.h>
#include <librevenge/librevenge.h>

#include <libwpg/libwpg.h>

#include <cassert>
#include <iostream>

K_PLUGIN_FACTORY_WITH_JSON(WPGImportFactory, "calligra_filter_wpg2svg.json", registerPlugin<WPGImport>();)

const QLoggingCategory &WPG_LOG()
{
    static const QLoggingCategory category("calligra.filter.wpg2svg");
    return category;
}

#define debugWpg qCDebug(WPG_LOG)
#define warnWpg qCWarning(WPG_LOG)
#define errorWpg qCCritical(WPG_LOG)

WPGImport::WPGImport(QObject *parent, const QVariantList &)
    : KoFilter(parent)
{
}

WPGImport::~WPGImport()
{
}

KoFilter::ConversionStatus WPGImport::convert(const QByteArray &from, const QByteArray &to)
{
    if (from != "application/x-wpg")
        return KoFilter::NotImplemented;

    if (to != "image/svg+xml")
        return KoFilter::NotImplemented;

    librevenge::RVNGInputStream *input = new librevenge::RVNGFileStream(m_chain->inputFile().toLocal8Bit());
    if (input->isStructured()) {
        librevenge::RVNGInputStream *olestream = input->getSubStreamByName("Anything");
        if (olestream) {
            delete input;
            input = olestream;
        }
    }
    if (!libwpg::WPGraphics::isSupported(input)) {
        warnWpg << "ERROR: Unsupported file format (unsupported version) or file is encrypted!";
        delete input;
        return KoFilter::NotImplemented;
    }

    ::librevenge::RVNGStringVector output;
    librevenge::RVNGSVGDrawingGenerator generator(output, "");

    if (!libwpg::WPGraphics::parse(input, &generator)) {
        warnWpg << "ERROR: SVG Generation failed!";
        delete input;
        return KoFilter::ParsingError;
    }
    assert(1 == output.size());

    delete input;

    QFile outputFile(m_chain->outputFile());
    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        warnWpg << "ERROR: Could not open output file" << m_chain->outputFile();
        return KoFilter::InternalError;
    }
    outputFile.write(
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" "
        "\"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n");
    outputFile.write(output[0].cstr());
    outputFile.close();

    return KoFilter::OK;
}

#include <WPGImport.moc>

/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002 The Karbon Developers

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "EpsImport.h"
#include "PsCommentLexer.h"

#include <QDebug>
#include <QFile>
#include <QLoggingCategory>
#include <QString>

#include <KoFilter.h>
#include <KoFilterChain.h>

#include <KPluginFactory>
#include <kshell.h>

K_PLUGIN_FACTORY_WITH_JSON(EpsImportFactory, "calligra_filter_eps2svgai.json", registerPlugin<EpsImport>();)

const QLoggingCategory &EPS_LOG()
{
    static const QLoggingCategory category("calligra.filter.eps2svgai");
    return category;
}

#define debugEps qCDebug(EPS_LOG)
#define warnEps qCWarning(EPS_LOG)
#define errorEps qCCritical(EPS_LOG)

EpsImport::EpsImport(QObject *parent, const QVariantList &)
    : KoFilter(parent)
{
    debugEps << "###   ###   EPS Import Filter";
}

EpsImport::~EpsImport() = default;

KoFilter::ConversionStatus EpsImport::convert(const QByteArray &from, const QByteArray &to)
{
    if (from != "image/x-eps" && from != "image/eps" && from != "application/eps" && from != "application/x-eps" && from != "application/postscript") {
        return KoFilter::NotImplemented;
    }

    if (to == "image/svg+xml") {
        // Copy filenames
        QString input = m_chain->inputFile();
        QString output = m_chain->outputFile();

        QString command = QLatin1String("pstoedit -f plot-svg ") + KShell::quoteArg(input) + QLatin1Char(' ') + KShell::quoteArg(output);

        debugEps << "command to execute is (%s)" << QFile::encodeName(command).data();

        // Execute it:
        if (!system(QFile::encodeName(command)))
            return KoFilter::OK;
        else
            return KoFilter::StupidError;
    }
    if (to == "application/illustrator") {
        // Copy input filename:
        QString input = m_chain->inputFile();

        // EPS original bounding box
        int llx = -1, lly = -1, urx = -1, ury = -1;
        BoundingBoxExtractor extractor;

        QFile file(input);

        if (file.open(QIODevice::ReadOnly)) {
            extractor.parse(file);
            llx = extractor.llx();
            lly = extractor.lly();
            urx = extractor.urx();
            ury = extractor.ury();
            file.close();
        } else
            debugEps << "file could not be opened";

        // sed filter
        QString sedFilter = QString("sed -e \"s/%%BoundingBox: 0 0 612 792/%%BoundingBox: %1 %2 %3 %4/g\"").arg(llx).arg(lly).arg(urx).arg(ury);

        // Build ghostscript call to convert ps/eps -> ai:
        QString command = QLatin1String("gs -q -P- -dBATCH -dNOPAUSE -dSAFER -dPARANOIDSAFER -dNODISPLAY ps2ai.ps ") + KShell::quoteArg(input) + " | "
            + sedFilter + " > " + KShell::quoteArg(m_chain->outputFile());

        debugEps << "command to execute is: " << QFile::encodeName(command);

        // Execute it:
        if (!system(QFile::encodeName(command)))
            return KoFilter::OK;
        else
            return KoFilter::StupidError;
    }

    return KoFilter::NotImplemented;
}

#include "EpsImport.moc"

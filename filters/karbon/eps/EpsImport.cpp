/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "EpsImport.h"
#include "PsCommentLexer.h"

#include <QString>
#include <QFile>
#include <QDebug>
#include <QLoggingCategory>

#include <KoFilter.h>
#include <KoFilterChain.h>

#include <kpluginfactory.h>
#include <kshell.h>

K_PLUGIN_FACTORY_WITH_JSON(EpsImportFactory, "calligra_filter_eps2svgai.json",
                           registerPlugin<EpsImport>();)

const QLoggingCategory &EPS_LOG()
{
    static const QLoggingCategory category("calligra.filter.eps2svgai");
    return category;
}

#define debugEps qCDebug(EPS_LOG)
#define warnEps qCWarning(EPS_LOG)
#define errorEps qCCritical(EPS_LOG)

EpsImport::EpsImport(QObject*parent, const QVariantList&)
        : KoFilter(parent)
{
    debugEps << "###   ###   EPS Import Filter";
}

EpsImport::~EpsImport()
{
}

KoFilter::ConversionStatus EpsImport::convert(const QByteArray& from, const QByteArray& to)
{
    if (from != "image/x-eps" &&
            from != "image/eps" &&
            from != "application/eps" &&
            from != "application/x-eps" &&
            from != "application/postscript") {
        return KoFilter::NotImplemented;
    }

    if (to == "image/svg+xml") {
        // Copy filenames
        QString input = m_chain->inputFile();
        QString output = m_chain->outputFile();

        QString command = QLatin1String("pstoedit -f plot-svg ") +
                KShell::quoteArg(input) + QLatin1Char(' ') +
                KShell::quoteArg(output);

        debugEps << "command to execute is (%s)" << QFile::encodeName(command).data() ;

        // Execute it:
        if (! system(QFile::encodeName(command)))
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
        QString sedFilter = QString("sed -e \"s/%%BoundingBox: 0 0 612 792/%%BoundingBox: %1 %2 %3 %4/g\"").
                            arg(llx).arg(lly).arg(urx).arg(ury);

        // Build ghostscript call to convert ps/eps -> ai:
        QString command  = QLatin1String(
            "gs -q -P- -dBATCH -dNOPAUSE -dSAFER -dPARANOIDSAFER -dNODISPLAY ps2ai.ps ") +
            KShell::quoteArg(input) +
            " | " +
            sedFilter +
            " > " +
            KShell::quoteArg(m_chain->outputFile());

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


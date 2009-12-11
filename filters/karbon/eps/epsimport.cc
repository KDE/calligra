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

#include "epsimport.h"
#include "pscommentlexer.h"

#include <QString>
#include <QFile>

#include <KoFilter.h>
#include <KoFilterChain.h>

#include <KGenericFactory>
#include <KRun>
#include <KProcess>
#include <KShell>
#include <KDebug>

typedef KGenericFactory<EpsImport> EpsImportFactory;
K_EXPORT_COMPONENT_FACTORY(libkarbonepsimport, EpsImportFactory("kofficefilters"))

EpsImport::EpsImport(QObject*parent, const QStringList&)
        : KoFilter(parent)
{
    kDebug() << "###   ###   EPS Import Filter";
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

        QString command("pstoedit -f plot-svg ");
        command += KShell::quoteArg(input);
        command += ' ';
        command += KShell::quoteArg(output);

        kDebug() << "command to execute is (%s)" << QFile::encodeName(command).data() ;

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
            qDebug("file could not be opened");

        // sed filter
        QString sedFilter = QString("sed -e \"s/%%BoundingBox: 0 0 612 792/%%BoundingBox: %1 %2 %3 %4/g\"").
                            arg(llx).arg(lly).arg(urx).arg(ury);

        // Build ghostscript call to convert ps/eps -> ai:
        QString command(
            "gs -q -P- -dBATCH -dNOPAUSE -dSAFER -dPARANOIDSAFER -dNODISPLAY ps2ai.ps ");
        command += KShell::quoteArg(input);
        command += " | ";
        command += sedFilter;
        command += " > ";
        command += KShell::quoteArg(m_chain->outputFile());

        qDebug("command to execute is (%s)", QFile::encodeName(command).data());

        // Execute it:
        if (!system(QFile::encodeName(command)))
            return KoFilter::OK;
        else
            return KoFilter::StupidError;
    }

    return KoFilter::NotImplemented;
}

#include "epsimport.moc"

